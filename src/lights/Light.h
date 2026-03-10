#pragma once

#include "TextureManager.h"
#include "FramebufferManager.h"
#include "Camera.h"
#include "config.h"

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
     * If the shadow map has not been initialized for this light, this
     * method calls to init_shadow_map to correctly set up this Framebuffer
     *
     * @param value True to enable shadow casting, false to disable
     */
    void set_shadows_enabled(bool value);

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

    /**
     * @brief Returns the shadow map framebuffer associated with this light
     *
     * @return Raw pointer to the shadow map Framebuffer, or nullptr if not initialized
     */
    Framebuffer* get_shadow_fbo() const { return m_shadow_fbo; }

    /**
     * @brief Returns the light space transformation matrix
     *
     * This matrix transforms world space positions into the light's clip space.
     *
     * @return Const reference to the light space matrix
     */
    const glm::mat4& get_light_space_matrix() const { return m_light_space_matrix; }

    /**
     * @brief Initializes the shadow map framebuffer for this light
     *
     * Must be called before shadows can be rendered for this light. 
     * Each subclass determines the appropriate framebuffer configuration for its light type.
     *
     * @param size Resolution of the shadow map in pixels (width and height)
     */
    virtual void init_shadow_map(int size) = 0;

    /**
     * @brief Recomputes the light space matrix for the current frame
     *
     * Each light type implements its own projection strategy.
     */
    virtual void compute_light_space_matrix() = 0;

protected:

    // Identifier name used to reference this light
    std::string     m_name;

    // RGB color of the light
    glm::vec3       m_color { 1.0f, 1.0f, 1.0f };

    // Intensity multiplier applied to the light color output
    float           m_intensity { 1.0f };

    // Whether this light casts shadows
    bool            m_shadows_enabled { false };

    // View-projection matrix to render shadow map
    glm::mat4       m_light_space_matrix{ glm::mat4(1.0f) };

    // Shadow map FBO
    Framebuffer*    m_shadow_fbo{ nullptr };
};