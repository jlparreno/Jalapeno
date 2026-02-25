#include "Material.h"

Material::Material(const std::string& name, ShaderProgram* shader) :
    m_name(name),
    m_shader(shader)
{
}

void Material::load_textures(const std::vector<TextureInfo>& textures)
{
    for (const auto& tex_info : textures)
    {
        // Avoid duplicates
        if (m_textures.find(tex_info.uniform_name) != m_textures.end())
            continue;
        
        // Load with TextureManager to have centralized control and add the pointer to the material
        Texture* texture = TextureManager::instance().load_texture(tex_info.name, tex_info.file_path, true);
        m_textures[tex_info.uniform_name] = texture;

        SPDLOG_INFO("New texture loaded from {} material: {}", m_name, tex_info.name);
    }
}

void Material::bind_textures() const
{
    int unit = 0;
    for (const auto& [uniform_name, tex] : m_textures)
    {
        tex->bind(unit);
        m_shader->set_uniform(uniform_name, unit);
        unit++;
    }
}
