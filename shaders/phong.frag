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

// Light and material
uniform Light light;
uniform Material material;

out vec4 FragColor;

void main()
{
    // Discard transparent fragments
    vec4 diffSample = texture(diffuse_tex, TexCoords);
    if (diffSample.a < 0.5)
        discard;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 viewDir = normalize(view_pos - FragPos);

    // Ambient
    vec3 ambient = light.ambient * diffSample.xyz;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffSample.xyz;

    //Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(specular_tex, TexCoords));

    // Final result
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}


