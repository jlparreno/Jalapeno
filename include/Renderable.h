#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ShaderProgram.h"
#include "Mesh.h"

class Renderable 
{
public:

    virtual ~Renderable() = default;

    virtual void draw(ShaderProgram* shader) const = 0;

    virtual const std::vector<Mesh>& get_meshes() const = 0;

    /**
     * @brief Gets the model matrix of the model using the transforms configured.
     *
     * @return Transform matrix for this model.
     */
    virtual glm::mat4 get_model_matrix() const;

    /**
     * @brief Sets a new position for the model.
     *
     * @param pos New position of the model.
     */
    void set_position(const glm::vec3& pos) { m_position = pos; }

    /**
     * @brief Sets new rotation values for the model.
     *
     * @param rot New rotation values, euler angles, for each axis.
     */
    void set_rotation(const glm::vec3& rot) { m_rotation = rot; }

    /**
     * @brief Sets a new scale value for the model.
     *
     * @param scale New scale values, per axis
     */
    void set_scale(const glm::vec3& scale) { m_scale = scale; }

    const glm::vec3& get_position() const { return m_position; }
    const glm::vec3& get_scale()    const { return m_scale; }
    const glm::vec3& get_rotation() const { return m_rotation; }

protected:

    // Transforms
    glm::vec3 m_position { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_scale { 1.0f, 1.0f, 1.0f };
    glm::vec3 m_rotation { 0.0f, 0.0f, 0.0f };
};