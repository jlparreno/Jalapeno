#include "PhongMaterial.h"

PhongMaterial::PhongMaterial(const std::string& name) : 
	Material(name, MaterialType::Phong)	
{
    m_shader = ShaderManager::instance().get_program("phong");

    if (!m_shader)
    {
        SPDLOG_WARN("Phong shader not found for the material: {}", name);
    }
}

void PhongMaterial::apply_uniforms(ShaderProgram* shader) const
{
    // Flat colors for no-textured cases
    shader->set_uniform("material.ambient_color", m_ambient_color);
    shader->set_uniform("material.diffuse_color", m_diffuse_color);
    shader->set_uniform("material.specular_color", m_specular_color);
    shader->set_uniform("material.shininess", m_shininess);

    // Bind textures if any
    bind_textures(shader);
}