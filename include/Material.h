#pragma once

#include <glm/glm.hpp>

#include "ShaderProgram.h"
#include "TextureManager.h"

#include <string>
#include <unordered_map>

/**
 * @class Material
 * 
 * @brief Represents a rendering material that contains multiple textures and shader parameters.
 *
 * The Material class encapsulates all textures (diffuse, normal, specular, etc.)
 * and allows derived classes to define custom uniform parameters. It manages
 * texture binding for rendering and provides an interface to query stored textures.
 */
class Material
{
public:

    /**
     * @brief Constructs a new Material with a given name and type.
     *
     * @param name Name used to identify the material.
     * @param type Type of the material (PBR, Phong...).
     */
    Material(const std::string& name, MaterialType type);

    /**
     * @brief Virtual destructor
     */
    virtual ~Material() = default;

    /**
     * @brief Add a new texture for the material through the Texture Manager
     *
     * @param name Name of the texture to load
     * @param path Path of the texture file in disk
     * @param type_name Type of the texture (diffuse, normal, specular...)
     * @param vertical_flip Wheter the texture should be flipped vertically or not
     * 
     * This method loads the texture through the Texture Manager and stores it
     * for later binding during rendering.
     */
    void add_texture(const std::string& name, const std::string& path, const std::string& type_name, bool vertical_flip = false);

    /**
     * @brief Binds all textures associated with this material to the GPU.
     *
     * @param shader Pointer to the ShaderProgram that will receive the textures.
     *
     * This function assigns available texture units sequentially and binds each
     * stored texture to the corresponding uniform in the shader. It should be
     * called before any draw call that uses this material.
     */
    void bind_textures(ShaderProgram* shader) const;

    /**
     * @brief Returns the material's identifier name.
     *
     * @return String reference used to identify the material.
     */
    const std::string& get_name() const { return m_name; }

    /**
     * @brief Returns the type of the material.
     *
     * @return The MaterialType of this material.
     */
    MaterialType get_type() const { return m_type; }

    /**
     * @brief Returns all textures stored in this material.
     *
     * @return A reference to the unordered_map containing texture names and pointers.
     */
    const std::unordered_map<std::string, Texture*>& get_all_textures() { return m_textures; }

    /**
     * @brief Applies uniform values specific to this material.
     *
     * @param shader Pointer to the ShaderProgram that will receive the uniforms.
     *
     * The base class implementation does nothing. Derived classes should override
     * this function to set shader uniforms such as colors, roughness, shininess,
     * metallic, emission, or other material-specific parameters.
     */
    virtual void apply_uniforms(ShaderProgram* shader) const = 0;

private:

    // Identifier name of the material
    std::string     m_name;

    // Material type (PBR, Phong...)
    MaterialType    m_type;

    // Mapping of shader uniform names to bound textures
    std::unordered_map<std::string, Texture*> m_textures;
};