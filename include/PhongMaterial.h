#pragma once

#include "Material.h"

/**
 * @class PhongMaterial
 * 
 * @brief Material implementing the classic Phong lighting model.
 *
 * This class should be used whenever a traditional Phong shading
 * pipeline is required.
 */
class PhongMaterial : public Material
{
public:

	/**
	 * @brief Constructs a new Phong material.
	 *
	 * @param name   Identifier name of the material.
	 * @param shader Pointer to the ShaderProgram that implements a phong shader.
	 */
	PhongMaterial(const std::string& name, ShaderProgram* shader);

    /**
     * @brief Sets the shininess exponent controlling specular highlight size.
     * @param value Higher values produce smaller, tighter specular highlights.
     */
    void set_shininess(float value) { m_shininess = value; }

    /**
     * @brief Gets the shininess of the material.
     * @return Shininess exponent used in the specular computation.
     */
    float get_shininess() const { return m_shininess; }

protected:

    /**
     * @brief Applies the Phong-specific uniform values to the shader.
     *
     * The method assumes the shader program is already bound.
     */
    void apply_uniforms() const override;

private:

    // Specular exponent
    float     m_shininess{ 32.0f };
};
