#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* material) :
    m_vertices(vertices),
    m_indices(indices),
    m_material(material),
    m_vertex_buffer(std::make_unique<VertexBuffer>(vertices, indices))
{
}

void Mesh::draw(ShaderProgram* shader) const
{
    // Check if material is valid
    if (!m_material)
    {
        SPDLOG_ERROR("Error getting the material for rendering mesh");
        return;
    }

    // Set specific material uniforms (this will bind textures too)
    m_material->apply_uniforms(shader);

    // Draw mesh
    draw_geometry();
}

void Mesh::draw_geometry() const
{
    m_vertex_buffer->draw();
}