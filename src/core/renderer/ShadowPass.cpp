#include "ShadowPass.h"

ShadowPass::ShadowPass()
{
    // Get depth shaders
    m_directional_shadows_shader = ShaderManager::instance().get_program("directional_shadows");
    m_point_shadows_shader = ShaderManager::instance().get_program("point_shadows");

    if (!m_directional_shadows_shader)
        SPDLOG_WARN("No directional shadows shader available for Shadow Pass");

    if (!m_point_shadows_shader)
        SPDLOG_WARN("No point shadows shader available for Shadow Pass");
}

void ShadowPass::execute(Scene& scene)
{
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
        
        if (DirectionalLight* dl = dynamic_cast<DirectionalLight*>(light.get()))
        {
            m_directional_shadows_shader->bind();
            m_directional_shadows_shader->set_uniform("light_matrix", dl->get_light_space_matrix());

            for (const auto& renderable : scene.get_scene_renderables())
            {
                m_directional_shadows_shader->set_uniform("model", renderable->get_model_matrix());
                renderable->draw_geometry();
            }

            m_directional_shadows_shader->unbind();
        }
        else if (PointLight* pl = dynamic_cast<PointLight*>(light.get()))
        {
            m_point_shadows_shader->bind();

            // Subir las 6 matrices del cubemap
            auto& shadow_matrices = pl->get_shadow_matrices();
            for (int i = 0; i < 6; i++)
            {
                m_point_shadows_shader->set_uniform("shadow_matrices[" + std::to_string(i) + "]", shadow_matrices[i]);
            }

            m_point_shadows_shader->set_uniform("light_pos", pl->get_position());
            m_point_shadows_shader->set_uniform("far_plane", pl->get_far_plane());

            for (const auto& renderable : scene.get_scene_renderables())
            {
                m_point_shadows_shader->set_uniform("model", renderable->get_model_matrix());
                renderable->draw_geometry();
            }

            m_point_shadows_shader->unbind();
        }
    }

    // Recover previous state
    glCullFace(GL_BACK);
}

void ShadowPass::upload_lights(Scene& scene, ShaderProgram* shader)
{
    // No lights data needed in this pass, only matrices
}