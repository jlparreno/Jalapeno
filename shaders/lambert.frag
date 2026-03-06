#version 460 core

struct PointLight 
{
    vec3  position;
    vec3  color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight 
{
    vec3  direction;
    vec3  color;
    float intensity;
};

#define MAX_POINT_LIGHTS 8
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform int        num_point_lights;

#define MAX_DIRECTIONAL_LIGHTS 4
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
        result += compute_point_light(point_lights[i], normal, diffuse_color);
    }

    // Accumulate each directional light contribution
    for (int i = 0; i < num_directional_lights; i++)
    {
        result += compute_directional_light(directional_lights[i], normal, diffuse_color);
    }
    
    FragColor = vec4(result, 1.0);
}