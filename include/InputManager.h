#pragma once

#include "Renderer.h"

/**
 * @class InputManager
 * 
 * @brief Centralized singleton manager for handling all input from GLFW
 *
 * InputManager is responsible for processing keyboard, mouse, and scroll input
 * events and providing a unified interface for the rest of the engine.
 */
class InputManager
{
public:

    /**
     * @brief Returns the global InputManager singleton instance.
     *
     * Ensures only one instance exists and provides a central access point
     * for all input-related operations.
     *
     * @return Reference to the InputManager singleton.
     */
    static InputManager& instance()
    {
        static InputManager instance;
        return instance;
    }

    /**
     * @brief Deleted copy constructor to prevent copying of the singleton.
     */
    InputManager(const InputManager&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying of the singleton.
     */
    InputManager& operator=(const InputManager&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving of the singleton.
     */
    InputManager(InputManager&&) = delete;

    /**
     * @brief Deleted move assignment operator to prevent moving of the singleton.
     */
    InputManager& operator=(InputManager&&) = delete;

    /**
     * @brief Initializes the input manager with a GLFW window.
     *
     * Sets up the GLFW callbacks for input events.
     * Must be called once before using any input queries.
     *
     * @param window Pointer to the GLFW window used for input events.
     */
    void init(GLFWwindow* window);

    /**
     * @brief Processes all input events for the current frame.
     *
     * This function is meant to be called once per frame, typically at the
     * beginning of the render loop.
     *
     * @param window Pointer to the GLFW window receiving input.
     */
    static void process_input(GLFWwindow* window);

private:

    // Private constructor and destructor to enforce the singleton pattern.
    InputManager() = default;
    ~InputManager() = default;

    /**
     * @brief Callback for window resize events.
     *
     * Automatically called by GLFW when the framebuffer size changes.
     *
     * @param window Pointer to the GLFW window.
     * @param width New width of the framebuffer in pixels.
     * @param height New height of the framebuffer in pixels.
     */
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    /**
     * @brief Callback for mouse movement events.
     *
     * Automatically called by GLFW when the cursor moves within the window.
     *
     * @param window Pointer to the GLFW window.
     * @param xposIn New x-coordinate of the cursor in screen space.
     * @param yposIn New y-coordinate of the cursor in screen space.
     */
    static void mouse_cursor_pos_callback(GLFWwindow* window, double xposIn, double yposIn);

    /**
     * @brief Callback for mouse scroll events.
     *
     * Automatically called by GLFW when the scroll wheel is used.
     *
     * @param window Pointer to the GLFW window.
     * @param xoffset Scroll offset along the x-axis.
     * @param yoffset Scroll offset along the y-axis.
     */
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


    // Pointer to the GLFW window associated with this input manager.
    GLFWwindow* m_window = nullptr;
};
