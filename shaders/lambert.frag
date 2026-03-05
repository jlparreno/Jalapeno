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
};


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

// Textures
uniform sampler2D diffuse_tex;
uniform sampler2D normal_tex;

// Light and material
uniform Light light;
uniform Material material;

out vec4 FragColor;

// Helper to get diffuse colors
// If the material doesn't have textures, a 1x1 white texture will be bound and configured color will be used
vec3 get_diffuse_color()
{
    return texture(diffuse_tex, TexCoords).rgb * material.diffuse_color;
}

void main()
{
    //Get diffuse color, using texture if any
    vec3 diffuse_color   = get_diffuse_color();
    
    // Needed directions
    vec3 normal       = normalize(Normal);
    vec3 light_dir = normalize(light.position - FragPos);

    // Ambient
    vec3 ambient = material.ambient_color * diffuse_color;

    // Diffuse
    float diff    = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * diffuse_color * light.diffuse;
    
    // Final result
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}
