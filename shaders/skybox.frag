#version 460 core

uniform samplerCube environment_map;

in  vec3 TexCoords;
out vec4 FragColor;

// --- TONE-MAPPING ---

// ACES is the industry-standard filmic curve (used in Unreal, Blender, etc.) 
// It adds slight contrast and warm highlights characteristic of film stock.
vec3 aces_tone_map(vec3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    vec3 color   = texture(environment_map, TexCoords).rgb;
    
    // Tonemapping + gamma to match scene colors
    color = aces_tone_map(color);
    color = pow(color, vec3(1.0 / 2.2));

    FragColor   = vec4(color, 1.0);
}