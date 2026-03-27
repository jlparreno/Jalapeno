#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "Model.h"
#include "Scene.h"
#include "FramebufferManager.h"
#include "ShaderManager.h"
#include "InputManager.h"
#include "UIManager.h"
#include "ShadowPass.h"
#include "GeometryPass.h"
#include "SkyboxPass.h"
#include "config.h"

#include <string>
#include <unordered_map>

/**
 * @class Renderer
 * 
 * @brief Central rendering system responsible for drawing scenes, managing the main window,
 *        and controlling frame lifecycle.
 *
 * The Renderer is implemented as a singleton, ensuring that only a single global rendering
 * context exists throughout the engine. It manages window creation, viewport configuration,
 * the render loop, and delegates draw operations to the scene objects.
 *
 * The Renderer also handles framebuffer resizing, profiling hooks, and calls the appropriate
 * rendering passes
 */
class Renderer
{
public:

    /**
     * @brief Retrieves the Renderer singleton instance.
     *
     * The first time this is called, it constructs the renderer with default
     * dimensions (1920x1080). Subsequent calls return the same instance.
     *
     * @return Reference to the Renderer singleton.
     */
    static Renderer& instance()
    {
        static Renderer instance("Renderer", SRC_WIDTH, SRC_HEIGHT);
        return instance;
    }

    /**
     * @brief Deleted copy constructor to prevent copying of the singleton
     */
    Renderer(const Renderer&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying of the singleton
     */
    Renderer& operator=(const Renderer&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving of the singleton
     */
    Renderer(Renderer&&) = delete;

    /**
     * @brief Deleted move assignment operator to prevent moving of the singleton
     */
    Renderer& operator=(Renderer&&) = delete;

    /**
     * @brief Adds a new render pass of type T to the render pipeline
     *
     * @tparam T The render pass type, derived from RenderPass
     *
     * @return Pointer to the created render pass of type T
     */
    template<typename T>
    T* add_render_pass()
    {
        // Create the render pass
        auto pass = std::make_unique<T>();
        T* raw = pass.get();

        // Guardarlo como Material
        m_passes.push_back(std::move(pass));

        return raw;
    }

    /**
     * @brief Main execution loop of the renderer.
     *
     * This method enters the main loop, processes input,
     * updates internal systems, draws the active scene, and manages
     * framerate timing until the window is closed.
     */
    void run();

    /**
     * @brief Resizes the renderer’s output viewport and any dependent buffers.
     *
     * Should be called whenever the window or framebuffer size changes.
     *
     * @param width New width in pixels.
     * @param height New height in pixels.
     */
    void resize(int width, int height);

    /**
     * @brief Assigns the scene to be rendered and initializes UI Manager
     *
     * @param scene Pointer to the Scene object to render
     */
    void set_scene(Scene* scene);

    /**
     * @brief Retrieves the currently bound scene.
     *
     * @return Pointer to the active Scene, or nullptr if none has been set.
     */
    Scene* get_scene() { return m_scene; }


    // Timing
    float delta_time = 0.0f;	// Time between current frame and last frame
    float last_frame = 0.0f;    // Time of last frame

    float		    m_time_counter{ 0.0f };
    unsigned int	m_fps{ 0 };

    // Input management
    float last_x{ 0.0f };
    float last_y{ 0.0f };

    bool first_mouse{ true };

private:

    // Private constructor and destructor to enforce singleton pattern
    Renderer(const std::string& name, int width, int height);
    ~Renderer() = default;

    /**
     * @brief Initializes GLFW, GLAD, the main window, the UI Manager,
     * shaders, and framebuffers.
     */
    void init();

    /**
     * @brief Renders the active scene.
     *
     * Should be invoked once per frame.
     * Handles clearing, framebuffer usage, and model rendering.
     * At the end, blits the resulting image to GLFW main FBO.
     */
    void render_scene();

    /**
     * @brief Cleans up the renderer before shutdown.
     *
     * Frees all resources,terminates UI and GLFW.
     */
    void terminate();

    /**
     * @brief Internal profiling.
     *
     * Can be used to measure frame time and GPU timings.
     */
    void display_frame_times();


    // Name of the renderer
    std::string     m_name;

    // Pointer to the scene currently being rendered
    Scene*          m_scene;

    // Pointer to the GLFW window used for output
    GLFWwindow*     m_window;

    // Renderer dimensions
    int             m_width;
    int             m_height;

    // Ordered list of passes configured in the renderer
    std::vector<std::unique_ptr<RenderPass>> m_passes;
};
