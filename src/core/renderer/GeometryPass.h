#pragma once

#include "RenderPass.h"
#include "ShadowPass.h"
#include "FramebufferManager.h"
#include "config.h"

/**
 * @class GeometryPass
 *
 * @brief Render pass responsible for shading and rendering all scene geometry
 *
 * The GeometryPass is the main forward rendering pass of the pipeline. For each
 * renderable in the scene it binds the mesh's assigned material and shader,
 * uploads transformation matrices and scene lights, samples the shadow maps
 * produced by the ShadowPass, and issues the final draw call.
 */
class GeometryPass : public RenderPass
{
public:

    /**
     * @brief Constructs a GeometryPass with default state
     */
    GeometryPass() = default;

    /**
     * @brief Executes the geometry pass for the current frame
     *
     * Iterates all renderables in the scene. For each mesh, binds its material
     * shader, uploads the model, view, and projection matrices, calls
     * upload_lights() to set lighting uniforms, applies material uniforms,
     * and issues the draw call.
     *
     * @param scene The scene containing all renderables and lights to render
     */
    void execute(Scene& scene) override;

    /**
     * @brief Uploads all scene lights as uniforms to the given shader
     *
     * Iterates all lights in the scene and uploads their properties to the shader. 
     * For shadow-enabled lights, also binds their shadow map depth texture and 
     * uploads the light space matrix.
     *
     * @param scene  The scene containing the lights to upload
     * @param shader The shader program to receive the light uniforms
     */
    void upload_lights(Scene& scene, ShaderProgram* shader) override;
};
