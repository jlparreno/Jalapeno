#pragma once

#include <glm/glm.hpp>
#include <string>

class Light
{
public:

    Light(const std::string& name, const glm::vec3& color, float intensity);

    virtual ~Light() = default;

    void set_color(const glm::vec3& color) { m_color = color; }
    void set_intensity(float intensity) { m_intensity = intensity; }
    void set_shadows_enabled(bool value) { m_shadows_enabled = value; }

    const std::string& get_name() const { return m_name; }
    glm::vec3 get_color() const { return m_color; }
    float get_intensity() const { return m_intensity; }
    bool get_shadows_enabled() const { return m_shadows_enabled; }

protected:

    std::string m_name;

    glm::vec3   m_color{ 1.0f, 1.0f, 1.0f };
    float       m_intensity{ 1.0f };
    bool        m_shadows_enabled{ false };
};