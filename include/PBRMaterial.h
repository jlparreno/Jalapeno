#pragma once

#include "Material.h"

class PBRMaterial : public Material
{
public:

    /**
     * @brief Constructs a new PBR material.
     *
     * @param name Identifier name of the material.
     */
    PBRMaterial(const std::string& name);

    void set_albedo(const glm::vec3& color) { m_albedo = color; }
    void set_metallic(float value) { m_metallic = value; }
    void set_roughness(float value) { m_roughness = value; }
    void  set_alpha_cutout(bool value) { m_alpha_cutout = value; }
    void  set_alpha_threshold(float value) { m_alpha_threshold = value; }
    
    const glm::vec3& get_albedo()    const { return m_albedo; }
    float get_metallic()  const { return m_metallic; }
    float get_roughness() const { return m_roughness; }
    bool  get_alpha_cutout()    const { return m_alpha_cutout; }
    float get_alpha_threshold() const { return m_alpha_threshold; }

protected:

    /**
     * @brief Applies the specific uniform values to the shader.
     *
     * The method assumes the shader program is already bound.
     */
    void apply_uniforms(ShaderProgram* shader) const override;

    void bind_textures(ShaderProgram* shader) const override;

private:

    glm::vec3 m_albedo{ 1.0f, 1.0f, 1.0f };
    float     m_metallic{ 0.0f };
    float     m_roughness{ 0.5f };

    bool  m_alpha_cutout{ false };
    float m_alpha_threshold{ 0.5f };
};