#pragma once

#include "Light.h"

class PointLight : public Light
{
public:

    PointLight(const std::string& name, const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f);

    void set_position(const glm::vec3& pos) { m_position = pos; }

    void set_attenuation(float constant, float linear, float quadratic)
    {
        m_constant = constant;
        m_linear = linear;
        m_quadratic = quadratic;
    }

    glm::vec3 get_position()  const { return m_position; }
    float     get_constant()  const { return m_constant; }
    float     get_linear()    const { return m_linear; }
    float     get_quadratic() const { return m_quadratic; }

private:

    glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };

    float     m_constant{ 1.0f };
    float     m_linear{ 0.09f };
    float     m_quadratic{ 0.032f };
};