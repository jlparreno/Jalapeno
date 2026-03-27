#pragma once

#include "types.h"

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

/**
 * @class ShaderProgram
 * @brief Encapsulates an OpenGL shader program composed of multiple shader stages
 *
 * This class allows adding multiple shader stages, compiling them, linking them into
 * a single OpenGL program, setting uniforms, and binding/unbinding for rendering
 */
class ShaderProgram
{
public:

    ShaderProgram() = default;

    /**
     * @brief Adds a shader stage to the program from a GLSL file
     *
     * The shader is stored internally and will be compiled and linked later
     *
     * @param type Type of the shader stage (Vertex, Fragment, Geometry, etc.)
     * @param path File path to the GLSL shader source
     */
    void add_shader(ShaderType type, const std::string& path);

    /**
     * @brief Compiles a shader stage from source code
     *
     * @param type Type of the shader stage
     * @param source GLSL source code of the shader
     * @return The OpenGL shader ID assigned to this stage
     */
    GLuint compile_stage(ShaderType type, const std::string& source);

    /**
     * @brief Links all added shader stages into a single OpenGL program
     *
     * After linking, the program can be bound for rendering. 
     * If linking fails,check OpenGL logs for errors
     *
     * @return true if linking succeeded, false otherwise.
     */
    bool link_program();

    /**
     * @brief Sets a boolean uniform variable in the shader
     *
     * @param name Name of the uniform variable
     * @param value Boolean value to set
     */
    void set_uniform(const std::string& name, bool value) const;

    /**
     * @brief Sets an integer uniform variable in the shader
     *
     * @param name Name of the uniform variable
     * @param value Integer value to set
     */
    void set_uniform(const std::string& name, int value) const;

    /**
     * @brief Sets a float uniform variable in the shader
     *
     * @param name Name of the uniform variable
     * @param value Float value to set
     */
    void set_uniform(const std::string& name, float value) const;

    /**
     * @brief Sets a vec3 uniform variable in the shader
     *
     * @param name Name of the uniform variable
     * @param value Vector value to set
     */
    void set_uniform(const std::string& name, glm::vec3 value) const;

    /**
     * @brief Sets a matrix 3x3 uniform variable in the shader
     *
     * @param name Name of the uniform variable
     * @param value Matrix value to set
     * @param transpose Optional value to transpose the matrix. False by default as GLM uses colum-major ordering.
     */
    void set_uniform(const std::string& name, glm::mat3 value, bool transpose = false) const;

    /**
     * @brief Sets a matrix 4x4 uniform variable in the shader
     *
     * @param name Name of the uniform variable
     * @param value Matrix value to set
     * @param transpose Optional value to transpose the matrix. False by default as GLM uses colum-major ordering.
     */
    void set_uniform(const std::string& name, glm::mat4 value, bool transpose = false) const;

    /**
     * @brief Binds the shader program for use in OpenGL rendering
     */
    void bind() const;

    /**
     * @brief Unbinds the shader program
     */
    void unbind() const;

    /**
     * @brief Returns the OpenGL program ID
     *
     * @return GLuint representing the OpenGL handle of this shader program
     */
    unsigned int get_id() const { return m_id; }

    /**
     * @brief Deduces the ShaderType from a file path based on its extension
     *
     * Supported extensions: .vert, .frag, .geom, .tesc, .tese, .comp
     *
     * @param path File path of the shader source
     * @return ShaderType corresponding to the file extension. Vertex type by default
     */
    static ShaderType get_type_from_path(const std::string& path);

private:

    /**
     * @brief Loads the contents of a shader source file into a string
     *
     * @param type Shader type, to select corresponding common includes
     * @param path Path to the shader file
     * 
     * @return std::string containing the GLSL source code
     */
    std::string load_file(ShaderType type, const std::string& path);


    /**
     * @brief Returns the location of a uniform variable, caching the result.
     *
     * On first call for a given name, queries OpenGL and stores the result.
     * Subsequent calls return the cached value directly.
     *
     * @param name Name of the uniform variable
     * @return GLint location, or -1 if not found
     */
    GLint get_uniform_location(const std::string& name) const;

    // OpenGL handle of the linked shader program
    unsigned int m_id{ 0 };

    // Stores the individual shader IDs by stage type for linking
    std::unordered_map<ShaderType, unsigned int> m_program_stages;

    // Cache of uniform name → location to avoid repeated glGetUniformLocation calls
    mutable std::unordered_map<std::string, GLint> m_uniform_cache;
    
    // Map from extension to OpenGL shader type
    inline static const std::unordered_map<std::string, ShaderType> s_extension_to_type
    {
        {".vert", ShaderType::Vertex},
        {".frag", ShaderType::Fragment},
        {".geom", ShaderType::Geometry},
        {".tesc", ShaderType::TessControl},
        {".tese", ShaderType::TessEvaluation},
        {".comp", ShaderType::Compute}
    };
};

