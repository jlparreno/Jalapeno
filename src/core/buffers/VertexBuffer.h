#pragma once

#include <glad/glad.h>
#include "types.h"

#include <vector>

/**
 * @class VertexBuffer
 *
 * @brief Wrapper for OpenGL vertex geometry buffers (VAO, VBO, EBO)
 *
 * Uploads vertex and index data to the GPU at construction and exposes a
 * draw() method for indexed rendering. CPU-side data is not retained after
 * upload — the caller (Mesh) is responsible for keeping it if needed.
 */
class VertexBuffer
{
public:

    /**
     * @brief Uploads vertex and index data to the GPU
     *
     * @param vertices Vertex data to upload
     * @param indices  Index data defining triangle connectivity
     */
    VertexBuffer(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    /**
     * @brief Releases VAO, VBO and EBO
     */
    ~VertexBuffer();

    // Copying is disabled, a copy would share the same handles causing a double-free when an instance is destroyed
    VertexBuffer(const VertexBuffer&)            = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    /**
     * @brief Issues an indexed draw call for this geometry
     *
     * Binds the VAO and calls glDrawElements. The shader must already be bound.
     */
    void draw() const;

private:

    /**
     * @brief Creates and populates VAO, VBO and EBO, and configures vertex attribute pointers
     */
    void setup_buffers(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    // OpenGL handle to the Vertex Array Object
    GLuint m_VAO { 0 };

    // OpenGL handle to the Vertex Buffer Object
    GLuint m_VBO { 0 };

    // OpenGL handle to the Element Buffer Object
    GLuint m_EBO { 0 };

    // Number of indices, stored at upload time for use in glDrawElements
    unsigned int m_index_count { 0 };
};
