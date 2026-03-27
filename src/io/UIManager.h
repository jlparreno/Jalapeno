#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif

#include "Scene.h"
#include "PBRMaterial.h"
#include "PhongMaterial.h"
#include "LambertMaterial.h"
#include "SkyBoxPass.h"
#include "ConsoleSink.h"

/**
 * @class UIManager
 *
 * @brief Singleton manager responsible for all ImGui-based user interface rendering
 *
 * The UIManager centralizes the initialization, update, rendering, and shutdown
 * of the engine's user interface. It abstracts away ImGui-specific setup so the 
 * rest of the engine remains UI-agnostic.
 *
 * The layout consists of a custom borderless title bar, a fullscreen dockspace
 * hosting the hierarchy, viewport, properties, materials, and console panels,
 * and a status bar at the bottom displaying performance metrics.
 */
class UIManager
{
public:

    /**
     * @brief Returns the single instance of the UIManager
     * 
     * @return Reference to the unique UIManager instance
     */
    static UIManager& instance()
    {
        static UIManager instance;
        return instance;
    }

    // Non-copyable, non-movable (singleton pattern)
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;
    UIManager(const UIManager&&) = delete;
    UIManager& operator=(UIManager&&) = delete;

    /**
     * @brief Initializes ImGui, loads fonts, applies the editor theme, and sets up panels
     *
     * Configures the ImGui context for use with GLFW and OpenGL, enables docking,
     * registers the console sink with the default spdlog logger, and stores the
     * GLFW window handle
     *
     * @param window Pointer to the GLFW window used by the application
     */
    void init(GLFWwindow* window);

    /**
     * @brief Begins a new ImGui frame and renders all editor panels
     *
     * Must be called once per frame before render(). Starts a new ImGui frame,
     * renders the title bar, dockspace, and all active panels in order.
     */
    void update();

    /**
     * @brief Submits all ImGui draw data to the GPU
     *
     * Must be called once per frame after update(). Finalizes the ImGui frame
     * and issues all draw commands produced by the editor panels.
     */
    void render();

    /**
     * @brief Shuts down ImGui and releases all UI resources
     *
     * Shuts down the GLFW and OpenGL ImGui backends and destroys the ImGui context.
     * Must be called once during application shutdown.
     */
    void terminate();

    /**
     * @brief Sets the active scene displayed by the hierarchy panel
     *
     * Should be called by the Renderer whenever a new scene is loaded or changed.
     *
     * @param scene Pointer to the Scene object containing all renderables and lights
     */
    void set_scene(Scene* scene) { m_scene = scene; }

    /**
     * @brief Sets the active skybox pass configured in the renderer
     *
     * Should be called by the Renderer whenever a new ShadowPass is configured.
     * Allows to enable/disable the execution od the render pass through the UI.
     *
     * @param scene Pointer to the SkyBoxPass
     */
    void set_skybox_pass(SkyboxPass* pass) { m_skybox_pass = pass; }

    /**
     * @brief Returns whether the mouse is hovering the viewport panel
     *
     * Used by the InputManager to determine whether mouse input should be
     * routed to the camera or consumed by ImGui.
     *
     * @return True if the mouse is over the viewport, false otherwise
     */
    bool get_viewport_hovered() const { return m_viewport_hovered; }

    /**
     * @brief Returns whether the viewport panel currently has input focus
     *
     * Used by the InputManager to determine whether keyboard input should be
     * routed to the camera or consumed by ImGui.
     *
     * @return True if the viewport panel is focused, false otherwise
     */
    bool get_viewport_focused() const { return m_viewport_focused; }

private:

    /// Private constructor and destructor to enforce the singleton pattern.
    UIManager() = default;
    ~UIManager() = default;

    /**
     * @brief Renders the custom borderless title bar
     *
     * Displays the application name centered in the bar and provides
     * draggable window movement and minimize, maximize, and close buttons.
     * Replaces the native Windows title bar decoration.
     */
    void draw_title_bar();

    /**
     * @brief Renders the fullscreen dockspace that hosts all editor panels
     *
     * Creates a fullscreen ImGui window between the title bar and status bar
     * that acts as the root docking target. Also renders the the bottom status 
     * bar showing FPS and frame time.
     */
    void draw_dockspace();

