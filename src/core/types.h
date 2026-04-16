#pragma once

#include "config.h"

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
 * @brief GPU-side point light data matching the std140 layout in lights.glsl
 *
 * All fields are vec4 to avoid std140 padding surprises with vec3.
 */
struct GPUPointLight
{
    glm::vec4 position;     // xyz = position, w = unused
    glm::vec4 color;        // xyz = color, w = intensity
    glm::vec4 attenuation;  // x = constant, y = linear, z = quadratic, w = unused
    glm::vec4 shadow;       // x = shadows_enabled (0/1), y = far_plane, zw = unused
};

/**
 * @brief GPU-side directional light data matching the std140 layout in lights.glsl
 */
struct GPUDirectionalLight
{
    glm::vec4 direction;    // xyz = direction, w = unused
    glm::vec4 color;        // xyz = color, w = intensity
    glm::vec4 shadow;       // x = shadows_enabled (0/1), yzw = unused
    glm::mat4 light_matrix;
};

/**
 * @brief Full UBO block for point lights
 * 
 * We need to add padding to make total size a multiple of 16 bytes. 
 * In std140 specification, the block size should be a multiple of the bigger type that it contains, vec4 in this case (16 bytes).
 */
struct PointLightBlock
{
    GPUPointLight  lights[MAX_POINT_LIGHTS];    // 4 * 64 = 256 bytes
    int  count{ 0 };                            // 4 bytes
    int  _pad[3];                               // 12 bytes padding → 272 bytes total (multiple of 16 bytes to match GPU size)
};

/**
 * @brief Full UBO block for directional lights, uploaded once per frame
 * 
 * We need to add padding to make total size a multiple of 16 bytes. 
 * In std140 specification, the block size should be a multiple of the bigger type that it contains, vec4 in this case (16 bytes).
 */
struct DirectionalLightBlock
{
    GPUDirectionalLight lights[MAX_DIRECTIONAL_LIGHTS]; // 2 * 112 = 224 bytes
    int count{ 0 };                                     // 4 bytes
    int _pad[3];                                        // 12 bytes padding → 240 bytes total (multiple of 16 bytes to match GPU size)
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
    bool resizable{ true };
    bool is_cubemap{ false };

    std::vector<AttachmentSpec> attachments;
};
