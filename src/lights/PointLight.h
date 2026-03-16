#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Light.h"


/**
 * @class PointLight
 *
 * @brief Represents a point light source that emits light in all directions from a position.
 *
 * A point light emits light equally in all directions from a fixed world-space position, 
 * and its intensity attenuates with distance using a configurable quadratic attenuation model.
 */
class PointLight : public Light
{
public:

    /**
     * @brief Constructs a PointLight with the given properties
     *
     * @param name      Identifier name used to reference this light
     * @param position  World-space position of the light
     * @param color     RGB color of the emitted light
     * @param intensity Intensity multiplier applied to the light output
     */
    PointLight(const std::string& name,
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& color = glm::vec3(1.0f),
        float intensity = 1.0f);

    /**
     * @brief Sets the world-space position of the light
     *
     * @param pos New position vector
     */
    void set_position(const glm::vec3& pos) { m_position = pos; }

    /**
     * @brief Sets the attenuation coefficients for distance-based light falloff
     *
     * Typical values: constant=1.0, linear=0.09, quadratic=0.032
     *
     * @param constant  Constant attenuation term. Should be >= 1.0
     * @param linear    Linear attenuation term controlling gradual falloff
     * @param quadratic Quadratic attenuation term controlling rapid falloff at distance
     */
    void set_attenuation(float constant, float linear, float quadratic)
    {
        m_constant = constant;
        m_linear = linear;
        m_quadratic = quadratic;
    }

    /**
     * @brief Returns the world-space position of the light
     *
     * @return Position vector
     */
    glm::vec3 get_position()  const { return m_position; }

    /**
     * @brief Returns the constant attenuation coefficient
     *
     * @return Constant term of the attenuation equation
     */
    float get_constant()  const { return m_constant; }

    /**
     * @brief Returns the linear attenuation coefficient
     *
     * @return Linear term of the attenuation equation
     */
    float get_linear()    const { return m_linear; }

    /**
     * @brief Returns the quadratic attenuation coefficient
     *
     * @return Quadratic term of the attenuation equation
     */
    float get_quadratic() const { return m_quadratic; }

    /**
     * @brief Returns the far plane distance used for point light shadow projection
     *
     * @return Far plane distance in world units
     */
    float get_far_plane() const { return m_far_plane; }

    /**
     * @brief Returns the six light space matrices used to render the shadow cubemap
     *
     * Each matrix corresponds to one face of the cubemap, combining a 90-degree
     * perspective projection with a view matrix looking in the direction of that
     * face. Updated every frame by compute_light_space_matrix().
     *
     * @return Const reference to the array of six light space matrices
     */
    const std::array<glm::mat4, 6>& get_shadow_matrices() const { return m_shadow_matrices; }

    /**
     * @brief Initializes the shadow map framebuffer for this light
     *
     * @param size Resolution of the shadow map in pixels (width and height)
     */
    void init_shadow_map(int size = 2048) override;

    /**
     * @brief Recomputes the light space matrix for the current frame
     */
    void compute_light_space_matrix() override;

private:

    // World-space position of the light source
    glm::vec3 m_position { 0.0f, 0.0f, 0.0f };

    // Constant term of the attenuation equation
    float m_constant { 1.0f };

    // Linear term of the attenuation equation
    float m_linear { 0.09f };

    // Quadratic term of the attenuation equation
    float m_quadratic { 0.032f };

    // Far plane distance used for shadow projection
    float m_far_plane{ 25.0f };

    // Matrices for shadow mapping
    std::array<glm::mat4, 6> m_shadow_matrices;
};