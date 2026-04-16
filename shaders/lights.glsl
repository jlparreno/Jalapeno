// lights.glsl — Shared UBO definitions for all shading models
//
// The C++ counterparts (GPUPointLight, GPUDirectionalLight, PointLightBlock, DirectionalLightBlock) 
// are defined in types.h and must stay in sync
//
// std140 layout:
//   - All vec3 fields are stored as vec4 (w = unused or repurposed)
//   - Struct sizes are multiples of 16 bytes

#define MAX_POINT_LIGHTS       4
#define MAX_DIRECTIONAL_LIGHTS 2

struct PointLightData
{
    vec4 position;      // xyz = position, w = unused
    vec4 color;         // xyz = color, w = intensity
    vec4 attenuation;   // x = constant, y = linear, z = quadratic, w = unused
    vec4 shadow;        // x = shadows_enabled (0/1), y = far_plane, zw = unused
};

struct DirectionalLightData
{
    vec4 direction;     // xyz = direction, w = unused
    vec4 color;         // xyz = color, w = intensity
    vec4 shadow;        // x = shadows_enabled (0/1), yzw = unused
    mat4 light_matrix;
};

layout(std140, binding = 0) uniform PointLightBlock
{
    PointLightData data[MAX_POINT_LIGHTS];
    int            count;
} point_lights;

layout(std140, binding = 1) uniform DirectionalLightBlock
{
    DirectionalLightData data[MAX_DIRECTIONAL_LIGHTS];
    int                  count;
} directional_lights;

// Shadow samplers
uniform samplerCube point_shadow_cubes[MAX_POINT_LIGHTS];
uniform sampler2D   directional_shadow_maps[MAX_DIRECTIONAL_LIGHTS];
