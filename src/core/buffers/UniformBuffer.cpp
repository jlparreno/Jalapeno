#include "UniformBuffer.h"

#include <spdlog/spdlog.h>

UniformBuffer::UniformBuffer(size_t size, int binding_point) : m_binding_point(binding_point), m_size(size)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
    glBufferData(GL_UNIFORM_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, m_id);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBuffer::~UniformBuffer()
{
    if (m_id) glDeleteBuffers(1, &m_id);
}

void UniformBuffer::upload(const void* data, size_t size)
{
    if (size > m_size)
    {
        SPDLOG_ERROR("UniformBuffer upload size ({}) exceeds buffer size ({})", size, m_size);
        return;
    }

    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, static_cast<GLsizeiptr>(size), data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
