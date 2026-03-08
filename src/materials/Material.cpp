#include "Material.h"

Material::Material(const std::string& name, MaterialType type) : 
    m_name(name), 
    m_type(type),
    m_shader(nullptr)
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
    // Base case, loop the textures and bind sequentially
    int unit = 0;
    for (const auto& [uniform_name, tex] : m_textures)
    {
        tex->bind(unit);
        shader->set_uniform(uniform_name, unit);
        unit++;
    }
}

void Material::bind_texture_or_white(const std::string& name, int unit, ShaderProgram* shader) const
{
    // Find the texture in the material
    auto it = m_textures.find(name);

    // Bind the texture found or the default 1x1 white one if not
    if (it != m_textures.end())
        it->second->bind(unit);
    else
        TextureManager::instance().get_white_texture()->bind(unit);

    // Set corresponding sampler uniform
    shader->set_uniform(name, unit);
}