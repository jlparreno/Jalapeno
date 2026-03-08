#include "PhongMaterial.h"

PhongMaterial::PhongMaterial(const std::string& name) : 
	Material(name, MaterialType::Phong)	
{
    // Automatically assign the phong shader
    m_shader = ShaderManager::instance().get_program("phong");

    // Warn if the shader is not available in the ShaderManager
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

    bind_textures(shader);
}

void PhongMaterial::bind_textures(ShaderProgram* shader) const
{
    bind_texture_or_white("diffuse_tex", 0, shader);
    bind_texture_or_white("specular_tex", 1, shader);
}
