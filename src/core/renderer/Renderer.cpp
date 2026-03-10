#include "Renderer.h"

#include "config.h"

Renderer::Renderer(const std::string& name, int width, int height) :
	m_name(name),
	m_window(nullptr),
	m_scene(nullptr),
	m_width(width),
	m_height(height)
{
	//Renderer initialization
	init();

	SPDLOG_INFO("Compiling shaders...");
	
	//TODO: Put this in the correct place for initialization (maybe configuration can tell which shaders to load and size of framebuffer)
	// Create and compile needed shaders
	auto& shader_mgr = ShaderManager::instance();
	std::string shader_dir = static_cast<std::string>(SHADERS_DIR);

	shader_mgr.load_program("lambert", { shader_dir + "simple_lighting.vert", shader_dir + "lambert.frag" });
	shader_mgr.load_program("phong", { shader_dir + "simple_lighting.vert", shader_dir + "phong.frag" });
	shader_mgr.load_program("pbr", { shader_dir + "simple_lighting.vert", shader_dir + "pbr.frag" });
	shader_mgr.load_program("depth", { shader_dir + "depth.vert", shader_dir + "depth.frag" });

	SPDLOG_INFO("Creating main Framebuffer");

	// Create needed framebuffers
	auto& framebuffer_mgr = FramebufferManager::instance();

	// Main FBO
	FramebufferSpec main_spec;
	main_spec.width = SRC_WIDTH; 
	main_spec.height = SRC_HEIGHT; 
	main_spec.samples = NUM_SAMPLES;
	main_spec.attachments = {
		{ GL_RGBA16F,          GL_COLOR_ATTACHMENT0        },
		{ GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT }
	};
	framebuffer_mgr.create_framebuffer("main", main_spec);

	// Create render passes (IN ORDER!)
	add_render_pass<ShadowPass>();
	add_render_pass<GeometryPass>();
}

void Renderer::run()
{
	// Enable alpha to coverage to manage blending for now
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

	// Main renderer loop
	while (!glfwWindowShouldClose(m_window))
	{
		//Time control
		float currentFrame = static_cast<float>(glfwGetTime());
		delta_time = currentFrame - last_frame;
		last_frame = currentFrame;

		// Display FPS and frame time in window title
		display_frame_times();

		// Process input events
		InputManager::instance().process_input(m_window);

		// Render all scene objects
		render_scene();

		// Update and render UI
		UIManager::update();
		UIManager::render();

		// Prepare next frame
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	// Cleanup
	terminate();
}

void Renderer::init()
{
	// GLFW initialization
	if (!glfwInit()) 
	{
		SPDLOG_ERROR("Failed GLFW initialization");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// GLFW window creation
	m_window = glfwCreateWindow(m_width, m_height, "Jalapeno", NULL, NULL);

	if (m_window == NULL)
	{
		SPDLOG_ERROR("Failed to create GLFW window");
		glfwTerminate();
	}

	// Load render engine icon for GLFW window
	int icon_width, icon_height, icon_channels;
	std::string icon_path = static_cast<std::string>(TEXTURES_DIR) + "jalapeno_logo.png";
	unsigned char* icon_pixels = stbi_load(icon_path.c_str(), &icon_width, &icon_height, &icon_channels, 4);
	if (!icon_pixels)
	{
		SPDLOG_WARN("Error loading Jalapeno icon");
	}

	GLFWimage icon;
	icon.width = icon_width;
	icon.height = icon_height;
	icon.pixels = icon_pixels;

	glfwSetWindowIcon(m_window, 1, &icon);

	glfwMakeContextCurrent(m_window);
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// TODO: Support V-Sync
	// glfwSwapInterval(1);

	// Init UI
	UIManager::init(m_window);

	// Init Input Manager
	InputManager::instance().init(m_window);

	// GLAD: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		SPDLOG_ERROR("Failed to initialize GLAD");
	}
}

void Renderer::render_scene()
{
	//Check valid scene
	if (!m_scene)
	{
		SPDLOG_ERROR("No valid scene loaded");
		glClearColor(0.16f, 0.18f, 0.20f, 1.0f);
		return;
	}

	// Call to the configured Render Passes
	for (auto& pass : m_passes)
	{
		pass->execute(*m_scene);
	}

	// Blit to GLFW default FBO
	Framebuffer* main_fbo = FramebufferManager::instance().get_framebuffer("main");

	glBindFramebuffer(GL_READ_FRAMEBUFFER, main_fbo->get_ID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glViewport(0, 0, m_width, m_height);
	
	// Blit operation (only color at the moment). If our FBO is multi-sampled, this vlit will resolve into the GLFW FBO (no MS)
	glBlitFramebuffer(0, 0, main_fbo->get_width(), main_fbo->get_height(), 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Unbind
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Renderer::resize(int width, int height)
{
	m_width = width; 
	m_height = height;

	//Update framebuffers size
	FramebufferManager::instance().resize_all(width, height);
}

void Renderer::display_frame_times()
{
	m_time_counter += delta_time;

	if (m_time_counter >= 1.0f) 
	{
		m_time_counter = 0.0f;
		m_fps = 1.0f / delta_time;

		//Time in ms (*1000.0f), 2 decimals (*100.0f / 100.0f)
		float time_ms = std::round(delta_time * 1000.0f * 100.0f) / 100.0f;

		//Get string with 2 decimals
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(2) << time_ms;

		std::string window_title = "Jalapeno    |    FPS: " + std::to_string(m_fps) + "    |    Time(ms): " + oss.str();
		glfwSetWindowTitle(m_window, window_title.c_str());
	}
}

void Renderer::terminate()
{
	auto& shader_mgr = ShaderManager::instance();
	glDeleteProgram(shader_mgr.get_program("lambert")->get_id());
	glDeleteProgram(shader_mgr.get_program("phong")->get_id());
	glDeleteProgram(shader_mgr.get_program("pbr")->get_id());
	glDeleteProgram(shader_mgr.get_program("depth")->get_id());

	// TODO: Check if loaded models buffers are correctly destroyed
	//glDeleteVertexArrays(1, &cube_VAO);
	//glDeleteVertexArrays(1, &light_VAO);
	//glDeleteBuffers(1, &VBO);

	// Terminate UI and GLFW
	UIManager::terminate();
	glfwTerminate();
}