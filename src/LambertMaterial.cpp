#include "LambertMaterial.h"

LambertMaterial::LambertMaterial(const std::string& name) : 
	Material(name, MaterialType::Lambert)
{
    m_shader = ShaderManager::instance().get_program("lambert");
    
    if (!m_shader)
    {
        SPDLOG_WARN("Lambert shader not found for the material: {}", name);
    }
}

void LambertMaterial::apply_uniforms(ShaderProgram* shader) const
{
    shader->set_uniform("material.ambient_color", m_ambient_color);
    shader->set_uniform("material.diffuse_color", m_diffuse_color);

    bind_textures(shader);
}