    /**
     * @brief Renders the scene hierarchy panel
     *
     * Displays all renderables and lights in the active scene organized
     * into collapsible sections. Clicking an item sets it as the selected
     * object for the properties panel.
     */
    void draw_hierarchy();

    /**
     * @brief Renders the properties panel for the currently selected object
     *
     * Shows editable transform properties for renderables, and light-specific
     * properties for lights. Displays a placeholder if nothing is selected.
     */
    void draw_properties();

    /**
     * @brief Renders the materials panel
     *
     * Displays all materials loaded in the MaterialManager as a scrollable
     * list of cards. Each card shows the material name, type badge, and texture 
     * thumbnails for all non-default textures. 
     * Materials using only default textures show a compact name-only card.
     */
    void draw_materials();

    /**
     * @brief Renders a single material card inside the materials panel
     *
     * Draws a styled child window containing the material name, type badge,
     * and texture thumbnails. Hovering a thumbnail shows a larger preview tooltip.
     * If all textures are the default white texture, renders a compact card
     * showing only the name and type.
     *
     * @param name     The unique name of the material
     * @param material Pointer to the Material to display
     */
    void draw_material_card(const std::string& name, Material* material);

    /**
     * @brief Renders the console log panel
     *
     * Displays log messages captured from the spdlog default logger via
     * the ConsoleSink. Supports filtering by log level (INFO, WARN, ERROR, DEBUG),
     * text search, autoscroll, and clearing the log history.
     */
    void draw_log_panel();

    /**
     * @brief Renders the render settings panel
     *
     * Displays render settings to enable/disable in real time and see the changes
     * in the renderer. Has a section with configurable render passes and another
     * one with specific render modes (Vsync, wireframe...)
     */
    void draw_render_settings();

    /**
     * @brief Renders the viewport panel
     *
     * Displays the resolved (non-MSAA) framebuffer as an ImGui image filling
     * the panel area. Resizes all resizable framebuffers when the panel size
     * changes, and updates the viewport hover and focus flags used by the
     * InputManager for input routing.
     */
    void draw_viewport();

    // Pointer to the active scene, used to populate the hierarchy panel
    Scene* m_scene{ nullptr };

    // Pointer to the GLFW window, used by the title bar
    GLFWwindow* m_window{ nullptr };

    // Shared spdlog sink that captures log messages for display in the console panel
    std::shared_ptr<ConsoleSinkMt> m_sink;

    // Pointer to the Skybox Pass configured in the renderer, to enable/disable it
    SkyboxPass* m_skybox_pass{ nullptr };

    // Currently selected renderable in the hierarchy panel. Nullptr if none
    Renderable* m_selected_renderable{ nullptr };

    // Currently selected light in the hierarchy panel. Nullptr if none
    Light* m_selected_light{ nullptr };

    // Currently selected material in the materials panel. Nullptr if none
    Material* m_selected_material{ nullptr };

    // --- VIEWPORT ---

    // Viewport panel width and height in pixels
    int m_viewport_width{ 0 };
    int m_viewport_height{ 0 };

    // Flags to save if the mouse is currently hovering the viewport panel
    bool m_viewport_hovered{ false };

    // Flag to save if the viewport panel currently has input focus
    bool m_viewport_focused{ false };

    // --- MATERIALS ---

    // Size in pixels of texture thumbnails in the materials panel
    float m_thumbnail_size{ 64.0f };

    // Spacing in pixels between elements inside material cards
    float m_card_padding{ 8.0f };

    // --- CONSOLE ---
     
    // Flags that control if specific level messages are shown in the console panel
    bool m_show_info{ true };
    bool m_show_warn{ true };
    bool m_show_error{ true };
    bool m_show_debug{ true };

    // Text buffer for the console search filter input
    char m_search_buf[128]{ "" };

    // Whether the console automatically scrolls to the latest log message
    bool m_autoscroll{ true };

    // Flag to trigger an immediate scroll to the bottom on the next frame
    bool m_scroll_to_bottom{ false };

    // --- TITLE BAR ---
    
    // True while the user is dragging the title bar to move the window
    bool m_dragging{ false };

    // Cursor offsets from the window origin recorded at drag start
    double m_drag_offset_x{ 0.0 };
    double m_drag_offset_y{ 0.0 };
};
