#pragma once

#include <glm/glm.hpp>
#include <string>

#define MAX_BONE_INFLUENCE 4

/**
* @enum ShaderType
* @brief Enum representing different types of GLSL shader stages
*/
enum class ShaderType
{
    Vertex,
    Fragment,
    Geometry,
    TessControl,
    TessEvaluation,
    Compute
};

enum class LightType
{
    Point,
    Directional
};

struct TextureInfo
{
    std::string name;
    std::string uniform_name;
    std::string file_path;
    std::string type_name;
};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;

    glm::vec3 tangent;
    glm::vec3 bitangent;

    // Bone indexes which will influence this vertex
    int m_bone_ids[MAX_BONE_INFLUENCE];

    // Weights from each bone
    float m_bone_weights[MAX_BONE_INFLUENCE];
};
