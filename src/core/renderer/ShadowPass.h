#pragma once

#include "RenderPass.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "PipelineState.h"

/**
 * @class ShadowPass
 *
 * @brief Render pass responsible for generating shadow maps for all shadow-enabled lights
 *
 * Each frame, the ShadowPass iterates all lights in the scene that have shadows
 * enabled, recomputes their light space matrices, and renders the scene geometry 
 * into each light's depth framebuffer. The resulting shadow maps are later sampled 
 * by the GeometryPass to determine shadowing at each fragment.
 */
class ShadowPass : public RenderPass
{
public:

    /**
     * @brief Constructs a ShadowPass and loads the depth shader
     */
    ShadowPass();

    /**
     * @brief Executes the shadow pass for the current frame
     *
     * For each shadow-enabled light in the scene, recomputes the light space
     * matrix, binds the light's depth framebuffer, and renders all scene renderables 
     * using the depth shader.
     *
     * @param scene The scene containing the lights and renderables to process
     */
    void execute(Scene& scene) override;

    /**
     * @brief Uploads light space matrices to the given shader
     *
     * No lights data needed for now in this pass. Empty function.
     *
     * @param scene  The scene containing the lights to upload
     * @param shader The shader program to receive the light uniforms
     */
    void upload_lights(Scene& scene, ShaderProgram* shader) override;

private:

    // OpenGL context state for this pass
    static const PipelineState k_state;

    // Depth-only shader used to render scene geometry into shadow map framebuffers for directional lights
    ShaderProgram* m_directional_shadows_shader;

    // Depth-only shader used to render scene geometry into shadow map framebuffers for point lights
    ShaderProgram* m_point_shadows_shader;
};
