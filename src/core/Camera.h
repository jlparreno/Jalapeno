#pragma once

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;

/**
 * @enum CameraMovement
 * @brief Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
 */
enum CameraMovement 
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

/**
 * @class Camera
 * @brief A flexible FPS-style camera for OpenGL
 *
 * This class implements a 3D camera that uses Euler angles to calculate its
 * orientation and produces a view matrix compatible with OpenGL's rendering
 * pipeline. It supports movement and rotation based on keyboard and mouse input,
 * providing an intuitive first-person or free-fly navigation system.
 *
 * The camera maintains directional vectors (front, right, up), updates them
 * automatically when yaw or pitch angles change, and exposes methods to process
 * user input, adjust field of view, and retrieve the current view matrix.
 */
class Camera
{
public:

    /**
     * @brief Constructs a new Camera using vector parameters
     * 
     * @param position Initial position of the camera
     * @param up World-space up vector
     * @param yaw Initial yaw angle
     * @param pitch Initial pitch angle
     */
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    /**
     * @brief Constructs a new Camera using scalar values
     * 
     * @param pos_x X coordinate of the camera position
     * @param pos_y Y coordinate of the camera position
     * @param pos_z Z coordinate of the camera position
     * @param up_x X component of the world up vector
     * @param up_y Y component of the world up vector
     * @param up_z Z component of the world up vector
     * @param yaw Initial yaw angle
     * @param pitch Initial pitch angle
     */
    Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch);

    /**
     * @brief Returns the view matrix calculated using Euler angles and LookAt
     * 
     * @return The camera view matrix
     */
    glm::mat4 get_view_matrix();

    /**
     * @brief Processes keyboard input to move the camera
     * 
     * @param direction Movement direction
     * @param delta_time Time elapsed between frames
     */
    void process_keyboard(CameraMovement direction, float delta_time);

    /**
     * @brief Rotates the camera around the target point (orbit).
     * 
     * @param xoffset Horizontal mouse movement in pixels.
     * @param yoffset Vertical mouse movement in pixels.
     *
     * Updates the yaw and pitch angles of the camera based on the mouse offsets
     * and recalculates the front, right, and up vectors.
     * Typical use: Alt + left mouse button drag in viewport.
     */
    void orbit(float xoffset, float yoffset);

    /**
     * @brief Processes mouse scroll wheel input affecting the camera field of view
     * 
     * @param y_offset Scroll-wheel vertical offset
     */
    void process_mouse_scroll(float y_offset);

    /**
     * @brief Returns the camera's current position
     *
     * @return Current position value
     */
    glm::vec3 get_position() const { return m_position; };

    /**
     * @brief Returns the camera's front vector. The view direction
     *
     * @return Current front vector value
     */
    glm::vec3 get_front() const { return m_front; };

    /**
     * @brief Returns the camera's current field of view
     * 
     * @return Current FOV value
     */
    float get_fov() const { return m_fov; };

private:

    /**
     * @brief Recomputes the camera's direction vectors from Euler angles
     */
    void update_camera_vectors();

    // Camera Attributes
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_world_up;

    // Euler Angles
    float m_yaw;
    float m_pitch;

    // Camera options
    float m_speed;
    float m_mouse_sensitivity;
    float m_fov;
};

