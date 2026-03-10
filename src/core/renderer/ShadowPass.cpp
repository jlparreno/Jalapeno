#include "ShadowPass.h"

ShadowPass::ShadowPass()
{
    // Get depth shader
    m_shadowmap_shader = ShaderManager::instance().get_program("depth");

    if (!m_shadowmap_shader)
    {
        SPDLOG_WARN("No depth shader available for Shadow Pass");
    }
}

void ShadowPass::execute(const Scene& scene)
{
    // Bind depth shader (same for all lights)
    m_shadowmap_shader->bind();

    // Enable front face culling to avoid Peter panning
    glCullFace(GL_FRONT);

    for (auto& light : scene.get_scene_lights())
    {
        // Only lights with enabled shadow casting
        if (!light->get_shadows_enabled() || !light->get_shadow_fbo())
            continue;

        // Recalculate matrix each frame as the camera frustum changes
        light->compute_light_space_matrix();

        // Bind specific light FBO and clear it
        light->get_shadow_fbo()->bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        
        // Upload light space matrix
        m_shadowmap_shader->set_uniform("light_matrix", light->get_light_space_matrix());

        // Render all renderables to update depth map
        for (auto& renderable : scene.get_scene_renderables())
        {
            m_shadowmap_shader->set_uniform("model", renderable->get_model_matrix());
            renderable->draw_geometry();
        }
    }

    // Recover previous state
    glCullFace(GL_BACK);
    m_shadowmap_shader->unbind();
}

void ShadowPass::upload_lights(const Scene& scene, ShaderProgram* shader)
{
    // No lights data needed in this pass, only matrices
}