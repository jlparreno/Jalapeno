#include "Material.h"

Material::Material(const std::string& name, MaterialType type) : 
    m_name(name), 
    m_type(type)
{
}

void Material::add_texture(const std::string& name, const std::string& path, const std::string& type_name, bool vertical_flip)
{
    Texture* texture = TextureManager::instance().load_texture(name, path, vertical_flip, true);

    // At the moment only one texture per type (last loaded)
    // TODO: uniform_name = "diffuse_tex" + std::to_string(i + 1);
    std::string uniform_name{ type_name + "_tex" };
    
    m_textures[uniform_name] = texture;
}

void Material::bind_textures(ShaderProgram* shader) const
{
    auto& tex_mgr = TextureManager::instance();
    GLuint white = tex_mgr.get_white_texture();

    // Bindea diffuse — textura real o blanca como fallback
    glActiveTexture(GL_TEXTURE0);
    if (m_textures.count("diffuse_tex"))
        glBindTexture(GL_TEXTURE_2D, m_textures.at("diffuse_tex")->get_id());
    else
        glBindTexture(GL_TEXTURE_2D, white);
    shader->set_uniform("diffuse_tex", 0);

    // Bindea specular — textura real o blanca como fallback
    glActiveTexture(GL_TEXTURE1);
    if (m_textures.count("specular_tex"))
        glBindTexture(GL_TEXTURE_2D, m_textures.at("specular_tex")->get_id());
    else
        glBindTexture(GL_TEXTURE_2D, white);
    shader->set_uniform("specular_tex", 1);

    /*int unit = 0;
    for (const auto& [uniform_name, tex] : m_textures)
    {
        tex->bind(unit);
        shader->set_uniform(uniform_name, unit);
        unit++;
    }*/
}
