#include "PointLight.h"

PointLight::PointLight(const std::string& name, const glm::vec3& position, const glm::vec3& color, float intensity) : 
    Light(name, color, intensity),
    m_position(position),
    m_constant(1.0f),
    m_linear(0.09f),
    m_quadratic(0.032f)
{
}
