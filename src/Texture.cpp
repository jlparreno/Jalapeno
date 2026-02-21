#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <spdlog/spdlog.h>

#include "Texture.h"


Texture::Texture()
{
    glGenTextures(1, &m_id);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

bool Texture::load_image(const std::string& path, bool vertical_flip, bool generate_mipmaps)
{
    // Flip the image vertically if required
    stbi_set_flip_vertically_on_load(vertical_flip);

    // STB load from disk
    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
    if (!data) 
    {
        SPDLOG_ERROR("Failed to load texture with STB: {}", path.c_str());
        return false;
    }

    //Set texture path variable
    m_path = path;

    // Select format depending on channels number
    GLenum format = (m_channels == 4) ? GL_RGBA : GL_RGB;

    
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);

    // Generate mipmaps if required
    if (generate_mipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Recommended parameters by default
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Free stb loaded data
    stbi_image_free(data);

    return true;
}

bool Texture::load_cubemap(const std::vector<std::string>& paths, bool vertical_flip, bool generate_mipmaps)
{
    // Flip the image vertically if required
    stbi_set_flip_vertically_on_load(vertical_flip);

    // Bind textures as cubemap
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    int width, height, channels;
    for (unsigned int i = 0; i < paths.size(); i++)
    {
        unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &channels, 0);
        if (!data)
        {
            SPDLOG_ERROR("Failed to load texture with STB: {}", paths[i].c_str());
            return false;
        }
        
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    // Recommended parameters by default
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}

void Texture::set_parameter(GLenum parameter, GLint value)
{
    // Bind the texture first (required to set the parameter
    glBindTexture(GL_TEXTURE_2D, m_id);

    // Try to apply parameter and check for errors
    glTexParameteri(GL_TEXTURE_2D, parameter, value);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) 
    {
        SPDLOG_ERROR("glTexParameteri failed with error: {}", error);
    }
}

void Texture::set_type(const std::string& type)
{
    if (type != "diffuse" && type != "normal" && type != "specular" && type != "height")
    {
        SPDLOG_ERROR("Trying to set invalid type for texture");
        return;
    }

    m_type = type;
}

void Texture::bind(unsigned int slot) const 
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}
