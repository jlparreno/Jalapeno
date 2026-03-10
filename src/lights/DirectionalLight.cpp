#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const std::string& name, const glm::vec3& direction, const glm::vec3& color, float intensity) :
    Light(name, color, intensity),
    m_direction(glm::normalize(direction)) 
{
}

void DirectionalLight::init_shadow_map(int size)
{
    // Depth-only framebuffer, no resizable as it is a shadow map
    FramebufferSpec spec;
    spec.width = size;
    spec.height = size;
    spec.resizable = false;
    spec.attachments = {
        { GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT }
    };

    // Create the FBO through the manager, the owner
    std::string fbo_name = m_name + "_shadows";
    Framebuffer* fbo = FramebufferManager::instance().create_framebuffer(fbo_name, spec);

    // Save only the pointer
    m_shadow_fbo = fbo;
}

void DirectionalLight::compute_light_space_matrix()
{
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    // If light is parallel to UP, UP vector should be parallel
    if (glm::abs(glm::dot(m_direction, glm::vec3(0.0f, 1.0f, 0.0f))) > 0.99f)
    {
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }

    // Position: go back following the negative direction
    // Looking at the scene center
    glm::mat4 light_view = glm::lookAt(-m_direction * 20.0f, glm::vec3(0.0f), up);

    // Orthogonal projection as this is a directional light
    glm::mat4 light_proj = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.1f, 30.0f);

    m_light_space_matrix = light_proj * light_view;
}
