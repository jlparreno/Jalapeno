#version 460 core

const vec3 sample_offset_directions[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);  

struct PointLight 
{
    vec3  position;
    vec3  color;
    float intensity;
    float constant;
    float linear;
    float quadratic;

    bool        shadows_enabled;
    samplerCube shadow_cube;
    float       far_plane;
};

struct DirectionalLight 
{
    vec3  direction;
    vec3  color;
    float intensity;

    bool      shadows_enabled;
    mat4      light_matrix;
    sampler2D shadow_tex;
};

#define MAX_POINT_LIGHTS 4
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform int        num_point_lights;

#define MAX_DIRECTIONAL_LIGHTS 2
uniform DirectionalLight directional_lights[MAX_DIRECTIONAL_LIGHTS];
uniform int              num_directional_lights;

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
vec3 compute_point_light(PointLight light, vec3 normal, vec3 diffuse_color)
{
    vec3  light_dir   = normalize(light.position - FragPos);
    float dist        = length(light.position - FragPos);

    float attenuation = 1.0 / (light.constant + 
                               light.linear * dist + 
                               light.quadratic * dist * dist);

    float diff    = max(dot(normal, light_dir), 0.0);
    vec3  diffuse = diff * diffuse_color * light.color * light.intensity;

    return diffuse * attenuation;
}

// Calculates contribution of a single directional light
vec3 compute_directional_light(DirectionalLight light, vec3 normal, vec3 base_color)
{
    vec3  light_dir = normalize(-light.direction);
    float diff      = max(dot(normal, light_dir), 0.0);
    return (material.ambient_color + diff * light.color * light.intensity) * base_color;
}

// Directional Shadows
float compute_directional_shadow(int index, vec3 normal, vec3 light_dir)
{
    vec4  frag_light_space = directional_lights[index].light_matrix * vec4(FragPos, 1.0);
    vec3  proj_coords      = frag_light_space.xyz / frag_light_space.w;
    proj_coords            = proj_coords * 0.5 + 0.5;

    if (proj_coords.z > 1.0)
        return 0.0;

    float shadow     = 0.0;
    vec2  texel_size = 1.0 / textureSize(directional_lights[index].shadow_tex, 0);
    for (int x = -1; x <= 1; x++)
    for (int y = -1; y <= 1; y++)
    {
        float pcf_depth = texture(directional_lights[index].shadow_tex, proj_coords.xy + vec2(x, y) * texel_size).r;
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
    for (int i = 0; i < num_point_lights; i++)
    {
        float shadow = 0.0;
        if (point_lights[i].shadows_enabled)
        {
            shadow = compute_point_shadow(point_lights[i].shadow_cube, point_lights[i].position, point_lights[i].far_plane);
        }

        result += compute_point_light(point_lights[i], normal, diffuse_color) * (1.0 - shadow);
    }

    // Accumulate each directional light contribution
    for (int i = 0; i < num_directional_lights; i++)
    {
        float shadow = 0.0;
        if (directional_lights[i].shadows_enabled)
        {
            shadow = compute_directional_shadow(i, normal, normalize(-directional_lights[i].direction));
        }

        result += compute_directional_light(directional_lights[i], normal, diffuse_color) * (1.0 - shadow);
    }
    
    FragColor = vec4(result, 1.0);
}