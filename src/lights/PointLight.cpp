#include "PointLight.h"

PointLight::PointLight(const std::string& name, const glm::vec3& position, const glm::vec3& color, float intensity) : 
    Light(name, color, intensity),
    m_position(position),
    m_constant(1.0f),
    m_linear(0.09f),
    m_quadratic(0.032f)
{
}

void PointLight::init_shadow_map(int size)
{
}

void PointLight::compute_light_space_matrix()
{
    // We always set a perpective projection of 90º for the cubemap
    glm::mat4 light_proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);

    // Looking at the origin
    glm::mat4 light_view = glm::lookAt(m_position, glm::vec3(0.0f), glm::vec3(0, 1, 0));

    m_light_space_matrix = light_proj * light_view;
}
