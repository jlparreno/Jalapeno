#pragma once

#include "Material.h"

class PhongMaterial : public Material
{
public:

	/**
	 * @brief Constructs a new Phong material.
	 *
	 * @param name Identifier name of the material.
	 */
	PhongMaterial(const std::string& name);

    void set_ambient_color(const glm::vec3& color) { m_ambient_color = color; }
    void set_diffuse_color(const glm::vec3& color) { m_diffuse_color = color; }
    void set_specular_color(const glm::vec3& color) { m_specular_color = color; }
    void set_shininess(float value) { m_shininess = value; }

    const glm::vec3& get_ambient_color()  const { return m_ambient_color; }
    const glm::vec3& get_diffuse_color()  const { return m_diffuse_color; }
    const glm::vec3& get_specular_color() const { return m_specular_color; }
    float            get_shininess()      const { return m_shininess; }

protected:

    /**
     * @brief Applies the specific uniform values to the shader.
     *
     * The method assumes the shader program is already bound.
     */
    void apply_uniforms(ShaderProgram* shader) const override;

private:

    glm::vec3 m_ambient_color { 0.1f, 0.1f, 0.1f };
    glm::vec3 m_diffuse_color { 1.0f, 1.0f, 1.0f };
    glm::vec3 m_specular_color{ 1.0f, 1.0f, 1.0f };

    float     m_shininess{ 32.0f };
};
