#include "PointLight.h"

PointLight::PointLight(const std::string& name, const glm::vec3& position, const glm::vec3& color, float intensity) : 
    Light(name, color, intensity),
    m_position(position) 
{
}
