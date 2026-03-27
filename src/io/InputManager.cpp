#include "InputManager.h"
#include "UIManager.h"

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
    // Camera will move only when we are hovering the viewport
    if (!UIManager::instance().get_viewport_hovered())
        return;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Renderer::instance().get_scene()->get_active_camera()->process_keyboard(FORWARD, Renderer::instance().delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Renderer::instance().get_scene()->get_active_camera()->process_keyboard(BACKWARD, Renderer::instance().delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Renderer::instance().get_scene()->get_active_camera()->process_keyboard(LEFT, Renderer::instance().delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Renderer::instance().get_scene()->get_active_camera()->process_keyboard(RIGHT, Renderer::instance().delta_time);
}

void InputManager::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //Update Renderer size
    Renderer::instance().resize(width, height);
}

void InputManager::mouse_cursor_pos_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    // Call GLFW callback first
    ImGui_ImplGlfw_CursorPosCallback(window, xposIn, yposIn);

    // Allow input if we are hovering the viewport even if ImGUI wants the mouse is true
    if (ImGui::GetIO().WantCaptureMouse && !UIManager::instance().get_viewport_hovered())
        return;

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
    // Call GLFW callback first
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

    // Allow input if we are hovering the viewport even if ImGUI wants the mouse is true
    if (ImGui::GetIO().WantCaptureMouse && !UIManager::instance().get_viewport_hovered())
        return;

    Renderer::instance().get_scene()->get_active_camera()->process_mouse_scroll(static_cast<float>(yoffset));
}
