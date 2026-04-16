#pragma once

#include "ShaderProgram.h"
#include "Texture.h"
#include "Material.h"
#include "VertexBuffer.h"

#include <string>
#include <vector>
#include <memory>

/**
 * @brief Represents a single drawable mesh of a 3D model
 *
 * A Mesh owns its CPU-side geometry (vertices, indices) and material, and
 * holds a VertexBuffer that manages the corresponding GPU resources.
 * This separation keeps geometry data decoupled from the graphics API,
 * making it straightforward to swap the GPU backend (e.g. Vulkan).
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

    // CPU-side geometry data
    std::vector<Vertex>       m_vertices;
    std::vector<unsigned int> m_indices;

    // Material used to shade this mesh
    Material* m_material { nullptr };

    // GPU representation — owns VAO, VBO and EBO
    std::unique_ptr<VertexBuffer> m_vertex_buffer;
};

