#pragma once

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <spdlog/spdlog.h>

#include "TextureManager.h"
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Material.h"
#include "types.h"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

/**
 * @brief Represents a 3D model composed of multiple meshes.
 *
 * This class loads a 3D model from file using the ASSIMP library, processes its
 * nodes and meshes recursively, and stores all resulting meshes for rendering.
 * It handles material textures, supports optional gamma correction, and can
 * draw itself using a supplied shader program.
 */
class Model
{
public:

    /**
     * @brief Constructs a new Model and loads it from a file.
     *
     * @param name Unique name for the model
     * @param path Filepath to the 3D model
     *
     * Loads the model immediately upon construction, including meshes and
     * materials.
     */
    Model(const std::string& name, const std::string& path);

    /**
     * @brief Loads a model from a file and stores its meshes
     *
     * @param path Filepath to the model
     *
     * Supports all file formats provided by ASSIMP. The meshes vector and
     * internal directory path are populated.
     */
    void load_model(const std::string& path);

    /**
     * @brief Draws the model using the provided shader program.
     *
     * Iterates over all stored meshes and calls their draw function.
     */
    void draw() const;

    /**
     * @brief Returns the model name.
     *
     * @return Model name string.
     */
    std::string get_name() const { return m_name; };

    /**
     * @brief Sets the material for drawing this model.
     *
     * @param material Pointer to the material to apply to the model.
     * 
     * Loads all the textures associated to the material if they haven't been loaded previously
     */
    void set_material(Material* material);
    
    /**
     * @brief Returns the material configured in the model
     *
     * @return Material pointer.
     */
    Material* get_material() const { return m_material; }

    /**
     * @brief Sets a new position for the model.
     *
     * @param pos New position of the model.
     */
    void set_position(const glm::vec3& pos) { m_position = pos; }

    /**
     * @brief Sets new rotation values for the model.
     *
     * @param rot New rotation values, euler angles, for each axis.
     */
    void set_rotation(const glm::vec3& rot) { m_rotation = rot; }

    /**
     * @brief Sets a new scale value for the model.
     *
     * @param scale New scale values, per axis
     */
    void set_scale(const glm::vec3& scale) { m_scale = scale; }


    /**
     * @brief Gets the model matrix of the model using the transforms configured.
     *
     * @return Transform matrix for this model.
     */
    glm::mat4 get_model_matrix() const;

private:

    /**
     * @brief Recursively processes an ASSIMP node and its children in the scene graph
     *
     * @param node Pointer to the current aiNode.
     * @param scene Pointer to the aiScene containing the full model.
     *
     * For each node, all meshes are processed, and the function is called
     * recursively for all child nodes.
     */
    void process_node(aiNode* node, const aiScene* scene);

    /**
     * @brief Processes a single mesh from ASSIMP and converts it to the internal Mesh format.
     *
     * @param mesh Pointer to the aiMesh to process.
     * @param scene Pointer to the aiScene containing the mesh.
     * 
     * @return Mesh The processed mesh ready for rendering.
     */
    Mesh process_mesh(aiMesh* mesh, const aiScene* scene);

    /**
     * @brief Gets all material textures info of a specific type for a mesh.
     *
     * @param mat Pointer to the aiMaterial containing textures.
     * @param type Type of texture to load (e.g., diffuse, specular).
     * @param typeName Name identifier for the texture type.
     * 
     * @return std::vector<TextureInfo> Vector of TextureInfo containing the uniform name and the path of the texture
     */
    std::vector<TextureInfo> get_material_textures_info(aiMaterial* mat, aiTextureType type, std::string type_name);


    // MODEL DATA

    // Model name
    std::string           m_name;
    
    // Directory path of the model file
    std::string           m_directory;

    // All the meshes of the model
    std::vector<Mesh>     m_meshes;

    // Material used to draw the model
    Material*             m_material;

    // Transforms for this model
    glm::vec3             m_position;
    glm::vec3             m_rotation;
    glm::vec3             m_scale;
};

