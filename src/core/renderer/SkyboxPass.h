#pragma once

#include "ShaderManager.h"
#include "Renderer.h"
#include "RenderPass.h"
#include "Cube.h"

/**
 * @class SkyboxPass
 *
 * @brief Render pass that draws an HDR environment cubemap as a background skybox
 *
 * Loads an HDR equirectangular image and converts it to a cubemap internally.
 * Renders a unit cube centered at the camera position using GL_LEQUAL depth
 * testing so the skybox only fills fragments not covered by scene geometry.
 * Must be executed after GeometryPass.
 */
class SkyboxPass : public RenderPass
{
public:

    /**
     * @brief Constructs a SkyboxPass and loads the required shaders
     *
     * Initializes the skybox and equirect-to-cubemap shaders and creates
     * the unit cube used for rendering. Does not load any skybox texture,
     * you should call load_from_hdr() to provide an environment map.
     */
    SkyboxPass();

    /**
     * @brief Executes the skybox pass for the current frame
     *
     * @param scene The scene containing the active camera
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

    /**
     * @brief Loads an HDR equirectangular image and converts it to the skybox cubemap
     *
     * Reads the HDR file via the TextureManager, renders it to a 512x512 cubemap
     * using the equirect-to-cubemap shader, and stores the result in m_skybox_fbo.
     *
     * @param hdr_path Path to the HDR equirectangular image file
     */
    void load_from_hdr(const std::string& hdr_path);

private:

    /**
     * @brief Converts an equirectangular HDR texture to a cubemap and stores it in m_skybox_fbo
     *
     * Renders the HDR texture to a 512x512 cubemap using the equirect-to-cubemap
     * geometry shader, which emits geometry to all 6 faces in a single draw call.
     *
     * @param hdr_texture Pointer to the loaded HDR equirectangular texture
     */
    void convert_hdr_to_cubemap(const Texture* hdr_texture);

    // Shader used to render the skybox cubemap each frame
    ShaderProgram* m_shader{ nullptr };

    // Shader used during initialization to convert the HDR equirectangular map to a cubemap
    ShaderProgram* m_equirect_to_cubemap_shader{ nullptr };

    // Framebuffer containing the skybox cubemap color attachment
    Framebuffer* m_skybox_fbo{ nullptr };

    // Unit cube used to render the skybox
    std::unique_ptr<Cube> m_cube{ nullptr };

    // Perspective projection matrix with 90° FOV used during cubemap capture
    static const glm::mat4 s_capture_projection;

    // Six view matrices pointing toward each face of the cubemap during capture
    static const glm::mat4 s_capture_views[6];
};
