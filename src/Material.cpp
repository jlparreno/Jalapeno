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
    int unit = 0;
    for (const auto& [uniform_name, tex] : m_textures)
    {
        tex->bind(unit);
        shader->set_uniform(uniform_name, unit);
        unit++;
    }
}
