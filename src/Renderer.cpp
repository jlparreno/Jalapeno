#include "Renderer.h"

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

	shader_mgr.load_program("texture", { shader_dir + "simple_texture.vert", shader_dir + "simple_texture.frag" });
	shader_mgr.load_program("lighting_phong", { shader_dir + "simple_lighting.vert", shader_dir + "phong.frag" });

	SPDLOG_INFO("Creating main Framebuffer");

	// Create needed framebuffers
	auto& framebuffer_mgr = FramebufferManager::instance();
	framebuffer_mgr.create_framebuffer("main", SRC_WIDTH, SRC_HEIGHT);
}

void Renderer::run()
{
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

	// TODO: Render Passes
	// Here the renderer will call the different passes. It will control de framebuffer bindings.
	// The responsibility of draw each pass is for each object.
	// Draw call will include which pass are we drawing, and each object decides what to do
	

	// Forward rendering: Main Pass
	Framebuffer* fbo = FramebufferManager::instance().get_framebuffer("main");

	// Check valid FBO
	if (!fbo)
	{
		SPDLOG_ERROR("Invalid FBO for Main Pass");
		return;
	}
	
	fbo->bind();
	render_all_models();
	fbo->unbind();

	//TODO: More passes??

	// Blit to GLFW default FBO
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->get_ID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glViewport(0, 0, m_width, m_height);
	
	// Blit operation (only color at the moment)
	glBlitFramebuffer(0, 0, fbo->get_width(), fbo->get_height(), 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Unbind
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Renderer::render_all_models()
{
	// Clear screen with out default clear color
	glClearColor(0.16f, 0.18f, 0.20f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Configure OpenGL state
	// TODO: Create OpenGL context manager class
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glViewport(0, 0, m_width, m_height);

	// Setup matrices with scene camera
	Camera* camera = m_scene->get_active_camera();
	float aspect = glm::clamp((float)m_width / (float)m_height, MIN_ASPECT, MAX_ASPECT);

	glm::mat4 view_mat = camera->get_view_matrix();
	glm::mat4 projection_mat = glm::perspective(glm::radians(camera->get_fov()), aspect, 0.1f, 100.0f);

	// Iterate all scene objects calling each draw function
	for (const Model* model : m_scene->get_scene_models())
	{
		Material* material = model->get_material();

		if (!material)
		{
			SPDLOG_ERROR("Error getting the material for rendering model: {}", model->get_name());
			continue;
		}
			
		// Get shader from material
		ShaderProgram* shader = material->get_shader();

		if (!shader)
		{
			SPDLOG_ERROR("Error getting the shader program for rendering model: {}", model->get_name());
			continue;
		}

		//Bind and set matrices
		shader->bind();

		shader->set_uniform("model", model->get_model_matrix());
		shader->set_uniform("view", view_mat);
		shader->set_uniform("projection", projection_mat);
		shader->set_uniform("view_pos", camera->get_position());

		// Lights
		// TODO: This loop is to add the possibility to add more lights in the future
		// Now we only will see the last light added to the scene
		for (const auto& light : m_scene->get_scene_lights())
		{
			shader->set_uniform("light.position", light->get_position());
			shader->set_uniform("light.ambient", light->get_ambient());
			shader->set_uniform("light.diffuse", light->get_diffuse());
			shader->set_uniform("light.specular", light->get_specular());
		}

		// Bind material textures
		material->bind_textures();

		// Set specific material uniforms
		material->apply_uniforms();

		// Draw model geometry
		model->draw();

		shader->unbind();
	}
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
	glDeleteProgram(shader_mgr.get_program("texture")->get_id());
	glDeleteProgram(shader_mgr.get_program("lighting_phong")->get_id());

	// TODO: Check if loaded models buffers are correctly destroyed
	//glDeleteVertexArrays(1, &cube_VAO);
	//glDeleteVertexArrays(1, &light_VAO);
	//glDeleteBuffers(1, &VBO);

	// Terminate UI and GLFW
	UIManager::terminate();
	glfwTerminate();
}