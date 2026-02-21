#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "types.h"

#include <string>

/**
 * @brief Represents a light source in the scene.
 *
 * This class encapsulates the properties of a light source, including its type
 * (point, directional...), position, color, and intensity. It also stores
 * shading-related parameters and allows enabling or disabling shadows.
 *
 * The Light class is intended to be used by the renderer to gather all lighting
 * information required for shading calculations.
 */
class Light 
{
public:
	
	/**
	 * @brief Constructs a default Light object.
	 * 
	 * Default light is a white point light at world-origin.
	 */
	Light(const std::string& name);

	/**
	 * @brief Constructs a new Light object with given attributes
	 *
	 * @param position  The world-space position of the light.
	 * @param color     The RGB color of the light.
	 * @param type      The type of light.
	 */
	Light(const std::string& name, const glm::vec3& position, const glm::vec3& color, LightType type = LightType::Point);

	/**
	 * @brief Sets the type of the light.
	 * 
	 * @param type New light type.
	 */
	void set_type(LightType type) { m_type = type; }

	/**
	 * @brief Sets the light’s position.
	 * 
	 * @param position New world-space position.
	 */
	void set_position(const glm::vec3& position) { m_position = position; }

	/**
	 * @brief Sets the light color.
	 * 
	 * @param color New RGB color.
	 */
	void set_color(const glm::vec3& color) { m_color = color; }

	/**
	 * @brief Sets the light intensity multiplier.
	 * 
	 * @param intensity New intensity value.
	 */
	void set_intensity(float intensity) { m_intensity = intensity; };

	/**
	 * @brief Enables or disables shadow casting for this light.
	 * 
	 * @param value True to enable shadows.
	 */
	void set_shadows_enabled(bool value) { m_shadows_enabled = value; };

	/**
	 * @brief Returns the type of the light.
	 *
	 * @return LightType enum value.
	 */
	std::string get_name() const { return m_name; }

	/**
	 * @brief Returns the type of the light.
	 * 
	 * @return LightType enum value.
	 */
	LightType get_type() const { return m_type; }

	/**
	 * @brief Returns the light position.
	 * 
	 * @return World-space position vector.
	 */
	glm::vec3 get_position() const { return m_position; }

	/**
	 * @brief Returns the light color.
	 * 
	 * @return RGB color vector.
	 */
	glm::vec3 get_color() const { return m_color; }

	/**
	 * @brief Returns the intensity multiplier of the light.
	 * 
	 * @return Intensity value.
	 */
	float get_intensity() const { return m_intensity; }

	/**
	 * @brief Returns the ambient component of the light
	 *
	 * @return Vec3 that represents the ambient component of the light
	 */
	glm::vec3 get_ambient() const { return m_ambient; }


	/**
	 * @brief Returns the diffuse component of the light
	 *
	 * @return Vec3 that represents the diffuse component of the light
	 */
	glm::vec3 get_diffuse() const { return m_diffuse; }


	/**
	 * @brief Returns the specular component of the light
	 *
	 * @return Vec3 that represents the specular component of the light
	 */
	glm::vec3 get_specular() const { return m_specular; }

	/**
	 * @brief Returns whether shadows are enabled for this light.
	 * 
	 * @return True if shadows are enabled.
	 */
	bool get_shadows_enabled() const { return m_shadows_enabled; }

private:

	// Name of light
	std::string m_name;

	// Type of light
	LightType m_type;

	// World space position of the light
	glm::vec3 m_position;

	// Base color of the light
	glm::vec3 m_color;

	// Intensity multiplier applied to the light
	float m_intensity;

	// Lighting components to use in specific lighting models
	glm::vec3 m_ambient;
	glm::vec3 m_diffuse;
	glm::vec3 m_specular;

	// Wheter this light cast shadows
	bool m_shadows_enabled;
};
