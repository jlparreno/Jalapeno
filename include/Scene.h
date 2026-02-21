#pragma once

#include "Model.h"
#include "Camera.h"
#include "Light.h"

#include <string>
#include <unordered_map>

/**
 * @class Scene
 * 
 * @brief Container for all scene objects such as models, lights, and cameras.
 *
 * The Scene class represents a self-contained collection of renderable and interactive
 * elements in the engine.
 *
 * The Scene is also responsible for tracking the active camera used during rendering.
 * Objects are stored using unique pointers to guarantee ownership and automatic lifetime
 * management. Accessors provide both direct lookups by name and lists of raw pointers for
 * iteration by the renderer.
 */
class Scene 
{
public:

	/**
	 * @brief Constructs a new scene with the given name.
	 *
	 * @param name Identifier for the scene.
	 */
	Scene(const std::string& name);

	/**
	 * @brief Adds a model to the scene by loading it from disk.
	 *
	 * The method creates a new Model instance, stores it internally,
	 * and returns a raw pointer for direct manipulation.
	 *
	 * @param name Name used as a unique key inside the scene.
	 * @param path Filesystem path to the model to load.
	 * 
	 * @return Pointer to the created Model object.
	 */
	Model* add_model(const std::string& name, const std::string& path);

	/**
	 * @brief Adds a new light to the scene.
	 *
	 * The light is created with default parameters unless configured later.
	 *
	 * @param name Unique identifier for the new light.
	 * 
	 * @return Pointer to the created Light object.
	 */
	Light* add_light(const std::string& name);

	/**
	 * @brief Retrieves a vector containing raw pointers to all models in the scene.
	 *
	 * Useful for render passes or editor views that need to iterate over objects.
	 *
	 * @return std::vector of const Model* containing all models in the scene.
	 */
	std::vector<const Model*>	get_scene_models() const;

	/**
	 * @brief Retrieves all cameras stored in the scene.
	 *
	 * @return std::vector of const Camera* containing all scene cameras.
	 */
	std::vector<const Camera*>	get_scene_cameras() const;

	/**
	 * @brief Retrieves all lights in the scene.
	 *
	 * @return std::vector of const Light* containing all lights.
	 */
	std::vector<const Light*>	get_scene_lights() const;

	/**
	 * @brief Gets a model in the scene by name.
	 *
	 * @param name Unique key of the model.
	 * 
	 * @return Pointer to the Model, or nullptr if not found.
	 */
	Model*			get_model(const std::string& name);

	/**
	 * @brief Gets a camera in the scene by name.
	 *
	 * @param name Unique key of the camera.
	 * 
	 * @return Pointer to the Camera, or nullptr if not found.
	 */
	Camera*			get_camera(const std::string& name);

	/**
	 * @brief Gets a light in the scene by name.
	 *
	 * @param name Unique key of the light.
	 * 
	 * @return Pointer to the Light, or nullptr if not found.
	 */
	Light*			get_light(const std::string& name);

	/**
	 * @brief Returns the scene name.
	 * 
	 * @return Name of the scene.
	 */
	std::string		get_name() const { return m_name; }

	/**
	 * @brief Returns the currently active camera used for rendering.
	 * 
	 * @return Pointer to the active Camera.
	 */
	Camera*			get_active_camera() const { return m_active_camera; }

private:

	// Name of the scene
	std::string m_name;

	// Camera currently used for rendering
	Camera* m_active_camera;

	// All models, cameras and lights stored and owned by the scene.
	std::unordered_map<std::string, std::unique_ptr<Model>>		m_models;
	std::unordered_map<std::string, std::unique_ptr<Camera>>	m_cameras;
	std::unordered_map<std::string, std::unique_ptr<Light>>		m_lights;
};
