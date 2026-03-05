#pragma once

#include "Light.h"

class DirectionalLight : public Light
{
public:

    DirectionalLight(const std::string& name, const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f), const glm::vec3& color = glm::vec3(1.0f), float intensity = 1.0f);

    void set_direction(const glm::vec3& dir) { m_direction = glm::normalize(dir); }

    glm::vec3 get_direction() const { return m_direction; }

private:

    glm::vec3 m_direction{ 0.0f, -1.0f, 0.0f };
};