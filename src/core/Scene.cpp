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
	auto sphere = std::make_unique<Sphere>(name);

	Sphere* ptr = sphere.get();
	m_renderables[name] = std::move(sphere);

	SPDLOG_INFO("Added new sphere to the scene: {}", name);

	return ptr;
}

PointLight* Scene::add_point_light(const std::string& name, const glm::vec3& position, const glm::vec3& color, float intensity)
{
	auto light = std::make_unique<PointLight>(name, position, color, intensity);

	PointLight* ptr = light.get();
	m_lights.push_back(std::move(light));

	SPDLOG_INFO("Added new point light to the scene: {}", name);

	return ptr;
}

DirectionalLight* Scene::add_directional_light(const std::string& name, const glm::vec3& direction, const glm::vec3& color, float intensity)
{
	auto light = std::make_unique<DirectionalLight>(name, direction, color, intensity);

	DirectionalLight* ptr = light.get();
	m_lights.push_back(std::move(light));

	SPDLOG_INFO("Added new directional light to the scene: {}", name);

	return ptr;
}

std::vector<Renderable*> Scene::get_scene_renderables()
{
	std::vector<Renderable*> result;
	result.reserve(m_renderables.size());

	for (auto& [name, renderable] : m_renderables)
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
