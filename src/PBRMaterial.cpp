#include "PBRMaterial.h"

PBRMaterial::PBRMaterial(const std::string& name) : 
    Material(name, MaterialType::PBR)
{
    m_shader = ShaderManager::instance().get_program("pbr");

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
    shader->set_uniform("material.alpha_cutout", m_alpha_cutout);
    shader->set_uniform("material.alpha_threshold", m_alpha_threshold);

    // Bind PBR textures
    bind_textures(shader);
}

void PBRMaterial::bind_textures(ShaderProgram* shader) const
{
    auto& tex_mgr = TextureManager::instance();
    GLuint white = tex_mgr.get_white_texture();

    // Albedo
    glActiveTexture(GL_TEXTURE0);
    if (m_textures.count("albedo_tex"))
        glBindTexture(GL_TEXTURE_2D, m_textures.at("albedo_tex")->get_id());
    else
        glBindTexture(GL_TEXTURE_2D, TextureManager::instance().get_white_texture());
    shader->set_uniform("albedo_tex", 0);

    // Metallic/Roughness — suelen venir en un único mapa (G=roughness, B=metallic)
    glActiveTexture(GL_TEXTURE1);
    if (m_textures.count("metallic_roughness_tex"))
        glBindTexture(GL_TEXTURE_2D, m_textures.at("metallic_roughness_tex")->get_id());
    else
        glBindTexture(GL_TEXTURE_2D, TextureManager::instance().get_white_texture());
    shader->set_uniform("metallic_roughness_tex", 1);

    // Normal
    glActiveTexture(GL_TEXTURE2);
    if (m_textures.count("normal_tex"))
        glBindTexture(GL_TEXTURE_2D, m_textures.at("normal_tex")->get_id());
    else
        glBindTexture(GL_TEXTURE_2D, TextureManager::instance().get_white_texture());
    shader->set_uniform("normal_tex", 2);

    shader->set_uniform("material.use_normal_map", m_textures.count("normal_tex") > 0);
}
