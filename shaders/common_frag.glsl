const float PI = 3.14159265359;

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

#define MAX_POINT_LIGHTS       8
#define MAX_DIRECTIONAL_LIGHTS 4

uniform PointLight       u_point_lights[MAX_POINT_LIGHTS];
uniform DirectionalLight u_directional_lights[MAX_DIRECTIONAL_LIGHTS];

uniform int              u_num_point_lights;
uniform int              u_num_directional_lights;