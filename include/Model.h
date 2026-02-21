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
     * @param path Filepath to the 3D model
     * @param gamma Optional gamma correction flag (default: false).
     *
     * Loads the model immediately upon construction, including meshes and
     * materials.
     */
    Model(const std::string& name, const std::string& path, bool gamma = false);


    /**
     * @brief Returns the model name.
     *
     * @return Model name string.
     */
    std::string get_name() const { return m_name; };

    /**
     * @brief Draws the model using the provided shader program.
     *
     * @param shader Pointer to a ShaderProgram used to render all meshes.
     *
     * Iterates over all stored meshes and calls their draw function.
     */
    void draw(ShaderProgram* shader) const;

private:

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
     * @brief Loads all material textures of a specific type for a mesh.
     *
     * @param mat Pointer to the aiMaterial containing textures.
     * @param type Type of texture to load (e.g., diffuse, specular).
     * @param typeName Name identifier for the texture type.
     * 
     * @return std::vector<Texture*> Vector of pointers to loaded textures, present in the TextureManager.
     *
     * Checks if textures were already loaded to avoid duplicates.
     */
    std::vector<Texture*> load_material_textures(aiMaterial* mat, aiTextureType type, std::string typeName);


    // MODEL DATA

    // Model name
    std::string           m_name;
    
    //All the meshes of the model
    std::vector<Mesh>     m_meshes;

    // Directory path of the model file
    std::string           m_directory;

    // Flag for gamma correction
    bool m_gamma_correction;
};

