#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const std::string& name, const glm::vec3& direction, const glm::vec3& color, float intensity) :
    Light(name, color, intensity),
    m_direction(glm::normalize(direction)) 
{
}
