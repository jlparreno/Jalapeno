#include "Light.h"

Light::Light(const std::string& name, const glm::vec3& color, float intensity) :
	m_name(name), 
	m_color(color), 
	m_intensity(intensity) 
{
}
