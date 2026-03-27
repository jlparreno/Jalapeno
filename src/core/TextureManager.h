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
     * @brief Creates and registers an empty texture under the given name
     *
     * Instantiates a default-constructed Texture with no image data and stores
     * it in the manager under the given name. Useful for creating textures that
     * will be populated later.
     *
     * @param name Unique identifier used to retrieve the texture later via get_texture()
     * 
     * @return Raw pointer to the newly created Texture
     */
    Texture* add_texture(const std::string& name);

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
     * @brief Loads an HDR texture from disk if it is not already loaded
     *
     * This function uses stbi_loadf to support HDR format. The original purpose is
     * to load environment maps for IBL, but can be used for any HDR texture.
     * 
     * @param name Unique name to identify the texture
     * @param path File path to the texture image
     * @param vertical_flip Optional parameter to flip the texture vertically. Default is false.
     *
     * @return Pointer to the loaded Texture object. Returns nullptr if loading failed.
     */
    Texture* load_hdr_texture(const std::string& name, const std::string& path, bool vertical_flip = false);

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

    /**
     * @brief Returns the default white 1x1 2D texture
     *
     * Used as a fallback when a material has no texture assigned for a given
     * slot, ensuring shaders always sample a valid texture.
     *
     * @return Raw pointer to the white fallback Texture
     */
    Texture* get_white_texture() const { return m_white_texture.get(); }

    /**
     * @brief Returns the default white 1x1 cubemap texture
     *
     * Used as a fallback to fill inactive samplerCube slots in the GeometryPass,
     * ensuring all cubemap samplers reference a valid texture.
     *
     * @return Raw pointer to the white fallback cubemap Texture
     */
    Texture* get_white_cubemap() const { return m_white_cubemap.get(); }

    /**
     * @brief Deletes a texture from the manager and releases its GPU resources
     *
     * Removes the texture identified by the given name from the internal map
     * and deletes the associated OpenGL texture object. If the name is not found,
     * logs a warning and does nothing.
     *
     * @param name Unique identifier of the texture to remove.
     */
    void remove_texture(const std::string& name);

private:

    // Private constructor to enforce singleton pattern
    TextureManager::TextureManager()
    {
        init(); //Automatically init on first call
    }

    // Private destructor to enforce singleton pattern
    ~TextureManager() = default;

    /**
     * @brief Initializes the TextureManager and creates the default white texture
     *
     * Creates a 1x1 white RGBA texture used as a fallback when a material has no
     * texture assigned to a given slot. This ensures shaders always receive a valid
     * texture binding, avoiding undefined sampling behavior.     
     */
    void init();

    // Map storing textures by their unique name
    // Uses unique_ptr to ensure proper memory management
    std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;

    // Default 1x1 white texture
    std::unique_ptr<Texture> m_white_texture;

    // Default 1x1 white cubemap
    std::unique_ptr<Texture> m_white_cubemap;
};

