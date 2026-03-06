#pragma once

#include <spdlog/spdlog.h>

#include "Texture.h"

#include <string>
#include <unordered_map>
#include <memory>

/**
 * @class TextureManager
 * @brief Singleton class for managing textures in the engine
 *
 * This class allows loading, storing, and retrieving textures by name.
 * It ensures that each texture is loaded only once
 */
class TextureManager 
{
public:

    /**
     * @brief Retrieves the singleton instance of the TextureManager
     *
     * @return Reference to the unique TextureManager instance
     */
    static TextureManager& instance() 
    {
        static TextureManager instance;
        return instance;
    }

    /**
     * @brief Deleted copy constructor to prevent copying of the singleton
     */
    TextureManager(const TextureManager&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying of the singleton
     */
    TextureManager& operator=(const TextureManager&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving of the singleton
     */
    TextureManager(TextureManager&&) = delete;

    /**
     * @brief Deleted move assignment operator to prevent moving of the singleton
     */
    TextureManager& operator=(TextureManager&&) = delete;

    /**
     * @brief Loads a texture from disk if it is not already loaded
     *
     * If the texture with the given name already exists in the manager,
     * it returns the existing texture. Otherwise, it loads the texture from
     * the specified file path.
     *
     * @param name Unique name to identify the texture
     * @param path File path to the texture image
     * @param vertical_flip Optional parameter to flip the texture vertically. Default is false.
     * @param generate_mipmaps Optional parameter to generate the mipmas for the texture. Default is true
     * 
     * @return Pointer to the loaded Texture object. Returns nullptr if loading failed.
     */
    Texture* load_texture(const std::string& name, const std::string& path, bool vertical_flip = false, bool generate_mipmaps = true);


    /**
     * @brief Loads a cubemap texture from disk if it is not already loaded
     *
     * If the texture with the given name already exists in the manager,
     * it returns the existing texture. Otherwise, it loads the texture from
     * the specified file path.
     *
     * @param name Unique name to identify the texture
     * @param paths Vector of file paths to the 6 textures of the cubemap, ordered (right, left, top, bottom, back, front)
     * @param vertical_flip Optional parameter to flip the textures vertically. Default is false.
     * @param generate_mipmaps Optional parameter to generate the mipmas for the textures. Default is true
     * 
     * @return Pointer to the loaded Texture object. Returns nullptr if loading failed.
     */
    Texture* load_cubemap(const std::string& name, const std::vector<std::string>& paths, bool vertical_flip = false, bool generate_mipmaps = true);

    /**
     * @brief Retrieves a texture by its name
     *
     * @param name Name of the texture to retrieve
     * @return Pointer to the Texture object if it exists, nullptr otherwise
     */
    Texture* get_texture(const std::string& name);

    /**
     * @brief Checks if a texture with the given name exists in the manager
     *
     * @param name Name of the texture to check
     * @return true if the texture exists, false otherwise
     */
    bool exists(const std::string& name) const 
    {
        return m_textures.find(name) != m_textures.end();
    }

    /**
     * @brief Gets all the textures loaded in the manager
     *
     * @return An map with all the textures. For each one, the key is the texture name 
     * and the value is a pointer to the Texture object
     */
    const std::unordered_map<std::string, std::unique_ptr<Texture>>& get_all_textures() const { return m_textures; }

    Texture* get_white_texture() const { return m_white_texture.get(); }

private:

    // Private constructor to enforce singleton pattern
    TextureManager::TextureManager()
    {
        init(); //Automatically init on first call
    }

    // Private destructor to enforce singleton pattern
    ~TextureManager() = default;


    void init();

    // Map storing textures by their unique name
    // Uses unique_ptr to ensure proper memory management
    std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;

    // Default 1x1 white texture
    std::unique_ptr<Texture> m_white_texture;
};

