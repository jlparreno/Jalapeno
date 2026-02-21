#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    m_position(position),
    m_world_up(up),
    m_yaw(yaw),
    m_pitch(pitch),
    m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
    m_speed(SPEED),
    m_mouse_sensitivity(SENSITIVITY),
    m_fov(FOV)
{
    update_camera_vectors();
}

Camera::Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch) :
    m_position(glm::vec3(pos_x, pos_y, pos_z)),
    m_world_up(glm::vec3(up_x, up_y, up_z)),
    m_yaw(yaw),
    m_pitch(pitch),
    m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
    m_speed(SPEED),
    m_mouse_sensitivity(SENSITIVITY),
    m_fov(FOV)
{
    update_camera_vectors();
}

glm::mat4 Camera::get_view_matrix()
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::process_keyboard(CameraMovement direction, float delta_time)
{
    float velocity = m_speed * delta_time;

    if (direction == FORWARD)
        m_position += m_front * velocity;
    if (direction == BACKWARD)
        m_position -= m_front * velocity;
    if (direction == LEFT)
        m_position -= m_right * velocity;
    if (direction == RIGHT)
        m_position += m_right * velocity;
}

void Camera::orbit(float xoffset, float yoffset)
{
    m_yaw += xoffset * m_mouse_sensitivity;
    m_pitch += yoffset * m_mouse_sensitivity;

    update_camera_vectors();
}

void Camera::process_mouse_scroll(float y_offset)
{
    m_fov -= y_offset;
    if (m_fov < 1.0f)
        m_fov = 1.0f;
    if (m_fov > 45.0f)
        m_fov = 45.0f;
}

void Camera::update_camera_vectors()
{
    // Calculate the new front vector
    glm::vec3 front;

    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_front = glm::normalize(front);

    // Also re-calculate the right and up vector
    // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_right = glm::normalize(glm::cross(m_front, m_world_up));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}