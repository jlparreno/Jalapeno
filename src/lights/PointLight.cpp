#include "PointLight.h"

PointLight::PointLight(const std::string& name, const glm::vec3& position, const glm::vec3& color, float intensity) : 
    Light(name, color, intensity),
    m_position(position),
    m_constant(1.0f),
    m_linear(0.09f),
    m_quadratic(0.032f)
{
}

void PointLight::init_shadow_map(int size)
{
    // Depth-only framebuffer, no resizable as it is a shadow map
    FramebufferSpec spec;
    spec.width = size;
    spec.height = size;
    spec.resizable = false;
    spec.is_cubemap = true;
    spec.attachments = {
        { GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT }
    };

    // Create the FBO through the manager, the owner
    std::string fbo_name = m_name + "_shadows";
    Framebuffer* fbo = FramebufferManager::instance().create_framebuffer(fbo_name, spec);

    // Save only the pointer
    m_shadow_fbo = fbo;
}

void PointLight::compute_light_space_matrix()
{
    // We always set a perpective projection of 90º for the cubemap
    // Aspect 1.0 as the cubemap is squared
    glm::mat4 light_proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, m_far_plane);

    m_shadow_matrices = 
    {
        light_proj * glm::lookAt(m_position, m_position + glm::vec3(1, 0, 0), glm::vec3(0,-1, 0)),
        light_proj * glm::lookAt(m_position, m_position + glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)),
        light_proj * glm::lookAt(m_position, m_position + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
        light_proj * glm::lookAt(m_position, m_position + glm::vec3(0,-1, 0), glm::vec3(0, 0,-1)),
        light_proj * glm::lookAt(m_position, m_position + glm::vec3(0, 0, 1), glm::vec3(0,-1, 0)),
        light_proj * glm::lookAt(m_position, m_position + glm::vec3(0, 0,-1), glm::vec3(0,-1, 0)),
    };

    // We set the first one for base class compatibility
    m_light_space_matrix = m_shadow_matrices[0];
}
