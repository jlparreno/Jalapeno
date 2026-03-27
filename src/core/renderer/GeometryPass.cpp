#include "GeometryPass.h"

void GeometryPass::execute(Scene& scene)
{
	// Forward rendering: Main Pass
	Framebuffer* fbo = FramebufferManager::instance().get_framebuffer("main");

	// Check valid FBO
	if (!fbo)
	{
		SPDLOG_ERROR("Invalid FBO for Geometry Pass");
		return;
	}

	fbo->bind();

	// Clear screen with out default clear color
	glClearColor(0.16f, 0.18f, 0.20f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Configure OpenGL state
	// TODO: Create OpenGL context manager class
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Setup matrices with scene camera
	Camera* camera = scene.get_active_camera();
	if (!camera)
	{
		SPDLOG_ERROR("No active camera in scene");
		fbo->unbind();
		return;
	}

	float aspect = glm::clamp((float)fbo->get_width() / (float)fbo->get_height(), MIN_ASPECT, MAX_ASPECT);

	glm::mat4 view_mat = camera->get_view_matrix();
	glm::mat4 projection_mat = glm::perspective(glm::radians(camera->get_fov()), aspect, 0.1f, 100.0f);

	// Iterate all scene objects calling each draw function
	for (const Renderable* renderable : scene.get_scene_renderables())
	{
		glm::mat4 model = renderable->get_model_matrix();
		glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model))); // Calculate in CPU and pass to the shader as uniform (cheaper)

		for (auto& mesh : renderable->get_meshes())
		{
			Material* material = mesh.get_material();
			if (!material)
			{
				SPDLOG_WARN("Mesh has no material, skipping draw");
				continue;
			}

			ShaderProgram* shader = material->get_shader();

			shader->bind();

			// Set scene transforms data
			shader->set_uniform("model", model);
			shader->set_uniform("view", view_mat);
			shader->set_uniform("projection", projection_mat);
			shader->set_uniform("normal_matrix", normal_matrix);
			shader->set_uniform("view_pos", camera->get_position());

			//Upload lights data to the shader
			upload_lights(scene, shader);

			// Draw the mesh
			mesh.draw(shader);

			shader->unbind();
		}
	}

	fbo->unbind();
}

void GeometryPass::upload_lights(Scene& scene, ShaderProgram* shader)
{
	int point_index = 0;
	int directional_index = 0;

	// Point lights are uploaded to the point_lights array and 
	// directional lights to the directional_lights array
	for (const auto& light : scene.get_scene_lights())
	{
		if (auto* pl = dynamic_cast<PointLight*>(light.get()))
		{
			std::string base = "point_lights[" + std::to_string(point_index) + "].";
			shader->set_uniform(base + "position", pl->get_position());
			shader->set_uniform(base + "color", pl->get_color());
			shader->set_uniform(base + "intensity", pl->get_intensity());
			shader->set_uniform(base + "constant", pl->get_constant());
			shader->set_uniform(base + "linear", pl->get_linear());
			shader->set_uniform(base + "quadratic", pl->get_quadratic());
			shader->set_uniform(base + "shadows_enabled", pl->get_shadows_enabled());

			//Shadow maps if any
			if (pl->get_shadows_enabled() && pl->get_shadow_fbo())
			{
				int shadow_unit = POINT_SHADOW_START + point_index;
				pl->get_shadow_fbo()->bind_depth_as_texture(shadow_unit);

				shader->set_uniform(base + "shadow_cube", shadow_unit);
				shader->set_uniform(base + "far_plane", pl->get_far_plane());
			}

			point_index++;
		}
		else if (auto* dl = dynamic_cast<DirectionalLight*>(light.get()))
		{
			std::string base = "directional_lights[" + std::to_string(directional_index) + "].";
			shader->set_uniform(base + "direction", dl->get_direction());
			shader->set_uniform(base + "color", dl->get_color());
			shader->set_uniform(base + "intensity", dl->get_intensity());
			shader->set_uniform(base + "shadows_enabled", dl->get_shadows_enabled());

			//Shadow maps if any
			if (dl->get_shadows_enabled() && dl->get_shadow_fbo())
			{
				int shadow_unit = DIRECTIONAL_SHADOW_START + directional_index;
				dl->get_shadow_fbo()->bind_depth_as_texture(shadow_unit);
				
				shader->set_uniform(base + "light_matrix", dl->get_light_space_matrix());
				shader->set_uniform(base + "shadow_tex", shadow_unit);
			}

			directional_index++;
		}
	}

	// Init unused slots to avoid default 0 for inactive samplers
	for (int i = directional_index; i < MAX_DIRECTIONAL_LIGHTS; i++)
	{
		std::string base = "directional_lights[" + std::to_string(i) + "].";
		shader->set_uniform(base + "shadows_enabled", false);
		shader->set_uniform(base + "shadow_tex", INACTIVE_SHADOW_2D_UNIT);
	}
	for (int i = point_index; i < MAX_POINT_LIGHTS; i++)
	{
		std::string base = "point_lights[" + std::to_string(i) + "].";
		shader->set_uniform(base + "shadows_enabled", false);
		shader->set_uniform(base + "shadow_cube", INACTIVE_SHADOW_CUBE_UNIT);
	}

	// Bind default white textures in inactive units
	glActiveTexture(GL_TEXTURE0 + INACTIVE_SHADOW_2D_UNIT);
	glBindTexture(GL_TEXTURE_2D, TextureManager::instance().get_white_texture()->get_id());

	glActiveTexture(GL_TEXTURE0 + INACTIVE_SHADOW_CUBE_UNIT);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureManager::instance().get_white_cubemap()->get_id());

	// Count uniforms
	shader->set_uniform("num_point_lights", point_index);
	shader->set_uniform("num_directional_lights", directional_index);
}
