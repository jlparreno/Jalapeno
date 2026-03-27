#pragma once

#include "Scene.h"

/**
 * @class RenderPass
 *
 * @brief Abstract base class representing a single pass in the render pipeline
 *
 * A RenderPass encapsulates a rendering operation such as shadow map generation, 
 * geometry rendering, or post-processing. Passes are executed sequentially by the 
 * Renderer each frame, each one reading from and writing to the appropriate framebuffers.
 *
 * Subclasses must implement execute() to define their rendering logic, and
 * upload_lights() to define how scene lights are uploaded to a given shader.
 */
class RenderPass
{
public:
    /**
     * @brief Virtual destructor to ensure correct cleanup of derived passes.
     */
    virtual ~RenderPass() = default;

    /**
     * @brief Executes this render pass for the current frame
     *
     * Implementations should bind the appropriate framebuffer, configure
     * OpenGL state, iterate scene renderables, and issue draw calls.
     *
     * @param scene The scene containing all renderables and lights to process
     */
    virtual void execute(Scene& scene) = 0;

    /**
     * @brief Uploads all scene lights as uniforms to the given shader
     *
     * Each pass may upload lights differently depending on its shading model.
     *
     * @param scene  The scene containing the lights to upload
     * @param shader The shader program to upload the light uniforms to
     */
    virtual void upload_lights(Scene& scene, ShaderProgram* shader) = 0;

    /**
     * @brief Enables or disables this render pass
     *
     * When disabled, the Renderer skips this pass entirely during execution.
     *
     * @param enabled True to enable the pass, false to disable it
     */
    void set_enabled(bool enabled) { m_enabled = enabled; }

    /**
     * @brief Returns whether this render pass is currently enabled
     *
     * @return True if the pass will be executed by the Renderer, false if it is skipped
     */
    bool get_enabled() const { return m_enabled; }

protected:

    // Whether this pass is active and should be executed each frame.
    bool m_enabled{ true };
};
