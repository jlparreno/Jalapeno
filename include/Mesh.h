#pragma once

#include <glad/glad.h>

#include "TextureManager.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <string>
#include <vector>

/**
 * @brief Represents a single drawable mesh of a 3D model
 *
 * A Mesh stores its own vertices, indices, and textures, and manages all 
 * required OpenGL buffers (VAO, VBO, EBO). Once constructed, it can be drawn 
 * using a shader program. This class is typically used by Model to represent 
 * each submesh loaded via ASSIMP, but can be used independently.
 */
class Mesh 
{
public:

    /**
     * @brief Constructs a Mesh with the provided vertex, index, and texture data.
     * 
     * @param vertices List of Vertex structs defining the mesh geometry.
     * @param indices Index buffer defining how vertices are connected.
     * @param textures_info List of TextureInfo with all the textures info (uniform_name, path, type) needed for this mesh
     *
     * Initializes the mesh data and configures the OpenGL buffers required for rendering.
     */
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<TextureInfo>& textures_info);

    /**
     * @brief Constructs a Mesh with the provided vertex and index data.
     *
     * @param vertices List of Vertex structs defining the mesh geometry.
     * @param indices Index buffer defining how vertices are connected.
     *
     * Initializes the mesh data and configures the OpenGL buffers required for rendering.
     */
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    /**
     * @brief Renders the mesh using a given shader program.
     * 
     * @param shader Pointer to the ShaderProgram used to draw the mesh.
     *
     * Binds the VAO, submits the draw call, and restores OpenGL state.
     */
    void draw() const;

    /**
     * @brief Add new texture info to the mesh textures list
     *
     * @param uniform_nam String with the name of the uniform to bind the texture to.
     * @param path Path to the texture file in disk.
     *
     * Add new texture info to the list of textures that will be loaded later for render.
     */
    void add_textures_info(const std::string& uniform_name, const std::string& path);

    /**
     * @brief Returns the information for each texture used by this mesh
     *
     * @return A vector of TextureInfo containing the information for each texture
     */
    std::vector<TextureInfo> get_textures_info() const { return m_textures_info; };

private:

    /**
     * @brief Initializes VAO, VBO, and EBO, and configures vertex attributes.
     *
     * This function generates and binds the necessary OpenGL buffer objects,
     * uploads vertex/index data to the GPU, and defines attribute pointers
     * for vertex positions, normals, UVs, tangents, etc.
     */
    void setup_buffers();

    // Mesh data
    std::vector<Vertex>       m_vertices;
    std::vector<unsigned int> m_indices;

    // Textures data containing name of the uniform and path to the texture file
    std::vector<TextureInfo>  m_textures_info;

    // OpenGL Render Data
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
};

