#include "Scene.h"

Scene::Scene(const std::string& name) : m_name(name)
{
	// Add main camera (at the moment we only have this one)
	auto camera = std::make_unique<Camera>(glm::vec3(-3.0f, 3.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, -10.0f);
	m_cameras["main"] = std::move(camera);
	m_active_camera = m_cameras["main"].get();
}

Model* Scene::add_model(const std::string& name, const std::string& path)
{
	auto model = std::make_unique<Model>(name, path);

	Model* ptr = model.get();
	m_renderables[model->get_name()] = std::move(model);

	SPDLOG_INFO("Added new model to the scene: {}", name);

	return ptr;
}

Sphere* Scene::add_sphere(const std::string& name)
{
	auto sphere = std::make_unique<Sphere>();

	Sphere* ptr = sphere.get();
	m_renderables[name] = std::move(sphere);

	SPDLOG_INFO("Added new sphere to the scene: {}", name);

	return ptr;
}

Light* Scene::add_light(const std::string& name)
{
	auto light = std::make_unique<Light>(name);
	m_lights[light->get_name()] = std::move(light);

	return m_lights[name].get();
}

std::vector<const Renderable*> Scene::get_scene_renderables() const
{
	std::vector<const Renderable*> result;
	result.reserve(m_renderables.size());

	for (const auto& [name, renderable] : m_renderables)
	{
		result.push_back(renderable.get());
	}

	return result;
}

std::vector<const Camera*> Scene::get_scene_cameras() const
{
	std::vector<const Camera*> result;
	result.reserve(m_cameras.size());

	for (const auto& [name, camera] : m_cameras)
	{
		result.push_back(camera.get());
	}

	return result;
}

std::vector<const Light*> Scene::get_scene_lights() const
{
	std::vector<const Light*> result;
	result.reserve(m_lights.size());

	for (const auto& [name, light] : m_lights)
	{
		result.push_back(light.get());
	}

	return result;
}

Renderable* Scene::get_renderable(const std::string& name)
{
	if (m_renderables.find(name) == m_renderables.end())
	{
		SPDLOG_ERROR("Model {} does not exist");
		return nullptr;
	}

	return m_renderables[name].get();
}

Camera* Scene::get_camera(const std::string& name)
{
	if (m_cameras.find(name) == m_cameras.end())
	{
		SPDLOG_ERROR("Camera {} does not exist");
		return nullptr;
	}

	return m_cameras[name].get();
}

Light* Scene::get_light(const std::string& name)
{
	if (m_lights.find(name) == m_lights.end())
	{
		SPDLOG_ERROR("Light {} does not exist");
		return nullptr;
	}

	return m_lights[name].get();
}
