#include "SkyboxPass.h"

const glm::mat4 SkyboxPass::s_capture_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

const glm::mat4 SkyboxPass::s_capture_views[6] = 
{
    glm::lookAt(glm::vec3(0), glm::vec3(1, 0, 0), glm::vec3(0,-1, 0)),
    glm::lookAt(glm::vec3(0), glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)),
    glm::lookAt(glm::vec3(0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
    glm::lookAt(glm::vec3(0), glm::vec3(0,-1, 0), glm::vec3(0, 0,-1)),
    glm::lookAt(glm::vec3(0), glm::vec3(0, 0, 1), glm::vec3(0,-1, 0)),
    glm::lookAt(glm::vec3(0), glm::vec3(0, 0,-1), glm::vec3(0,-1, 0)),
};

SkyboxPass::SkyboxPass()
{
    // Get shader pointers
    m_shader = ShaderManager::instance().get_program("skybox");
    m_equirect_to_cubemap_shader = ShaderManager::instance().get_program("equirect_to_cubemap");

    // Create skybox cube
    m_cube = std::make_unique<Cube>("skybox_cube");
}

void SkyboxPass::load_from_hdr(const std::string& hdr_path)
{
    // Load HDR texture trough TextureManager
    Texture* hdr = TextureManager::instance().load_hdr_texture("skybox_hdr", hdr_path, true);

    if (!hdr)
    {
        SPDLOG_ERROR("Failed to load HDR: {}", hdr_path);
        return;
    }

    // Conversion to cubemap that is stored in m_skybox_fbo
    convert_hdr_to_cubemap(hdr);

    // Remove the loaded HDR texture as we don't need it anymore
    TextureManager::instance().remove_texture("skybox_hdr");
    SPDLOG_INFO("Skybox loaded from {}", hdr_path);
}

void SkyboxPass::execute(Scene& scene)
{
    // Only if we have a skybox loaded
    if (!m_skybox_fbo) return;

    // Get Main FBO
    Framebuffer* fbo = FramebufferManager::instance().get_framebuffer("main");

    // Check valid FBO and shader
    if (!fbo)
    {
        SPDLOG_ERROR("Invalid FBO for Geometry Pass");
        return;
    }

    if (!m_shader)
    {
        SPDLOG_WARN("No valid skybox shader loaded");
        return;
    }

    // Bind FBO and shader
    fbo->bind();
    m_shader->bind();

    // Configure state. Depth func to LEQUAL to pass depth test where depth buffer is 1.0 (background)
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glCullFace(GL_FRONT);

    // Configure matrices
    Camera* cam = scene.get_active_camera();
    float aspect = glm::clamp((float)fbo->get_width() / (float)fbo->get_height(), MIN_ASPECT, MAX_ASPECT);

    // Delete translate from view matrix casting to mat3. Skybox always centered in camera
    glm::mat4 view_mat = glm::mat4(glm::mat3(cam->get_view_matrix()));
    glm::mat4 projection_mat = glm::perspective(glm::radians(cam->get_fov()), aspect, 0.1f, 100.0f);

    m_shader->set_uniform("view", view_mat);
    m_shader->set_uniform("projection", projection_mat);

    // Bind skybox cubemap
    m_shader->set_uniform("environment_map", 0);
    m_skybox_fbo->bind_color_as_texture(0, 0);
    
    // Skybox draw call
    m_cube->draw_geometry();

    // Restore state
    glCullFace(GL_BACK);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    // Unbind shader and fbo
    m_shader->unbind();
    fbo->unbind();
}

void SkyboxPass::convert_hdr_to_cubemap(const Texture* hdr_texture)
{
    // Create skybox FBO to store the environment cubemap
    FramebufferSpec spec;
    spec.width = 512;
    spec.height = 512;
    spec.resizable = false;
    spec.is_cubemap = true;
    spec.attachments = {
        { GL_RGB16F, GL_COLOR_ATTACHMENT0 }
    };
    m_skybox_fbo = FramebufferManager::instance().create_framebuffer("skybox", spec);

    // Check FBO and shader
    if (!m_skybox_fbo || !m_equirect_to_cubemap_shader)
    {
        SPDLOG_WARN("No valid setup to convert HDR to cubemap skybox");
        return;
    }

    // Bind FBO and shader
    m_skybox_fbo->bind();
    m_equirect_to_cubemap_shader->bind();

    //Configure uniforms
    m_equirect_to_cubemap_shader->set_uniform("projection", s_capture_projection);
    m_equirect_to_cubemap_shader->set_uniform("equirect_map", 0);
    hdr_texture->bind(0);
    
    for (int i = 0; i < 6; i++)
    {
        m_equirect_to_cubemap_shader->set_uniform("capture_views[" + std::to_string(i) + "]", s_capture_views[i]);
    }

    // Configure state
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the cubemap
    m_cube->draw_geometry();

    // Restore state
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Unbind shader and FBO
    m_equirect_to_cubemap_shader->unbind();
    m_skybox_fbo->unbind();
}

void SkyboxPass::upload_lights(Scene& scene, ShaderProgram* shader)
{
    // No lights data needed in this pass, only matrices
}

