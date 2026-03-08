#pragma once

#include "Material.h"

/**
 * @class PhongMaterial
 *
 * @brief Material implementing the classic Phong lighting model.
 *
 * PhongMaterial provides shading based on the Phong reflectance model,
 * which considers ambient, diffuse, and specular lighting components.
 * The specular highlight size is controlled by the shininess exponent.
 */
class PhongMaterial : public Material
{
public:

    /**
     * @brief Constructs a new PhongMaterial with the given name
     *
     * @param name Identifier name used to reference this material
     */
    PhongMaterial(const std::string& name);

    /**
     * @brief Sets the ambient color of the material
     *
     * @param color RGB ambient color value
     */
    void set_ambient_color(const glm::vec3& color) { m_ambient_color = color; }

    /**
     * @brief Sets the diffuse color of the material
     *
     * @param color RGB diffuse color value
     */
    void set_diffuse_color(const glm::vec3& color) { m_diffuse_color = color; }

    /**
     * @brief Sets the specular color of the material.
     *
     * @param color RGB specular color value
     */
    void set_specular_color(const glm::vec3& color) { m_specular_color = color; }

    /**
     * @brief Sets the shininess exponent controlling specular highlight size
     *
     * Higher values produce smaller, tighter specular highlights.
     * Typical values range from 8 (very broad) to 256 (very sharp).
     *
     * @param value Shininess exponent
     */
    void set_shininess(float value) { m_shininess = value; }

    /**
     * @brief Returns the ambient color of the material
     *
     * @return Const reference to the ambient color vector
     */
    const glm::vec3& get_ambient_color()  const { return m_ambient_color; }

    /**
     * @brief Returns the diffuse color of the material
     *
     * @return Const reference to the diffuse color vector
     */
    const glm::vec3& get_diffuse_color()  const { return m_diffuse_color; }

    /**
     * @brief Returns the specular color of the material
     *
     * @return Const reference to the specular color vector
     */
    const glm::vec3& get_specular_color() const { return m_specular_color; }

    /**
     * @brief Returns the shininess exponent of the material
     *
     * @return Shininess exponent used in the specular computation
     */
    float get_shininess() const { return m_shininess; }

protected:

    /**
     * @brief Applies Phong-specific uniform values to the shader
     *
     * Uploads ambient, diffuse, specular color and shininess values
     * to the bound shader. The shader program is assumed to be already
     * bound before this call.
     *
     * @param shader Pointer to the ShaderProgram that will receive the uniforms
     */
    void apply_uniforms(ShaderProgram* shader) const override;

    /**
     * @brief Binds the diffuse and specular textures to their texture units
     *
     * Falls back to a 1x1 white texture for any slot that has no texture assigned,
     * ensuring the shader always receives valid samplers. No normal texture for now.
     *
     * @param shader Pointer to the ShaderProgram that will receive the texture uniforms
     */
    void bind_textures(ShaderProgram* shader) const override;

private:

    // Ambient color applied as a constant base illumination term
    glm::vec3 m_ambient_color{ 0.1f, 0.1f, 0.1f };

    // Diffuse color defining the base surface color under direct lighting
    glm::vec3 m_diffuse_color{ 1.0f, 1.0f, 1.0f };

    // Specular color defining the color of direct light reflections
    glm::vec3 m_specular_color{ 1.0f, 1.0f, 1.0f };

    // Shininess exponent controlling the size of specular highlights
    float m_shininess{ 32.0f };
};