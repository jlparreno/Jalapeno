#pragma once

#include <glm/glm.hpp>
#include <string>

/**
 * @class Light
 *
 * @brief Base class representing a light source in the scene.
 *
 * This class encapsulates the common properties shared by all light types,
 * such as name, color, intensity, and shadow casting. Derived classes are
 * responsible for defining type-specific properties such as position or
 * direction.
 *
 * Light is not intended to be instantiated directly. 
 * Use derived classes instead.
 */
class Light
{
public:

    /**
     * @brief Constructs a Light with the given name, color and intensity
     *
     * @param name      Identifier name used to reference this light
     * @param color     RGB color of the light
     * @param intensity Multiplier applied to the light's color output
     */
    Light(const std::string& name, const glm::vec3& color, float intensity);

    /**
     * @brief Virtual destructor
     */
    virtual ~Light() = default;

    /**
     * @brief Sets the RGB color of the light
     *
     * @param color New color value
     */
    void set_color(const glm::vec3& color) { m_color = color; }

    /**
     * @brief Sets the intensity multiplier of the light
     *
     * Higher values produce brighter output
     *
     * @param intensity New intensity value
     */
    void set_intensity(float intensity) { m_intensity = intensity; }

    /**
     * @brief Enables or disables shadow casting for this light
     *
     * @param value True to enable shadow casting, false to disable
     */
    void set_shadows_enabled(bool value) { m_shadows_enabled = value; }

    /**
     * @brief Returns the identifier name of the light
     *
     * @return Const reference to light's name string
     */
    const std::string& get_name() const { return m_name; }

    /**
     * @brief Returns the RGB color of the light
     *
     * @return Color vector
     */
    glm::vec3 get_color() const { return m_color; }

    /**
     * @brief Returns the intensity multiplier of the light
     *
     * @return Intensity value
     */
    float get_intensity() const { return m_intensity; }

    /**
     * @brief Returns whether shadow casting is enabled for this light
     *
     * @return True if shadows are enabled
     */
    bool get_shadows_enabled() const { return m_shadows_enabled; }

protected:

    // Identifier name used to reference this light
    std::string m_name;

    // RGB color of the light
    glm::vec3   m_color { 1.0f, 1.0f, 1.0f };

    // Intensity multiplier applied to the light color output
    float       m_intensity { 1.0f };

    // Whether this light casts shadows
    bool        m_shadows_enabled { false };
};