#include "GeometryPass.h"
#include "BufferManager.h"

const PipelineState GeometryPass::k_state = PipelineState(); // all defaults

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

	// Apply OpenGL pipeline state
	k_state.apply();

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
	PointLightBlock       pl_block{};
	DirectionalLightBlock dl_block{};

	for (const auto& light : scene.get_scene_lights())
	{
		if (auto* pl = dynamic_cast<PointLight*>(light.get()))
		{
			// Ignore extra lights if any
			if (pl_block.count >= MAX_POINT_LIGHTS) continue; 

			GPUPointLight& gpu  = pl_block.lights[pl_block.count];
			gpu.position        = glm::vec4(pl->get_position(), 0.0f);
			gpu.color           = glm::vec4(pl->get_color(), pl->get_intensity());
			gpu.attenuation     = glm::vec4(pl->get_constant(), pl->get_linear(), pl->get_quadratic(), 0.0f);
			gpu.shadow          = glm::vec4(pl->get_shadows_enabled() ? 1.0f : 0.0f, pl->get_far_plane(), 0.0f, 0.0f);

			if (pl->get_shadows_enabled() && pl->get_shadow_fbo())
			{
				int unit = POINT_SHADOW_START + pl_block.count;
				pl->get_shadow_fbo()->bind_depth_as_texture(unit);
				shader->set_uniform("point_shadow_cubes[" + std::to_string(pl_block.count) + "]", unit);
			}

			pl_block.count++;
		}
		else if (auto* dl = dynamic_cast<DirectionalLight*>(light.get()))
		{
			// Ignore extra lights if any
			if (dl_block.count >= MAX_DIRECTIONAL_LIGHTS) continue;

			GPUDirectionalLight& gpu	= dl_block.lights[dl_block.count];
			gpu.direction           = glm::vec4(dl->get_direction(), 0.0f);
			gpu.color               = glm::vec4(dl->get_color(), dl->get_intensity());
			gpu.shadow              = glm::vec4(dl->get_shadows_enabled() ? 1.0f : 0.0f, 0.0f, 0.0f, 0.0f);
			gpu.light_matrix		= dl->get_light_space_matrix();

			if (dl->get_shadows_enabled() && dl->get_shadow_fbo())
			{
				int unit = DIRECTIONAL_SHADOW_START + dl_block.count;
				dl->get_shadow_fbo()->bind_depth_as_texture(unit);
				shader->set_uniform("directional_shadow_maps[" + std::to_string(dl_block.count) + "]", unit);
			}

			dl_block.count++;
		}
	}

	// Bind fallback textures for inactive shadow sampler slots
	for (int i = pl_block.count; i < MAX_POINT_LIGHTS; i++)
	{
		shader->set_uniform("point_shadow_cubes[" + std::to_string(i) + "]", INACTIVE_SHADOW_CUBE_UNIT);
	}
		
	for (int i = dl_block.count; i < MAX_DIRECTIONAL_LIGHTS; i++)
	{
		shader->set_uniform("directional_shadow_maps[" + std::to_string(i) + "]", INACTIVE_SHADOW_2D_UNIT);
	}

	glActiveTexture(GL_TEXTURE0 + INACTIVE_SHADOW_2D_UNIT);
	glBindTexture(GL_TEXTURE_2D, TextureManager::instance().get_white_texture()->get_id());

	glActiveTexture(GL_TEXTURE0 + INACTIVE_SHADOW_CUBE_UNIT);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureManager::instance().get_white_cubemap()->get_id());

	// Upload light data to GPU via UBOs
	BufferManager::instance().get_uniform_buffer("point_lights")->upload(&pl_block, sizeof(PointLightBlock));
	BufferManager::instance().get_uniform_buffer("directional_lights")->upload(&dl_block, sizeof(DirectionalLightBlock));
}
