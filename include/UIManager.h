#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

/**
 * @class UIManager
 * 
 * @brief Centralized manager for all ImGui-based user interface rendering.
 *
 * The UIManager class provides a static interface for initializing,
 * updating, rendering, and shutting down the graphical user interface.
 * It abstracts away all ImGui-specific setup and frame management,
 * allowing the rest of the engine to remain UI-agnostic.
 */
class UIManager 
{
public:

	/**
	 * @brief Initializes ImGui and sets up the UI context for usage.
	 *
	 * This function configures ImGui for use with GLFW and OpenGL,
	 * creates the context, loads fonts, and sets up docking if enabled.
	 *
	 * @param window Pointer to the GLFW window used by the application.
	 */
	static void init(GLFWwindow* window);

	/**
	 * @brief Begins a new ImGui frame and updates all UI elements.
	 *
	 * This should be called once per frame. It refreshes input handling, 
	 * prepares ImGui windows, and processesall UI logic for editor panels,
	 * viewport windows, or toolbars.
	 */
	static void update();

	/**
	 * @brief Renders all ImGui draw data to the screen.
	 *
	 * After the engine has completed its rendering for the current frame,
	 * this method must be called to finalize the ImGui frame and issue
	 * all rendering commands produced by the UI.
	 */
	static void render();

	/**
	 * @brief Cleans up and destroys the ImGui context.
	 *
	 * This should be called only once during application shutdown.
	 * It ensures that all resources allocated by ImGui are properly released.
	 */
	static void terminate();

private:

	// TODO: Declare and define specific UI sections rendering functions.
	// These methods should encapsulate UI-specific module rendering, 
	// such as texture inspector windows, hierarchy panes, or asset browsers.
	
	//static void textures_menu(TextureManager& tex_mgr);

	// Use of ImGUI Docking
	//static void DrawDockspace();
	//static void DrawHierarchy();
	//static void DrawViewport(uint32_t textureID);

};
