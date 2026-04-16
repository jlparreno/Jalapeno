#pragma once

#include <glad/glad.h>
#include <cstddef>

/**
 * @class UniformBuffer
 *
 * @brief Wrapper for an OpenGL Uniform Buffer Object (UBO)
 *
 * Allocates a fixed-size GPU buffer at construction, binds it permanently to
 * a binding point, and exposes upload() to stream CPU data.
 */
class UniformBuffer
{
public:

    /**
     * @brief Allocates the UBO and binds it to the given binding point
     *
     * @param size          Size of the buffer in bytes
     * @param binding_point GLSL binding point (must match layout(binding = N) in shader)
     */
    UniformBuffer(size_t size, int binding_point);

    /**
     * @brief Releases the GPU buffer
     */
    ~UniformBuffer();

    /**
     * @brief Streams CPU data into the buffer
     *
     * @param data Pointer to the data to upload
     * @param size Number of bytes to upload (must be <= buffer size)
     */
    void upload(const void* data, size_t size);

    /**
     * @brief Returns the OpenGL buffer ID
     */
    GLuint get_id() const { return m_id; }

    /**
     * @brief Returns the binding point this buffer is permanently bound to
     */
    int get_binding_point() const { return m_binding_point; }

private:

    // OpenGL handle to the buffer object
    GLuint m_id{ 0 };

    // GLSL binding point this buffer is permanently bound to via glBindBufferBase
    int m_binding_point{ 0 };

    // Size of the buffer in bytes, used to validate uploads
    size_t m_size{ 0 };
};
