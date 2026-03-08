#pragma once

#include "Model.h"
#include "Sphere.h"
#include "Camera.h"
#include "PointLight.h"
#include "DirectionalLight.h"

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
	 * @brief Constructs a new scene with the given name
	 *
	 * @param name Identifier for the scene
	 */
	Scene(const std::string& name);

	/**
	 * @brief Adds an assimp model to the scene by loading it from disk
	 *
	 * The method creates a new Model instance, stores it internally,
	 * and returns a raw pointer for direct manipulation.
	 *
	 * @param name Name used as a unique key inside the scene
	 * @param path Filesystem path to the model to load
	 * 
	 * @return Pointer to the created Renderable object
	 */
	Model* add_model(const std::string& name, const std::string& path);

	/**
	 * @brief Adds a generic sphere to the scene by loading it from disk
	 *
	 * The method creates a new Sphere instance, stores it internally,
	 * and returns a raw pointer for direct manipulation.
	 *
	 * @param name Name used as a unique key inside the scene
	 *
	 * @return Pointer to the created Renderable object
	 */
	Sphere* add_sphere(const std::string& name);

	/**
	 * @brief Creates and adds a point light to the scene
	 *
	 * @param name      Identifier name used to reference this light
	 * @param position  World-space position of the light
	 * @param color     RGB color of the light
	 * @param intensity Intensity multiplier applied to the light output
	 * 
	 * @return          Raw pointer to the newly created PointLight
	 */
	PointLight* add_point_light(const std::string& name,
								const glm::vec3& position, 
								const glm::vec3& color = glm::vec3(1.0f), 
								float intensity = 1.0f);

	/**
	 * @brief Creates and adds a directional light to the scene
	 *
	 * @param name      Identifier name used to reference this light
	 * @param direction World-space direction the light rays travel towards
	 * @param color     RGB color of the light
	 * @param intensity Intensity multiplier applied to the light output
	 * 
	 * @return          Raw pointer to the newly created DirectionalLight
	 */
	DirectionalLight* add_directional_light(const std::string& name, 
											const glm::vec3& direction, 
											const glm::vec3& color = glm::vec3(1.0f), 
											float intensity = 1.0f);

	/**
	 * @brief Retrieves a vector containing raw pointers to all renderables in the scene
	 *
	 * @return std::vector of const Renderable* containing all renderables in the scene.
	 */
	std::vector<const Renderable*>	get_scene_renderables() const;

	/**
	 * @brief Retrieves all cameras stored in the scene
	 *
	 * @return std::vector of const Camera* containing all scene cameras
	 */
	std::vector<const Camera*>		get_scene_cameras() const;

	/**
	 * @brief Retrieves all lights in the scene.
	 *
	 * @return std::vector of Light pointers
	 */
	const std::vector<std::unique_ptr<Light>>& get_scene_lights() const { return m_lights; }

	/**
	 * @brief Gets a renderable in the scene by name
	 *
	 * @param name Unique key of the renderable
	 * 
	 * @return Pointer to the Renderable, or nullptr if not found
	 */
	Renderable*		get_renderable(const std::string& name);

	/**
	 * @brief Gets a camera in the scene by name
	 *
	 * @param name Unique key of the camera
	 * 
	 * @return Pointer to the Camera, or nullptr if not found
	 */
	Camera*			get_camera(const std::string& name);

	/**
	 * @brief Returns the scene name
	 * 
	 * @return Name of the scene
	 */
	std::string		get_name() const { return m_name; }

	/**
	 * @brief Returns the currently active camera used for rendering
	 * 
	 * @return Pointer to the active Camera
	 */
	Camera*			get_active_camera() const { return m_active_camera; }

private:

	// Name of the scene
	std::string m_name;

	// Camera currently used for rendering
	Camera* m_active_camera;

	// All models, cameras and lights stored and owned by the scene.
	std::unordered_map<std::string, std::unique_ptr<Renderable>>	m_renderables;
	std::unordered_map<std::string, std::unique_ptr<Camera>>		m_cameras;

	std::vector<std::unique_ptr<Light>> m_lights;
};
