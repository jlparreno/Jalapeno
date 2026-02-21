#pragma once

#include <glad/glad.h>

#include "TextureManager.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "types.h"

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
     * @param textures List of textures applied to the mesh (diffuse, specular, etc.).
     *
     * Initializes the mesh data and configures the OpenGL buffers required for rendering.
     */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);

    /**
     * @brief Renders the mesh using a given shader program.
     * 
     * @param shader Pointer to the ShaderProgram used to draw the mesh.
     *
     * Binds all necessary textures and the VAO, submits the draw call, and
     * restores OpenGL state afterward.
     */
    void draw(ShaderProgram* shader) const;

private:

    // Mesh data
    std::vector<Vertex>       m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture*>     m_textures;

    // OpenGL Render Data
    GLuint m_VAO; 
    GLuint m_VBO;
    GLuint m_EBO;

    /**
     * @brief Initializes VAO, VBO, and EBO, and configures vertex attributes.
     *
     * This function generates and binds the necessary OpenGL buffer objects,
     * uploads vertex/index data to the GPU, and defines attribute pointers
     * for vertex positions, normals, UVs, tangents, etc.
     */
    void setup_buffers();
};

