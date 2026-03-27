#pragma once

#include "Material.h"
#include "config.h"

/**
 * @class PBRMaterial
 *
 * @brief Material implementing the Physically Based Rendering (PBR) shading model.
 *
 * PBRMaterial provides shading based on the metallic/roughness workflow,
 * as defined by the glTF 2.0 specification. It uses a Cook-Torrance BRDF
 * with GGX normal distribution, Smith geometry function, and Schlick Fresnel
 * approximation.
 *
 * Supports albedo, metallic/roughness, and normal map textures. Scalar factors
 * are used as fallback values when no texture is assigned, or as multiplicative
 * modulators when a texture is present.
 *
 */
class PBRMaterial : public Material
{
public:

    /**
     * @brief Constructs a new PBRMaterial with the given name
     *
     * @param name Identifier name used to reference this material
     */
    PBRMaterial(const std::string& name);

    /**
     * @brief Sets the albedo color factor of the material
     *
     * Acts as a multiplier over the albedo texture if one is assigned,
     * or as the base surface color if no texture is present.
     *
     * @param color RGB albedo color value
     */
    void set_albedo(const glm::vec3& color) { m_albedo = color; }

    /**
     * @brief Sets the metallic factor of the material.
     *
     * Controls how metallic the surface appears. A value of 0.0 represents
     * a fully dielectric surface, while 1.0 represents a fully metallic surface.
     * Acts as a multiplier over the metallic channel of the metallic/roughness texture.
     *
     * @param value Metallic factor in the range [0.0, 1.0]
     */
    void set_metallic(float value) { m_metallic = value; }

    /**
     * @brief Sets the roughness factor of the material
     *
     * Controls the microsurface roughness. A value of 0.0 produces a perfectly
     * smooth, mirror-like surface, while 1.0 produces a fully rough, diffuse surface.
     * Acts as a multiplier over the roughness channel of the metallic/roughness texture.
     *
     * @param value Roughness factor in the range [0.0, 1.0]
     */
    void set_roughness(float value) { m_roughness = value; }

    /**
     * @brief Returns the albedo color factor of the material
     *
     * @return Const reference to the albedo color vector
     */
    const glm::vec3& get_albedo() const { return m_albedo; }

    /**
     * @brief Returns the metallic factor of the material
     *
     * @return Metallic factor in the range [0.0, 1.0]
     */
    float get_metallic() const { return m_metallic; }

    /**
     * @brief Returns the roughness factor of the material
     *
     * @return Roughness factor in the range [0.0, 1.0]
     */
    float get_roughness() const { return m_roughness; }

protected:

    /**
     * @brief Applies PBR-specific uniform values to the shader
     *
     * Uploads albedo color, metallic factor, roughness factor to the bound shader. 
     * The shader program is assumed to be already bound before this call.
     *
     * @param shader Pointer to the ShaderProgram that will receive the uniforms
     */
    void apply_uniforms(ShaderProgram* shader) const override;

    /**
     * @brief Binds the albedo, metallic/roughness and normal textures to their texture units
     *
     * Falls back to a 1x1 white texture for any slot that has no texture assigned,
     * ensuring the shader always receives valid samplers.
     *
     * @param shader Pointer to the ShaderProgram that will receive the texture uniforms
     */
    void bind_textures(ShaderProgram* shader) const override;

private:

    // Albedo color factor, used as base color or texture modulator
    glm::vec3 m_albedo { 1.0f, 1.0f, 1.0f };

    // Metallic factor controlling the metallic/dielectric response of the surface
    float m_metallic { 0.0f };

    // Roughness factor controlling the microsurface roughness of the surface
    float m_roughness { 0.5f };
};