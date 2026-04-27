#include "Renderer.h"

#include "UIManager.h"
#include "config.h"

Renderer::Renderer(const std::string& name, int width, int height) :
	m_name(name),
	m_window(nullptr),
	m_scene(nullptr),
	m_width(width),
	m_height(height)
{
	// Renderer initialization
	init();

	// Init shaders
	init_shaders();

	// Create needed framebuffers
	auto& framebuffer_mgr = FramebufferManager::instance();

	SPDLOG_INFO("Creating main Framebuffer");

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

	SPDLOG_INFO("Creating MSAA resolve Framebuffer");

	// MSAA resolver buffer
	FramebufferSpec resolve_spec;
	resolve_spec.width = SRC_WIDTH;
	resolve_spec.height = SRC_HEIGHT;
	resolve_spec.samples = 1;
	resolve_spec.resizable = true;
	resolve_spec.attachments = {
		{ GL_RGBA16F, GL_COLOR_ATTACHMENT0 }
	};
	FramebufferManager::instance().create_framebuffer("resolve", resolve_spec);

	// Create Light UBOs
	auto& buffer_mgr = BufferManager::instance();
	buffer_mgr.create_uniform_buffer("point_lights",       sizeof(PointLightBlock),       POINT_LIGHT_UBO_BINDING);
	buffer_mgr.create_uniform_buffer("directional_lights", sizeof(DirectionalLightBlock), DIRECTIONAL_LIGHT_UBO_BINDING);

	// Create render passes (IN ORDER!)
	add_render_pass<ShadowPass>();
	add_render_pass<GeometryPass>();
	
	SkyboxPass* skybox = add_render_pass<SkyboxPass>();
	skybox->load_from_hdr(static_cast<std::string>(TEXTURES_DIR) + "kloofendal_48d_partly_cloudy_puresky_2k.hdr");

	// Set skybox pass for UI control
	UIManager::instance().set_skybox_pass(skybox);
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

		// Process input events
		InputManager::instance().process_input(m_window);

		// Render all scene objects
		render_scene();

		// Update and render UI
		UIManager::instance().update();
		UIManager::instance().render();

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

	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // remove Window bar
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

	// Center window in monitor
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	int pos_x = (mode->width - m_width) / 2;
	int pos_y = (mode->height - m_height) / 2;
	glfwSetWindowPos(m_window, pos_x, pos_y);

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
	stbi_image_free(icon_pixels);

	glfwMakeContextCurrent(m_window);
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// V-Sync disabled
	glfwSwapInterval(0);

	// GLAD: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		SPDLOG_ERROR("Failed to initialize GLAD");
	}

	// Init UI
	UIManager::instance().init(m_window);

	// Init Input Manager
	InputManager::instance().init(m_window);
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
		if (pass->get_enabled())
		{
			pass->execute(*m_scene);
		}
	}

	// Blit MSAA → resolve FBO (resolve will be rendered in ImGUI viewport)
	Framebuffer* main_fbo = FramebufferManager::instance().get_framebuffer("main");
	Framebuffer* resolve_fbo = FramebufferManager::instance().get_framebuffer("resolve");

	glBindFramebuffer(GL_READ_FRAMEBUFFER, main_fbo->get_ID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolve_fbo->get_ID());

	glBlitFramebuffer(0, 0, main_fbo->get_width(), main_fbo->get_height(),
				      0, 0, resolve_fbo->get_width(), resolve_fbo->get_height(),
					  GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::resize(int width, int height)
{
	m_width = width; 
	m_height = height;

	//Update framebuffers size
	FramebufferManager::instance().resize_all(width, height);
}

void Renderer::set_scene(Scene* scene)
{ 
	m_scene = scene; 

	// Set scene to UI Manager
	UIManager::instance().set_scene(scene);
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
	// Release all GPU resources before destroying the OpenGL context
	ShaderManager::instance().clear();
	BufferManager::instance().clear();
	FramebufferManager::instance().clear();
	TextureManager::instance().clear();

	// Terminate UI and GLFW
	UIManager::instance().terminate();
	glfwTerminate();
}

void Renderer::init_shaders()
{
	SPDLOG_INFO("Compiling shaders...");

	auto& shader_mgr = ShaderManager::instance();
	const std::string shaders_dir = static_cast<std::string>(SHADERS_DIR);

	// Lighting
	shader_mgr.load_program("lambert",  { shaders_dir + "simple_lighting.vert", shaders_dir + "lambert.frag" });
	shader_mgr.load_program("phong",	{ shaders_dir + "simple_lighting.vert", shaders_dir + "phong.frag"	 });
	shader_mgr.load_program("pbr",		{ shaders_dir + "simple_lighting.vert", shaders_dir + "pbr.frag"	 });

	// Shadows
	shader_mgr.load_program("directional_shadows",	{ shaders_dir + "depth.vert",        shaders_dir + "depth.frag" });
	shader_mgr.load_program("point_shadows",		{ shaders_dir + "point_shadow.vert", shaders_dir + "point_shadow.geom", shaders_dir + "point_shadow.frag" });

	// Skybox / environment
	shader_mgr.load_program("skybox",              { shaders_dir + "skybox.vert",				shaders_dir + "skybox.frag" });
	shader_mgr.load_program("equirect_to_cubemap", { shaders_dir + "equirect_to_cubemap.vert",	shaders_dir + "cubemap_capture.geom", shaders_dir + "equirect_to_cubemap.frag" });
}

void Renderer::set_vsync(bool enabled)
{
	m_vsync = enabled;
	glfwSwapInterval(enabled ? 1 : 0);
}

void Renderer::set_wireframe(bool enabled)
{
	m_wireframe = enabled;
	PipelineState::set_wireframe_override(enabled);
}