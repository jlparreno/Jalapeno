#pragma once

#include <glad/glad.h>

#include "ShaderProgram.h"
#include "Texture.h"
#include "Material.h"

#include <string>
#include <vector>

/**
 * @brief Represents a single drawable mesh of a 3D model
 *
 * A Mesh stores its own vertices, indices and material, and manages all 
 * required OpenGL buffers (VAO, VBO, EBO). Once constructed, it can be drawn 
 * using a shader program. This class is typically used by Model to represent 
 * each submesh loaded via ASSIMP, but can be used independently.
 */
class Mesh 
{
public:

    Mesh() = default;

    /**
     * @brief Constructs a Mesh with the provided vertex and index data.
     *
     * @param vertices List of Vertex structs defining the mesh geometry.
     * @param indices Index buffer defining how vertices are connected.
     * @param material Pointer to a material to apply to the model
     *
     * Initializes the mesh data and configures the OpenGL buffers required for rendering.
     */
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* material);

    /**
     * @brief Draws the mesh applying all material uniforms to the given shader
     *
     * @param shader The shader program to upload material uniforms to
     */
    void draw(ShaderProgram* shader) const;

    /**
     * @brief Draws the mesh geometry without applying any material uniforms
     * 
     * Used in passes such as the ShadowPass where material properties are irrelevant.
     */
    void draw_geometry() const;

    /**
     * @brief Sets the material used to render this mesh
     *
     * The mesh does not take ownership of the material. The caller is responsible
     * for ensuring the material remains valid for the lifetime of this mesh.
     *
     * @param material Pointer to the material to assign to this mesh
     */
    void set_material(Material* material) { m_material = material; }

    /**
     * @brief Returns the material configured in the mesh
     *
     * @return Material pointer
     */
    Material* get_material() const { return m_material; }

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

    // Material used to draw the mesh
    Material* m_material;

    // OpenGL Render Data
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
};

