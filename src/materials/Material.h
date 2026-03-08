#pragma once

#include <glm/glm.hpp>

#include "ShaderManager.h"
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
 *
 * Material is not intended to be instantiated directly. Use derived classes
 * such as PhongMaterial, LambertMaterial or PBRMaterial instead.
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
     * @brief Loads and registers a new texture for this material
     *
     * @param name          Name used to identify and retrieve the texture internally
     * @param path          File path to the texture on disk
     * @param type_name     Semantic type of the texture (e.g. "diffuse", "normal", "specular")
     * @param vertical_flip Whether the texture should be flipped vertically on load
     *
     * This method loads the texture through the TextureManager and stores it
     * internally for later binding during rendering.
     */
    void add_texture(const std::string& name, const std::string& path, const std::string& type_name, bool vertical_flip = false);

    /**
     * @brief Returns the material's identifier name
     *
     * @return Const reference to the string used to identify this material
     */
    const std::string& get_name() const { return m_name; }

    /**
     * @brief Returns the type of the material
     *
     * @return The MaterialType of this material
     */
    MaterialType    get_type() const { return m_type; }

    /**
    * @brief Returns the shader program associated with this material
    *
    * Each material type owns a specific shader. The Renderer uses this
    * to bind the correct shader before issuing draw calls
    *
    * @return Pointer to the ShaderProgram used by this material
    */
    ShaderProgram*  get_shader() const { return m_shader; }

    /**
     * @brief Returns all textures stored in this material
     *
     * @return Const reference to the map of texture names to Texture pointers
     */
    const std::unordered_map<std::string, Texture*>& get_all_textures() const { return m_textures; }

    /**
     * @brief Applies material-specific uniform values to the given shader
     *
     * @param shader Pointer to the ShaderProgram that will receive the uniforms
     *
     * Derived classes must override this method to upload their specific.
     * parameters (colors, roughness, shininess, metallic, etc.) to the shader.
     * The shader is assumed to be already bound before this call.
     */
    virtual void apply_uniforms(ShaderProgram* shader) const = 0;

    /**
     * @brief Binds all textures associated with the material to the GPU
     *
     * @param shader Pointer to the ShaderProgram that will receive the texture uniforms
     *
     * Assigns texture units sequentially and binds each stored texture to its
     * corresponding sampler uniform in the shader. Should be called before any
     * draw call that uses this material. Derived classes may override this method
     * to define a custom binding order or use additional fallback textures.
     */
    virtual void bind_textures(ShaderProgram* shader) const;

    /**
     * @brief Binds a texture to the specified unit, or a white texture if not found
     *
     * @param name         Name used to look up the texture in the material's texture map
     * @param unit         Texture unit index to bind to (e.g. 0 for GL_TEXTURE0)
     * @param shader       Pointer to the ShaderProgram that will receive the sampler uniform
     *
     * This helper centralizes the common pattern of binding a named texture or falling back
     * to a 1x1 white texture if the material does not have a texture assigned for that slot.
     * It also sets the corresponding sampler uniform on the shader.
     * Should be called from derived class bind_textures() implementations.
     */
    void bind_texture_or_white(const std::string& name, int unit, ShaderProgram* shader) const;

protected:

    // Mapping of shader uniform names to Texture instances
    std::unordered_map<std::string, Texture*> m_textures;

    // Shader program used to render this material.
    // Set by derived class constructors via the ShaderManager.
    ShaderProgram* m_shader;

private:

    // Identifier name of the material in the MaterialManager
    std::string     m_name;

    // Material type (Lambert, Phong, PBR...)
    MaterialType    m_type;
};