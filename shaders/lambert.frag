#version 460 core

#include "lights.glsl"

const vec3 sample_offset_directions[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

struct Material 
{
    vec3 ambient_color;
    vec3 diffuse_color;
};
uniform Material material;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

// Textures
uniform sampler2D diffuse_tex;

out vec4 FragColor;

// Helper to get diffuse colors
// If the material doesn't have textures, a 1x1 white texture will be bound and configured color will be used
vec3 get_diffuse_color()
{
    return texture(diffuse_tex, TexCoords).rgb * material.diffuse_color;
}

// Calculates contribution of a single point light
vec3 compute_point_light(PointLightData light, vec3 normal, vec3 diffuse_color)
{
    vec3  light_dir   = normalize(light.position.xyz - FragPos);
    float dist        = length(light.position.xyz - FragPos);

    float attenuation = 1.0 / (light.attenuation.x +
                               light.attenuation.y * dist +
                               light.attenuation.z * dist * dist);

    float diff    = max(dot(normal, light_dir), 0.0);
    vec3  diffuse = diff * diffuse_color * light.color.xyz * light.color.w;

    return diffuse * attenuation;
}

// Calculates contribution of a single directional light
vec3 compute_directional_light(DirectionalLightData light, vec3 normal, vec3 base_color)
{
    vec3  light_dir = normalize(-light.direction.xyz);
    float diff      = max(dot(normal, light_dir), 0.0);
    return (material.ambient_color + diff * light.color.xyz * light.color.w) * base_color;
}

// Directional Shadows
float compute_directional_shadow(int index, vec3 normal, vec3 light_dir)
{
    vec4 frag_light_space = directional_lights.data[index].light_matrix * vec4(FragPos, 1.0);
    vec3 proj_coords      = frag_light_space.xyz / frag_light_space.w;
    proj_coords           = proj_coords * 0.5 + 0.5;

    if (proj_coords.z > 1.0)
        return 0.0;

    float shadow     = 0.0;
    vec2  texel_size = 1.0 / textureSize(directional_shadow_maps[index], 0);
    for (int x = -1; x <= 1; x++)
    for (int y = -1; y <= 1; y++)
    {
        float pcf_depth = texture(directional_shadow_maps[index], proj_coords.xy + vec2(x, y) * texel_size).r;
        shadow += proj_coords.z > pcf_depth ? 1.0 : 0.0;
    }
    return shadow / 9.0;
}

// Point Shadows
float compute_point_shadow(samplerCube shadow_cube, vec3 light_pos, float far_plane)
{
    float shadow  = 0.0;
    float bias    = 0.05; 
    
    // PCF control
    float samples = 20.0;
    float disk_radius = 0.01;
    float offset  = 0.1;

    vec3  frag_to_light = FragPos - light_pos;
    float current_depth = length(frag_to_light);

    // PCF to avoid jagged edges
    for(int i = 0; i < samples; ++i)
    {
        float closest_depth = texture(shadow_cube, frag_to_light + sample_offset_directions[i] * disk_radius).r;
        closest_depth *= far_plane;   // undo mapping [0;1]
        if(current_depth - bias > closest_depth)
            shadow += 1.0;
    }
    
    shadow /= float(samples);

    return shadow;
}

void main()
{
    vec3 normal = normalize(Normal);

    // Diffuse color, textured or not
    vec3 diffuse_color   = get_diffuse_color();

    // Ambient
    vec3 result = material.ambient_color * diffuse_color;

    // Accumulate each point light contribution
    for (int i = 0; i < point_lights.count; i++)
    {
        float shadow = 0.0;
        if (point_lights.data[i].shadow.x > 0.5)
        {
            shadow = compute_point_shadow(point_shadow_cubes[i], point_lights.data[i].position.xyz, point_lights.data[i].shadow.y);
        }

        result += compute_point_light(point_lights.data[i], normal, diffuse_color) * (1.0 - shadow);
    }

    // Accumulate each directional light contribution
    for (int i = 0; i < directional_lights.count; i++)
    {
        float shadow = 0.0;
        if (directional_lights.data[i].shadow.x > 0.5)
        {
            shadow = compute_directional_shadow(i, normal, normalize(-directional_lights.data[i].direction.xyz));
        }

        result += compute_directional_light(directional_lights.data[i], normal, diffuse_color) * (1.0 - shadow);
    }
    
    FragColor = vec4(result, 1.0);
}