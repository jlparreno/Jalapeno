#version 460 core

struct Light 
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 view_pos;

// Names convention for textures
uniform sampler2D diffuse_tex;
uniform sampler2D normal_tex;
uniform sampler2D specular_tex;

uniform Light light;

out vec4 FragColor;

void main()
{
    // Ambient
    vec3 ambient = light.ambient * vec3(texture(diffuse_tex, TexCoords));

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(diffuse_tex, TexCoords));

    //Specular
    vec3 viewDir = normalize(view_pos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    //32 value is the shininess value of the highlight
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = light.specular * spec * vec3(texture(specular_tex, TexCoords));

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}


