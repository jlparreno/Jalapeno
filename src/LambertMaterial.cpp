#include "LambertMaterial.h"

LambertMaterial::LambertMaterial(const std::string& name) : 
	Material(name, MaterialType::Lambert)
{
    // Automatically assign the lambert shader
    m_shader = ShaderManager::instance().get_program("lambert");
    
    // Warn if the shader is not available in the ShaderManager
    if (!m_shader)
    {
        SPDLOG_WARN("Lambert shader not found for the material: {}", name);
    }
}

void LambertMaterial::apply_uniforms(ShaderProgram* shader) const
{
    // Flat colors for no-textured cases
    shader->set_uniform("material.ambient_color", m_ambient_color);
    shader->set_uniform("material.diffuse_color", m_diffuse_color);

    bind_textures(shader);
}

void LambertMaterial::bind_textures(ShaderProgram* shader) const
{
    bind_texture_or_white("diffuse_tex", 0, shader);
}