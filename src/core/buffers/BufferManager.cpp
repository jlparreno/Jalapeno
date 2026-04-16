#include "BufferManager.h"

UniformBuffer* BufferManager::create_uniform_buffer(const std::string& name, size_t size, int binding_point)
{
    auto it = m_uniform_buffers.find(name);
    if (it != m_uniform_buffers.end())
    {
        SPDLOG_WARN("UniformBuffer '{}' already exists. Recreating it.", name);
    }

    m_uniform_buffers[name] = std::make_unique<UniformBuffer>(size, binding_point);
    return m_uniform_buffers[name].get();
}

UniformBuffer* BufferManager::get_uniform_buffer(const std::string& name)
{
    auto it = m_uniform_buffers.find(name);
    if (it == m_uniform_buffers.end())
    {
        SPDLOG_ERROR("UniformBuffer '{}' not found.", name);
        return nullptr;
    }

    return it->second.get();
}

void BufferManager::remove(const std::string& name)
{
    auto it = m_uniform_buffers.find(name);
    if (it != m_uniform_buffers.end())
    {
        m_uniform_buffers.erase(it);
        SPDLOG_INFO("UniformBuffer '{}' removed.", name);
    }
}

void BufferManager::clear()
{
    m_uniform_buffers.clear();
}
