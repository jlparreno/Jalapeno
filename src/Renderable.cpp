#include "Renderable.h"

glm::mat4 Renderable::get_model_matrix() const
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, m_position);
    
    model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));

    model = glm::scale(model, m_scale);

    return model;
}