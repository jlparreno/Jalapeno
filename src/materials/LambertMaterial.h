#pragma once

#include "Material.h"

/**
 * @class LambertMaterial
 *
 * @brief Material implementing the Lambertian diffuse reflectance model.
 *
 * LambertMaterial provides a simple, matte shading model based on the
 * Lambertian reflectance equation. It considers only ambient and diffuse
 * lighting components, with no specular highlights.
 */
class LambertMaterial : public Material
{
public:

    /**
     * @brief Constructs a new LambertMaterial with the given name
     *
     * @param name Identifier name used to reference this material
     */
    LambertMaterial(const std::string& name);

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
     * @brief Returns the ambient color of the material
     *
     * @return Const reference to the ambient color vector
     */
    const glm::vec3& get_ambient_color() const { return m_ambient_color; }

    /**
     * @brief Returns the diffuse color of the material
     *
     * @return Const reference to the diffuse color vector
     */
    const glm::vec3& get_diffuse_color() const { return m_diffuse_color; }

protected:

    /**
     * @brief Applies Lambert-specific uniform values to the shader
     *
     * Uploads the ambient and diffuse color values to the bound shader.
     * The shader program is assumed to be already bound before this call.
     *
     * @param shader Pointer to the ShaderProgram that will receive the uniforms
     */
    void apply_uniforms(ShaderProgram* shader) const override;

    /**
     * @brief Binds the diffuse texture to its corresponding texture unit
     *
     * Falls back to a 1x1 white texture if no diffuse texture has been assigned,
     * ensuring the shader always receives a valid sampler.
     *
     * @param shader Pointer to the ShaderProgram that will receive the texture uniforms
     */
    void bind_textures(ShaderProgram* shader) const override;

private:

    // Ambient color applied as a constant base illumination term
    glm::vec3 m_ambient_color{ 0.1f, 0.1f, 0.1f };

    // Diffuse color defining the base surface color under direct lighting
    glm::vec3 m_diffuse_color{ 1.0f, 1.0f, 1.0f };
};