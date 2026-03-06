#pragma once

#include "Light.h"

/**
 * @class DirectionalLight
 *
 * @brief Represents a directional light source that illuminates the scene uniformly.
 *
 * A directional light simulates a light source that is infinitely far away,
 * such as the sun. All rays are parallel and travel in the same direction,
 * producing uniform illumination across the entire scene regardless of position.
 *
 * Unlike PointLight, directional lights have no position and no attenuation.
 */
class DirectionalLight : public Light
{
public:

    /**
     * @brief Constructs a DirectionalLight with the given properties
     *
     * @param name      Identifier name used to reference this light
     * @param direction World-space direction the light rays travel towards
     * @param color     RGB color of the light
     * @param intensity Intensity multiplier applied to the light output
     */
    DirectionalLight(const std::string& name, 
                     const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f), 
                     const glm::vec3& color = glm::vec3(1.0f), 
                     float intensity = 1.0f);

    /**
     * @brief Sets the direction of the light
     *
     * The provided vector is normalized automatically
     *
     * @param dir New direction vector
     */
    void set_direction(const glm::vec3& dir) { m_direction = glm::normalize(dir); }

    /**
     * @brief Returns the normalized direction of the light
     *
     * @return Unit vector representing the direction of the light
     */
    glm::vec3 get_direction() const { return m_direction; }

private:

    // Normalized world-space direction the light rays travel towards.
    // Defaults to straight down
    glm::vec3 m_direction { 0.0f, -1.0f, 0.0f };
};