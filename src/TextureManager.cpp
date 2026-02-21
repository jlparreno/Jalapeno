#include "TextureManager.h"

Texture* TextureManager::load_texture(const std::string& name, const std::string& path, bool vertical_flip, bool generate_mipmaps)
{
    // If it exists, return the previously loaded texture
    if (exists(name))
    {
        return m_textures[name].get();
    }

    // Otherwise, generate the new texture and load the image from disk
    auto tex = std::make_unique<Texture>();
    if (!tex->load_image(path, vertical_flip, generate_mipmaps))
    {
        SPDLOG_ERROR("Failed to load texture: {}", path);
        return nullptr;
    }

    //Move data to the manager array
    m_textures[name] = std::move(tex);

    //Return the pointer to the new texture
    return m_textures[name].get();
}

Texture* TextureManager::load_cubemap(const std::string& name, const std::vector<std::string>& paths, bool vertical_flip, bool generate_mipmaps)
{
    // If it exists, return the previously loaded cubemap texture
    if (exists(name))
    {
        return m_textures[name].get();
    }

    // Otherwise, generate the new texture and load the image from disk
    auto tex = std::make_unique<Texture>();
    if (!tex->load_cubemap(paths, vertical_flip, generate_mipmaps))
    {
        SPDLOG_ERROR("Failed to load cubemap texture: {}", name);
        return nullptr;
    }

    //Move data to the manager array
    m_textures[name] = std::move(tex);

    //Return the pointer to the new texture
    return m_textures[name].get();
}

Texture* TextureManager::get_texture(const std::string& name) 
{
    //Look for the texture in the manager map
    auto it = m_textures.find(name);

    //If it exists, return it. Return nullptr otherwise
    if (it != m_textures.end()) 
    {
        return it->second.get();
    }

    return nullptr;
}