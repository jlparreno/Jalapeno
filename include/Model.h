#pragma once

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <spdlog/spdlog.h>

#include "Renderable.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Material.h"
#include "PhongMaterial.h"
#include "PBRMaterial.h"
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
class Model : public Renderable
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
     * @brief Draws the model using the provided shader program.
     *
     * @param shader Pointer to the ShaderProgram used to draw the mesh.
     * 
     * Iterates over all stored meshes and calls their draw function.
     */
    void draw(ShaderProgram* shader) const override;

    /**
     * @brief Returns the model name.
     *
     * @return Model name string.
     */
    std::string get_name() const { return m_name; };

    const std::vector<Mesh>& get_meshes() const override { return m_meshes; }

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
     * @brief Gets all material textures of a specific type, loads them and creates the material
     *
     * @param mat Pointer to the aiMaterial containing textures.
     * @param type Type of texture to load (e.g., diffuse, specular).
     * @param type_name Name identifier for the texture type.
     * @param vertical_flip Wether the textures should be flipper vertically or not.
     */
    void load_material_textures(aiMaterial* mat, const std::string& material_name, aiTextureType type, const std::string& type_name, bool vertical_flip = false);


    // MODEL DATA

    // Model name
    std::string           m_name;
    
    // Directory path of the model file
    std::string           m_directory;

    // Original format of model (OBJ, GLTF...)
    std::string           m_format;

    // All the meshes of the model
    std::vector<Mesh>     m_meshes;
};

