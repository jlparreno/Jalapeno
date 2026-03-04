#pragma once

#include "Framebuffer.h"

#include <spdlog/spdlog.h>

#include <string>
#include <unordered_map>
#include <memory>

/**
 * @class FramebufferManager
 * 
 * @brief Singleton class for managing framebuffer objects
 *
 * This class provides a global registry for named framebuffer objects (FBOs).
 * It handles creating, retrieving, resizing and destroying framebuffers,
 * ensuring proper ownership via std::unique_ptr and preventing resource leaks.
 */
class FramebufferManager
{
public:

    /**
     * @brief Retrieves the singleton instance of the FramebufferManager
     *
     * @return Reference to the unique FramebufferManager instance
     */
    static FramebufferManager& instance()
    {
        static FramebufferManager instance;
        return instance;
    }

    /**
     * @brief Deleted copy constructor to prevent copying the singleton
     */
    FramebufferManager(const FramebufferManager&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying the singleton
     */
    FramebufferManager& operator=(const FramebufferManager&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving the singleton
     */
    FramebufferManager(const FramebufferManager&&) = delete;

    /**
     * @brief Deleted move assignment operator to prevent moving the singleton
     */
    FramebufferManager& operator=(FramebufferManager&&) = delete;

    /**
     * @brief Creates a framebuffer with a unique name.
     *
     * If a framebuffer with the same name already exists, it will be replaced.
     *
     * @param name Unique identifier for the framebuffer.
     * @param width Width in pixels.
     * @param height Height in pixels.
     * @param samples Num of samples of the framebuffer (MSAA)
     * 
     * @return Pointer to the created framebuffer.
     */
    Framebuffer* create_framebuffer(const std::string& name, int width, int height, int samples = 1);

    /**
     * @brief Retrieves a framebuffer by name.
     * 
     * @param name Name of the framebuffer.
     * 
     * @return Pointer to the framebuffer, or nullptr if not found.
     */
    Framebuffer* get_framebuffer(const std::string& name);

    /**
     * @brief Removes a framebuffer from the registry.
     *
     * Resources are automatically freed thanks to std::unique_ptr.
     *
     * @param name Name of the framebuffer to remove.
     */
    void remove(const std::string& name);

    /**
     * @brief Resizes all registered framebuffers.
     *
     * Typically called when the window or viewport changes resolution.
     *
     * @param width New width in pixels.
     * @param height New height in pixels.
     */
    void resize_all(int width, int height);

    /**
     * @brief Destroys all framebuffers and clears the registry.
     */
    void clear();

private:

    // Private constructor and destructor to enforce singleton pattern
    FramebufferManager() = default;
    ~FramebufferManager() = default;

    // Internal map storing all managed framebuffers
    std::unordered_map<std::string, std::unique_ptr<Framebuffer>> m_framebuffers;
};
