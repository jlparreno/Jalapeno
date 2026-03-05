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
uniform PointLight u_point_lights[MAX_POINT_LIGHTS];
uniform int        u_num_point_lights;

#define MAX_DIRECTIONAL_LIGHTS 4
uniform DirectionalLight u_directional_lights[MAX_DIRECTIONAL_LIGHTS];
uniform int              u_num_directional_lights;

struct Material 
{
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;

    float shininess;
};

uniform Material material;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

// Scene data
uniform vec3 view_pos;

// Textures
uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;
uniform sampler2D normal_tex;

out vec4 FragColor;

// Helpers to get diffuse and specular colors
// If the material doesn't have textures, a 1x1 white texture will be bound and configured color will be used
vec3 get_diffuse_color()
{
    return texture(diffuse_tex, TexCoords).rgb * material.diffuse_color;
}

vec3 get_specular_color()
{
    return texture(specular_tex, TexCoords).rgb * material.specular_color;
}

float compute_attenuation(PointLight light, float dist)
{
    return 1.0 / (light.constant
                + light.linear    * dist
                + light.quadratic * dist * dist);
}

vec3 compute_point_light(PointLight light, vec3 normal, vec3 view_dir, vec3 diffuse_color, vec3 specular_color)
{
    vec3  light_dir   = normalize(light.position - FragPos);
    float dist        = length(light.position - FragPos);
    float attenuation = compute_attenuation(light, dist);

    // Ambient
    vec3 ambient = material.ambient_color * diffuse_color;

    // Diffuse
    float diff    = max(dot(normal, light_dir), 0.0);
    vec3  diffuse = diff * diffuse_color * light.color * light.intensity;

    // Specular
    vec3  reflect_dir = reflect(-light_dir, normal);
    float spec        = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3  specular    = spec * specular_color * light.color * light.intensity;

    return (ambient + diffuse + specular) * attenuation;
}

vec3 compute_directional_light(DirectionalLight light, vec3 normal, vec3 view_dir, vec3 diffuse_color, vec3 specular_color)
{
    vec3 light_dir = normalize(-light.direction);

    // Ambient
    vec3 ambient = material.ambient_color * diffuse_color;

    // Diffuse
    float diff    = max(dot(normal, light_dir), 0.0);
    vec3  diffuse = diff * diffuse_color * light.color * light.intensity;

    // Specular
    vec3  reflect_dir = reflect(-light_dir, normal);
    float spec        = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3  specular    = spec * specular_color * light.color * light.intensity;

    return ambient + diffuse + specular;
}

void main()
{
    vec4 diff_sample = texture(diffuse_tex, TexCoords);

    // Discard transparent fragments
    if (diff_sample.a < 0.5)
        discard;

    // Diffuse and specular colors, textured or not
    vec3 diffuse_color = get_diffuse_color();
    vec3 specular_color = get_specular_color();

    // Normal, no normal mapping for now
    vec3 normal = normalize(Normal);
    //    if(material.use_normal_texture)
    //        norm = texture(normal_tex, TexCoords).xyz;

    vec3 view_dir = normalize(view_pos - FragPos);


    vec3 result = vec3(0.0);

    for (int i = 0; i < u_num_point_lights; i++)
    {
        result += compute_point_light(u_point_lights[i], normal, view_dir, diffuse_color, specular_color);
    }

    for (int i = 0; i < u_num_directional_lights; i++)
    {
        result += compute_directional_light(u_directional_lights[i], normal, view_dir, diffuse_color, specular_color);
    }

    FragColor = vec4(result, 1.0);
}


