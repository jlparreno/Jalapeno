#pragma once

#include <spdlog/spdlog.h>

#include "ShaderProgram.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

/**
 * @class ShaderManager
 * 
 * @brief Singleton class for managing shader program objects
 *
 * This class handles loading, storing, and retrieving shader programs by name.
 * It ensures that each program is loaded only once
 */
class ShaderManager 
{
public:

    /**
     * @brief Retrieves the singleton instance of the ShaderManager
     *
     * @return Reference to the unique ShaderManager instance
     */
    static ShaderManager& instance() 
    {
        static ShaderManager instance;
        return instance;
    }

    /**
     * @brief Deleted copy constructor to prevent copying the singleton
     */
    ShaderManager(const ShaderManager&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying the singleton
     */
    ShaderManager& operator=(const ShaderManager&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving the singleton
     */
    ShaderManager(ShaderManager&&) = delete;

    /**
     * @brief Deleted move assignment operator to prevent moving the singleton
     */
    ShaderManager& operator=(ShaderManager&&) = delete;

    /**
     * @brief Loads a shader program from vertex and fragment shader files
     *
     * If a program with the given name already exists, it returns the existing program.
     * Otherwise, it compiles and links the shaders into a new ShaderProgram.
     *
     * @param name Unique name to identify the shader program
     * @param vertexPath Path to the vertex shader GLSL file
     * @param fragmentPath Path to the fragment shader GLSL file
     * @return Reference to the loaded or existing ShaderProgram. Returns nullptr if loading failed.
     */
    ShaderProgram* load_program(const std::string& name, const std::vector<std::string>& shaders);

    /**
     * @brief Retrieves a shader program by its unique name
     *
     * @param name Name of the shader program to retrieve
     * @return Pointer to the ShaderProgram if it exists, nullptr otherwise
     */
    ShaderProgram* get_program(const std::string& name);

    /**
     * @brief Checks if a shader program with the given name exists
     *
     * @param name Name of the shader program to check
     * @return true if the shader program exists, false otherwise
     */
    bool exists(const std::string& name) const 
    {
        return m_programs.find(name) != m_programs.end();
    }

private:

    // Private constructor and destructor to enforce singleton pattern
    ShaderManager() = default;
    ~ShaderManager() = default;

    // Map storing shaders by their unique name
    // Uses unique_ptr to ensure proper memory management
    std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> m_programs;
};

