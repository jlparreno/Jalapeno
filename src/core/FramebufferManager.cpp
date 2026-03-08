#include "FramebufferManager.h"

Framebuffer* FramebufferManager::create_framebuffer(const std::string& name, int width, int height, int samples)
{
    // Check if framebuffer already exists
    auto it = m_framebuffers.find(name);
    if (it != m_framebuffers.end())
    {
        SPDLOG_WARN("Framebuffer {} already exists. Recreating it.", name);
    }

    // Create or overwrite the framebuffer
    auto fbo = std::make_unique<Framebuffer>(width, height, samples);

    // Move new framebuffer to the manager array
    m_framebuffers[name] = std::move(fbo);

    // Return the pointer to the new fbo
    return m_framebuffers[name].get();
}

Framebuffer* FramebufferManager::get_framebuffer(const std::string& name)
{
    //Look for the fbo in the manager map
    auto it = m_framebuffers.find(name);

    //If it exists, return it. Return nullptr otherwise
    if (it == m_framebuffers.end())
    {
        SPDLOG_ERROR("Framebuffer {} not found.", name);
        return nullptr;
    }

    return it->second.get();
}

void FramebufferManager::remove(const std::string& name)
{
    //Look for the fbo in the manager map and remove it
    auto it = m_framebuffers.find(name);
    if (it != m_framebuffers.end())
    {
        m_framebuffers.erase(it);
        SPDLOG_INFO("Framebuffer {} removed.", name);
    }
}

void FramebufferManager::resize_all(int width, int height)
{
    // Loop all the framebuffers in manager, resizing them
    for (const auto& [name, fbo] : m_framebuffers)
    {
        if (fbo) 
            fbo->resize(width, height);
    }
}

void FramebufferManager::clear()
{
    m_framebuffers.clear();
}