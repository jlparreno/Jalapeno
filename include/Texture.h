#pragma once

#include <glad/glad.h>

#include <string>
#include <vector>
#include <iostream>

/**
 * @class Texture
 * @brief Wrapper for an OpenGL 2D texture
 *
 * Provides loading from file or memory, configurable filtering and wrapping
 * parameters, and optional mipmap generation. Its purpose is to centralize
 * the management of the OpenGL texture resource and offer a simple interface
 * for use by the TextureManager or the rendering system.
 */
class Texture
{
public:

    /**
     * @brief Constructs an empty OpenGL texture object
     *
     * Generates a new OpenGL texture ID. 
     * You must call load_image() to upload actual image data.
     */
    Texture();

    /**
     * @brief Destructs the texture and frees the OpenGL resource
     *
     * Automatically calls glDeleteTextures on the texture handle
     */
    ~Texture();

    /**
     * @brief Loads a texture from an image file using stb_image
     *
     * @param path Path to the image file
     * @param vertical_flip
     * @param generate_mipmaps Whether mipmaps should be generated after upload
     * 
     * @return True on successful load, false otherwise.
     */
    bool load_image(const std::string& path, bool vertical_flip = false, bool generate_mipmaps = true);

    /**
     * @brief Loads a cubemap texture from an image file using stb_image
     *
     * @param paths Vector with all the path to the image files
     * @param vertical_flip Whether textures should be flipped vertically
     * @param generate_mipmaps Whether mipmaps should be generated after upload
     *
     * @return True on successful load, false otherwise.
     */
    bool load_cubemap(const std::vector<std::string>& paths, bool vertical_flip, bool generate_mipmaps);

    /**
     * @brief Applies an specific parameter to the texture.
     *
     * @param parameter name of the parameter to be updated (WRAP, FILTER, ...)
     * @param value value to apply to the parameter
     * 
     * This call binds the texture internally before setting parameters
     * because glTexParameteri operates on the currently bound texture.
     * Previous state is not recovered automatically.
     */
    void set_parameter(GLenum parameter, GLint value);

    /**
     * @brief Sets texture type (diffuse, normal, specular, height)
     *
     * @param type String of the type to set
     */
    void set_type(const std::string& type);

    /**
     * @brief Binds the texture to a texture unit (default = 0).
     *
     * @param unit Texture unit number (0 = GL_TEXTURE0).
     */
    void bind(unsigned int slot = 0) const;

    /**
     * @brief Returns the OpenGL texture ID.
     */
    GLuint get_id() const { return m_id; }

    /**
     * @brief Returns the texture type as a string (diffuse, specular, normal or height)
     */
    std::string get_type() const { return m_type; }

    /**
     * @brief Returns the texture width in pixels.
     */
    int width() const { return m_width; }

    /**
     * @brief Returns the texture height in pixels.
     */
    int height() const { return m_height; }

    /**
     * @brief Returns the number of channels
     */
    int channels() const { return m_channels; }

private:

    // OpenGL texture handle
    GLuint m_id{ 0 };

    // Texture type
    std::string m_type{ "diffuse" };

    // Texture type
    std::string m_path{ "" };

    // Width in pixels
    int m_width{ 0 };

    // Height in pixels
    int m_height{ 0 };

    // Number of channels
    int m_channels{ 0 };
};

