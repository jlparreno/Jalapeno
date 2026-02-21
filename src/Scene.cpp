#include "Scene.h"

Scene::Scene(const std::string& name) : m_name(name)
{
	// Add main camera (at the moment we only have this one)
	auto camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
	m_cameras["main"] = std::move(camera);
	m_active_camera = m_cameras["main"].get();
}

Model* Scene::add_model(const std::string& name, const std::string& path)
{
	auto model = std::make_unique<Model>(name, path);
	m_models[model->get_name()] = std::move(model);

	return m_models[name].get();
}

Light* Scene::add_light(const std::string& name)
{
	auto light = std::make_unique<Light>(name);
	m_lights[light->get_name()] = std::move(light);

	return m_lights[name].get();
}

std::vector<const Model*> Scene::get_scene_models() const
{
	std::vector<const Model*> result;
	result.reserve(m_models.size());

	for (const auto& [name, model] : m_models)
	{
		result.push_back(model.get());
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

Model* Scene::get_model(const std::string& name)
{
	if (m_models.find(name) == m_models.end())
	{
		SPDLOG_ERROR("Model {} does not exist");
		return nullptr;
	}

	return m_models[name].get();
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
