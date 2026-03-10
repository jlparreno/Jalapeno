#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

/**
* @enum ShaderType
*
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

/**
 * @enum LightType
 *
 * @brief Represents the type of a light source in the scene
 */
enum class LightType
{
    Point,
    Directional
};

/**
 * @enum MaterialType
 *
 * @brief Represents the shading model used by a material
 */
enum class MaterialType
{
    Lambert,
    Phong,
    PBR
};

/**
 * @struct Vertex
 *
 * @brief Represents a single vertex with all its associated attributes
 *
 * This struct defines the per-vertex data layout used throughout the engine.
 * It is uploaded directly to the GPU via VBOs and its memory layout must
 * match the vertex attribute pointers configured in Mesh::setup_buffers().
 */
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;

    glm::vec3 tangent;
    glm::vec3 bitangent;
};

/**
 * @brief Describes a single attachment of a Framebuffer
 */
struct AttachmentSpec
{
    GLenum internal_format{ GL_RGBA16F };
    GLenum attachment_point{ GL_COLOR_ATTACHMENT0 };
};

/**
 * @brief Describes the full configuration of a Framebuffer
 */
struct FramebufferSpec
{
    int width{ 1920 };
    int height{ 1080 };
    int samples{ 1 };
    int resizable{ true };

    std::vector<AttachmentSpec> attachments;
};
