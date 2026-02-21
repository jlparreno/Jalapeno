#include "InputManager.h"

void InputManager::init(GLFWwindow* window)
{
    m_window = window;
    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);
}

void InputManager::process_input(GLFWwindow* window)
{
    auto& renderer = Renderer::instance();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Renderer::instance().get_scene()->get_active_camera()->process_keyboard(FORWARD, renderer.delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Renderer::instance().get_scene()->get_active_camera()->process_keyboard(BACKWARD, renderer.delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Renderer::instance().get_scene()->get_active_camera()->process_keyboard(LEFT, renderer.delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Renderer::instance().get_scene()->get_active_camera()->process_keyboard(RIGHT, renderer.delta_time);
}


void InputManager::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //Update Renderer size
    Renderer::instance().resize(width, height);
}

void InputManager::mouse_cursor_pos_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    auto& renderer = Renderer::instance();

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (renderer.first_mouse)
    {
        renderer.last_x = xpos;
        renderer.last_y = ypos;
        renderer.first_mouse = false;
    }

    float xoffset = xpos - renderer.last_x;
    float yoffset = renderer.last_y - ypos; // reversed since y-coordinates go from bottom to top

    renderer.last_x = xpos;
    renderer.last_y = ypos;

    ImGuiIO& io = ImGui::GetIO();

    // Detect pressed buttons
    bool alt_pressed = io.KeyAlt;
    bool left_click = ImGui::IsMouseDown(0);

    // Maya rotation: Alt + left click
    if (alt_pressed && left_click)
    {
        Renderer::instance().get_scene()->get_active_camera()->orbit(xoffset, yoffset);
    }

    // Reset cursor when it is not used
    if (!(left_click))
    {
        renderer.first_mouse = true;
    }
}

void InputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Renderer::instance().get_scene()->get_active_camera()->process_mouse_scroll(static_cast<float>(yoffset));
}
