#include "PBRMaterial.h"

PBRMaterial::PBRMaterial(const std::string& name) : 
    Material(name, MaterialType::PBR)
{
    // Automatically assign the PBR shader
    m_shader = ShaderManager::instance().get_program("pbr");

    // Warn if the shader is not available in the ShaderManager
    if (!m_shader)
    {
        SPDLOG_WARN("PBR shader not found for the material: {}", name);
    }
}

void PBRMaterial::apply_uniforms(ShaderProgram* shader) const
{
    // Scalar factors
    shader->set_uniform("material.albedo_color", m_albedo);
    shader->set_uniform("material.metallic", m_metallic);
    shader->set_uniform("material.roughness", m_roughness);

    // Bind PBR textures
    bind_textures(shader);
}

void PBRMaterial::bind_textures(ShaderProgram* shader) const
{
    bind_texture_or_white("albedo_tex", ALBEDO_TEX_UNIT, shader);
    bind_texture_or_white("metallic_roughness_tex", METALLIC_ROUGHNESS_TEX_UNIT, shader);
    bind_texture_or_white("normal_tex", NORMAL_TEX_UNIT, shader);

    // Set uniform flag to use normal textures or not
    shader->set_uniform("material.use_normal_map", m_textures.count("normal_tex") > 0);
}
