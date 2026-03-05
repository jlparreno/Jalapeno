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

	SPDLOG_INFO("Creating main Framebuffer");

	// Create needed framebuffers
	auto& framebuffer_mgr = FramebufferManager::instance();
	framebuffer_mgr.create_framebuffer("main", SRC_WIDTH, SRC_HEIGHT, NUM_SAMPLES);
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

	draw_all_renderables();

	fbo->unbind();

	//TODO: More passes??

	// Blit to GLFW default FBO
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->get_ID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glViewport(0, 0, m_width, m_height);
	
	// Blit operation (only color at the moment). If our FBO is multi-sampled, this vlit will resolve into the GLFW FBO (no MS)
	glBlitFramebuffer(0, 0, fbo->get_width(), fbo->get_height(), 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Unbind
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Renderer::draw_all_renderables()
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
	for (const Renderable* renderable : m_scene->get_scene_renderables())
	{
		glm::mat4 model = renderable->get_model_matrix();

		for (auto& mesh : renderable->get_meshes())
		{
			Material* material = mesh.get_material();
			ShaderProgram* shader = material->get_shader();

			shader->bind();

			shader->set_uniform("model", model);
			shader->set_uniform("view", view_mat);
			shader->set_uniform("projection", projection_mat);
			shader->set_uniform("view_pos", camera->get_position());

			//Upload lights data to the shader
			upload_lights(shader);

			material->apply_uniforms(shader);

			mesh.draw(shader);

			shader->unbind();
		}
	}
}

void Renderer::upload_lights(ShaderProgram* shader)
{
	int point_index = 0;
	int directional_index = 0;

	for (auto& light : m_scene->get_scene_lights())
	{
		if (auto* pl = dynamic_cast<PointLight*>(light.get()))
		{
			std::string base = "u_point_lights[" + std::to_string(point_index) + "].";
			shader->set_uniform(base + "position", pl->get_position());
			shader->set_uniform(base + "color", pl->get_color());
			shader->set_uniform(base + "intensity", pl->get_intensity());
			shader->set_uniform(base + "constant", pl->get_constant());
			shader->set_uniform(base + "linear", pl->get_linear());
			shader->set_uniform(base + "quadratic", pl->get_quadratic());
			point_index++;
		}
		else if (auto* dl = dynamic_cast<DirectionalLight*>(light.get()))
		{
			std::string base = "u_directional_lights[" + std::to_string(directional_index) + "].";
			shader->set_uniform(base + "direction", dl->get_direction());
			shader->set_uniform(base + "color", dl->get_color());
			shader->set_uniform(base + "intensity", dl->get_intensity());
			directional_index++;
		}
	}

	shader->set_uniform("u_num_point_lights", point_index);
	shader->set_uniform("u_num_directional_lights", directional_index);
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

	// TODO: Check if loaded models buffers are correctly destroyed
	//glDeleteVertexArrays(1, &cube_VAO);
	//glDeleteVertexArrays(1, &light_VAO);
	//glDeleteBuffers(1, &VBO);

	// Terminate UI and GLFW
	UIManager::terminate();
	glfwTerminate();
}