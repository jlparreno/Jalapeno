#include "Model.h"

Model::Model(const std::string& name, const std::string& path, bool gamma) : m_name(name), m_gamma_correction(gamma)
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

    // Retrieve the directory path, including last slash
    m_directory = path.substr(0, path.find_last_of('/') + 1);

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
    std::vector<Texture*>     textures;

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

    // Materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // Diffuse Maps
    std::vector<Texture*> diffuseMaps = load_material_textures(material, aiTextureType_DIFFUSE, "diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // Specular Maps
    std::vector<Texture*> specularMaps = load_material_textures(material, aiTextureType_SPECULAR, "specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // Normal Maps
    std::vector<Texture*> normalMaps = load_material_textures(material, aiTextureType_HEIGHT, "normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // Height Maps
    std::vector<Texture*> heightMaps = load_material_textures(material, aiTextureType_AMBIENT, "height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // Return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures);
}

std::vector<Texture*> Model::load_material_textures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture*> textures;

    // Iterate material textures
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        // Get texture path with Assimp
        aiString str;
        mat->GetTexture(type, i, &str);

        // Get texture name: texture name from file path
        std::filesystem::path texture_path{ str.C_Str() };
        std::string texture_name = texture_path.stem().string();

        // Try to load texture into the Manager. 
        // If it exists, the manager will return the pointer to the existing texture previously loaded.
        Texture* texture = TextureManager::instance().load_texture(texture_name, m_directory + str.C_Str(), true);

        if (texture)
        {
            texture->set_type(typeName);

            // Add the texture to the list
            textures.push_back(texture);
        }
    }

    return textures;
}
