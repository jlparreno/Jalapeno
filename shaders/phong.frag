#version 460 core

struct Light 
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material 
{
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;

    float shininess;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

// Scene data
uniform vec3 view_pos;

// Textures
uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;
uniform sampler2D normal_tex;

// Light and material
uniform Light light;
uniform Material material;

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

    // Light and view directions
    vec3 light_dir = normalize(light.position - FragPos);
    vec3 view_dir = normalize(view_pos - FragPos);

    // Ambient
    vec3 ambient = light.ambient * diffuse_color;

    // Diffuse
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuse_color;

    //Specular
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * specular_color;

    // Final result
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}


