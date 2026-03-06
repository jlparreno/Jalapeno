#include "ShaderProgram.h"

void ShaderProgram::add_shader(ShaderType type, const std::string& path) 
{
    std::string source = load_file(type, path);
    unsigned int shaderID = compile_stage(type, source);

    // Store the id for later linking
    m_program_stages[type] = shaderID;
}

std::string ShaderProgram::load_file(ShaderType type, const std::string& path)
{
    // Check if the file exists in the disk
    if (!std::filesystem::exists(path)) 
    {
        SPDLOG_ERROR("Shader file not found: {}", path);
        return "";
    }

    // Open the file
    std::ifstream file(path);
    if (!file.is_open()) 
    {
        SPDLOG_ERROR("Failed to open shader file: {}", path);
        return "";
    }

    // Get and return the contents of the file
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint ShaderProgram::compile_stage(ShaderType type, const std::string& source)
{
    // Vertex by default
    GLenum gl_type{ GL_VERTEX_SHADER };

    // Deduce the GLenum we need depending on the type
    switch (type) 
    {
    case ShaderType::Vertex:
        gl_type = GL_VERTEX_SHADER;
        break;
    case ShaderType::Fragment:
        gl_type = GL_FRAGMENT_SHADER;
        break;
    case ShaderType::Geometry:
        gl_type = GL_GEOMETRY_SHADER;
        break;
    case ShaderType::TessControl:
        gl_type = GL_TESS_CONTROL_SHADER;
        break;
    case ShaderType::TessEvaluation:
        gl_type = GL_TESS_EVALUATION_SHADER;
        break;
    case ShaderType::Compute:
        gl_type = GL_COMPUTE_SHADER;
        break;
    }

    // Create the shader and compile it
    GLuint shader = glCreateShader(gl_type);
    const char* src = source.c_str();

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for errors
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, log);
        SPDLOG_ERROR("Shader compilation error: {}", log);
        return 0;
    }

    // Return new shader id
    return shader;
}

bool ShaderProgram::link_program() 
{
    // Generate the program ID
    m_id = glCreateProgram();

    // Attach each previously compiled shader stage
    for (auto& [type, shaderID] : m_program_stages)
    {
        glAttachShader(m_id, shaderID);
    }

    // Link program and check for errors
    glLinkProgram(m_id);

    int success;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);

    if (!success) 
    {
        char log[1024];
        glGetProgramInfoLog(m_id, 1024, nullptr, log);
        SPDLOG_ERROR("Shader linkage error: {}", log);
        return false;
    }

    // Once linked, stage shaders can be deleted
    for (auto& [type, shaderID] : m_program_stages)
        glDeleteShader(shaderID);

    m_program_stages.clear();
    return true;
}

//void ShaderProgram::apply_material(const Material& material)
//{
//    switch (material.get_type())
//    {
//    case MaterialType::Phong:
//        material.upload_uniforms(this);
//        break;
//    /*default:
//        upload_generic_uniforms(material);
//        break;*/
//    }
//}

void ShaderProgram::set_uniform(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void ShaderProgram::set_uniform(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::set_uniform(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::set_uniform(const std::string& name, glm::vec3 value) const
{
    glUniform3f(glGetUniformLocation(m_id, name.c_str()), value.x, value.y, value.z);
}

void ShaderProgram::set_uniform(const std::string& name, glm::mat4 value, bool transpose) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::bind() const 
{ 
    glUseProgram(m_id);
}

void ShaderProgram::unbind() const 
{ 
    glUseProgram(0); 
}

ShaderType ShaderProgram::get_type_from_path(const std::string& path)
{
    // Get extension of the file usinf std::filesystem (gets the extension with the dot)
    std::filesystem::path fs_path(path);
    std::string extension = fs_path.extension().string();

    // Look for the corresponding type in our map
    auto it = s_extension_to_type.find(extension);
    if (it != s_extension_to_type.end())
    {
        return it->second;
    }

    //If the extension is unknown, notify it and return Vertex type by default
    SPDLOG_ERROR("Unknown shader extension: {}, in file {}", extension, path);
    return ShaderType::Vertex;
}


