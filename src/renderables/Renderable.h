#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ShaderProgram.h"
#include "Mesh.h"

/**
 * @class Renderable
 *
 * @brief Abstract base class for any object that can be rendered in the scene.
 *
 * Renderable defines the common interface and transform properties shared by
 * all renderable objects, regardless of whether they are imported assets or
 * procedurally generated primitives.
 *
 * Derived classes must implement draw() and get_meshes() to define how their
 * geometry is submitted to the GPU. The model matrix is computed from the
 * position, rotation and scale properties and can be overridden if needed.
 *
 * Renderable is not intended to be instantiated directly. Use derived classes instead.
 */
class Renderable 
{
public:

    /**
     * @brief Default constructor
     *
     * Initializes the renderable at the world origin with unit scale
     * and zero rotation.
     */
    Renderable() = default;

    /**
     * @brief Virtual destructor.
     */
    virtual ~Renderable() = default;

    /**
     * @brief Draws all meshes of this renderable using the given shader
     *
     * @param shader Pointer to the ShaderProgram to use for rendering
     *
     * Derived classes must implement this method to submit their geometry
     * to the GPU. The shader is assumed to be already bound before this call.
     */
    virtual void draw(ShaderProgram* shader) const = 0;

    /**
     * @brief Returns all meshes that compose this renderable
     *
     * @return Const reference to the vector of meshes owned by this renderable
     */
    virtual const std::vector<Mesh>& get_meshes() const = 0;

    /**
     * @brief Sets a new position for the model
     *
     * @param pos New position of the model
     */
    void set_position(const glm::vec3& pos) { m_position = pos; }

    /**
     * @brief Sets new rotation values for the model
     *
     * @param rot New rotation values, euler angles, for each axis
     */
    void set_rotation(const glm::vec3& rot) { m_rotation = rot; }

    /**
     * @brief Sets a new scale value for the model
     *
     * @param scale New scale values, per axis
     */
    void set_scale(const glm::vec3& scale) { m_scale = scale; }

    /**
     * @brief Gets the model matrix of the model using the transforms configured
     *
     * @return Transform matrix for this model
     */
    virtual glm::mat4 get_model_matrix() const;

    /**
      * @brief Returns the world-space position of this renderable
      *
      * @return Const reference to the position vector
      */
    const glm::vec3& get_position() const { return m_position; }

    /**
     * @brief Returns the scale of this renderable
     *
     * @return Const reference to the scale vector
     */
    const glm::vec3& get_scale() const { return m_scale; }

    /**
     * @brief Returns the rotation of this renderable as Euler angles
     *
     * @return Const reference to the rotation vector in degrees
     */
    const glm::vec3& get_rotation() const { return m_rotation; }

protected:

    // World-space position of this renderable
    glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };

    // Scale factors applied per axis
    glm::vec3 m_scale{ 1.0f, 1.0f, 1.0f };

    // Rotation angles in degrees for each axis (pitch, yaw, roll)
    glm::vec3 m_rotation{ 0.0f, 0.0f, 0.0f };
};