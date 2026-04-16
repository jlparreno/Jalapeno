#pragma once

#include "UniformBuffer.h"

#include <spdlog/spdlog.h>

#include <string>
#include <unordered_map>
#include <memory>

/**
 * @class BufferManager
 *
 * @brief Singleton class for managing GPU buffer objects
 *
 * This class provides a global registry for GPU buffer objects.
 * It handles creating, retrieving, resizing and destroying different types of GPU buffers (UBOs only at the moment),
 * ensuring proper ownership via std::unique_ptr and preventing resource leaks.
 */
class BufferManager
{
public:

    /**
     * @brief Retrieves the singleton instance
     */
    static BufferManager& instance()
    {
        static BufferManager instance;
        return instance;
    }

    /**
     * @brief Deleted copy constructor to prevent copying the singleton
     */
    BufferManager(const BufferManager&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying the singleton
     */
    BufferManager& operator=(const BufferManager&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving the singleton
     */
    BufferManager(const BufferManager&&) = delete;

    /**
     * @brief Deleted move assignment operator to prevent moving the singleton
     */
    BufferManager& operator=(BufferManager&&) = delete;

    /**
     * @brief Creates and registers a UniformBuffer
     *
     * If a buffer with the same name already exists it will be replaced.
     *
     * @param name          Unique identifier for the buffer
     * @param size          Size of the buffer in bytes
     * @param binding_point GLSL binding point (must match layout(binding = N) in shader)
     *
     * @return Raw pointer to the created UniformBuffer
     */
    UniformBuffer* create_uniform_buffer(const std::string& name, size_t size, int binding_point);

    /**
     * @brief Retrieves a UniformBuffer by name
     *
     * @param name Name of the buffer to retrieve
     * 
     * @return Raw pointer to the buffer, or nullptr if not found
     */
    UniformBuffer* get_uniform_buffer(const std::string& name);

    /**
     * @brief Removes and destroys a buffer by name
     *
     * @param name Name of the buffer to remove
     */
    void remove(const std::string& name);

    /**
     * @brief Destroys all managed buffers and clears the registry
     */
    void clear();

private:

    // Private constructor and destructor to enforce singleton pattern
    BufferManager()  = default;
    ~BufferManager() = default;

    // Internal map storing all managed UBOs
    std::unordered_map<std::string, std::unique_ptr<UniformBuffer>> m_uniform_buffers;
};
