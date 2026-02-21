#include "Light.h"

Light::Light(const std::string& name) :
	m_name(name),
	m_type(LightType::Point),
	m_position(glm::vec3(0.0f,0.0f,0.0f)),
	m_color(glm::vec3(1.0f, 1.0f, 1.0f)),
	m_intensity(1.0f),
	m_ambient(glm::vec3(0.2f, 0.2f, 0.2f)),
	m_diffuse(glm::vec3(0.5f, 0.5f, 0.5f)),
	m_specular(glm::vec3(0.5f, 0.5f, 0.5f)),
	m_shadows_enabled(false)
{
}

Light::Light(const std::string& name, const glm::vec3& position, const glm::vec3& color, LightType type) :
	m_name(name),
	m_type(type),
	m_position(position),
	m_color(color),
	m_intensity(1.0f),
	m_ambient(glm::vec3(0.2f, 0.2f, 0.2f)),
	m_diffuse(glm::vec3(0.5f, 0.5f, 0.5f)),
	m_specular(glm::vec3(0.5f, 0.5f, 0.5f)),
	m_shadows_enabled(false)
{
}
