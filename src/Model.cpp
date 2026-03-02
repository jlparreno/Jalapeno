#include "Model.h"

Model::Model(const std::string& name, const std::string& path) :
    m_name(name), 
    m_directory(""),
    m_position(glm::vec3(0.0f,0.0f,0.0f)),
    m_scale(glm::vec3(1.0f, 1.0f, 1.0f)),
    m_rotation(glm::vec3(0.0f, 0.0f, 0.0f))
{
    load_model(path);
}

void Model::draw(ShaderProgram* shader) const
{
    //Call to draw all meshes that compose the model
    for (unsigned int i = 0; i < m_meshes.size(); i++)
    {
        m_meshes[i].draw(shader);
    }
}

void Model::load_model(const std::string& path)
{
    // Read file with ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    // Check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        SPDLOG_ERROR("Failed ASSIMP importation: {}", importer.GetErrorString());
        return;
    }

    // Retrieve the directory path, including last slash, and file format
    std::filesystem::path file_path(path);
    m_directory = file_path.parent_path().string() + "/";
    m_format = file_path.extension().string();

    // Process ASSIMP's root node recursively
    process_node(scene->mRootNode, scene);

    SPDLOG_INFO("Loaded new model: {}", m_name);
}

void Model::process_node(aiNode* node, const aiScene* scene)
{
    // Process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // The node only contains indices to the actual objects in the scene.
        // The scene contains all the data
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(process_mesh(mesh, scene));
    }

    // After processing all of the meshes (if any), then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        process_node(node->mChildren[i], scene);
    }
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
    // Data to fill
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    //std::vector<TextureInfo>  textures;

    // Iterate mesh vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        // Assimp has its own vertex format. Convert input data to our internal data format
        glm::vec3 vector;

        // Positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        // Normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        else
        {
            vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        // Texture Coordinates
        if (mesh->mTextureCoords[0]) // Check if the mesh contain texture coordinates
        {
            glm::vec2 vec;
            // A vertex can contain up to 8 different texture coordinates. 
            // Here we are always taking the first set.
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.tex_coords = vec;

            // Tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;

            // Bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        }
        else
        {
            vertex.tex_coords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Iterate faces to get vertex indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        
        // Retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Load materials into Material Manager
    auto& material_mgr = MaterialManager::instance();

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    
    MaterialType material_type = material_mgr.get_material_type(material);
    std::string material_name = material->GetName().Empty() ? std::to_string(mesh->mMaterialIndex) : material->GetName().C_Str();

    switch (material_type)
    {
        // TODO: Other types, at the moment only Phong supported
        /*case MaterialType::PBR:
            mat = material_mgr.add_material<PBRMaterial>(material_name);
            break;*/

        case MaterialType::Phong:
            material_mgr.add_material<PhongMaterial>(material_name);
            break;
    }

    // Set if we want vertical flip or not. At the moment, based on file format
    bool vertical_flip = false;
    if (m_format == ".obj")
        vertical_flip = true;

    // Diffuse Maps
    load_material_textures(material, material_name, aiTextureType_DIFFUSE, "diffuse", vertical_flip);

    // Specular Maps
    load_material_textures(material, material_name, aiTextureType_SPECULAR, "specular", vertical_flip);

    // Normal Maps
    load_material_textures(material, material_name, aiTextureType_NORMALS, "normal", vertical_flip);

    // Height Maps
    load_material_textures(material, material_name, aiTextureType_HEIGHT, "height", vertical_flip);
    
    // Return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, material_mgr.get_material(material_name));
}

void Model::load_material_textures(aiMaterial* mat, const std::string& material_name, aiTextureType type, const std::string& type_name, bool vertical_flip)
{
    auto& material_mgr = MaterialManager::instance();

    // Iterate material textures
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        // Get texture path with Assimp
        aiString str;
        mat->GetTexture(type, i, &str);

        // Texture path to save loaded
        // Get texture name: texture name from file path
        std::filesystem::path texture_path{ m_directory + str.C_Str() };
        std::string texture_name = texture_path.stem().string();

        // Create or get the material and initialize texture
        Material* material = material_mgr.get_material(material_name);
        material->add_texture(texture_name, texture_path.string(), type_name, vertical_flip);
    }
}

glm::mat4 Model::get_model_matrix() const
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, m_position);
    model = glm::scale(model, m_scale);
    //model = glm::rotate(model,);

    return model;
}
