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
    // Flags que el shader usará para decidir qué samplear
    bool has_diffuse = m_textures.count("diffuse_tex") > 0;
    bool has_specular = m_textures.count("specular_tex") > 0;
    bool has_normal = m_textures.count("normal_tex") > 0;

    shader->set_uniform("material.use_diffuse_texture", has_diffuse);
    shader->set_uniform("material.use_specular_texture", has_specular);
    shader->set_uniform("material.use_normal_texture", has_normal);

    // Colores planos como fallback
    shader->set_uniform("material.ambient_color", m_ambient_color);
    shader->set_uniform("material.diffuse_color", m_diffuse_color);
    shader->set_uniform("material.specular_color", m_specular_color);
    shader->set_uniform("material.shininess", m_shininess);

    // Texturas si existen
    bind_textures(shader);
}