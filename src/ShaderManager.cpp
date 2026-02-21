#include "ShaderManager.h"

ShaderProgram* ShaderManager::load_program(const std::string& name, const std::vector<std::string>& shaders)
{
    // If it exists, return the previously loaded program
    if (exists(name)) 
    {
        return m_programs[name].get();
    }

    // Otherwise, generate the new program, add the shaders and link it
    auto program = std::make_unique<ShaderProgram>();

    for (auto& shader : shaders)
    {
        program->add_shader(ShaderProgram::get_type_from_path(shader), shader);
    }

    // Link program and check if everything is fine. Return nullptr if not
    if (!program->link_program())
    {
        SPDLOG_ERROR("Failed to link program: {}", name);
        return nullptr;
    }

    // Move new program to the manager array
    m_programs[name] = std::move(program);

    SPDLOG_INFO("Added new shader: {}", name);

    // Return the pointer to the new program
    return m_programs[name].get();
}

ShaderProgram* ShaderManager::get_program(const std::string& name) 
{
    //Look for the program in the manager map
    auto it = m_programs.find(name);
    
    //If it exists, return it. Return nullptr otherwise
    if (it != m_programs.end()) 
    {
        return it->second.get();
    }

    return nullptr;
}