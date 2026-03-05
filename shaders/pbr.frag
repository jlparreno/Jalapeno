#version 460 core

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

#define MAX_POINT_LIGHTS 8
uniform PointLight u_point_lights[MAX_POINT_LIGHTS];
uniform int        u_num_point_lights;

#define MAX_DIRECTIONAL_LIGHTS 4
uniform DirectionalLight u_directional_lights[MAX_DIRECTIONAL_LIGHTS];
uniform int              u_num_directional_lights;

struct Material 
{
    bool    use_normal_map;
    vec3    albedo_color;
    float   metallic;
    float   roughness;
    bool    alpha_cutout;
    float   alpha_threshold;
};
uniform Material material;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

// Scene data
uniform vec3 view_pos;

// Textures
uniform sampler2D albedo_tex;
uniform sampler2D metallic_roughness_tex;
uniform sampler2D normal_tex;

out vec4 FragColor;

// PBR Functions

// Normal Distribution Function — GGX/Trowbridge-Reitz
float distribution_ggx(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

// Geometry Function — Smith's Schlick-GGX
float geometry_schlick_ggx(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return geometry_schlick_ggx(NdotV, roughness)
         * geometry_schlick_ggx(NdotL, roughness);
}

// Fresnel — Schlick approximation
vec3 fresnel_schlick(float cos_theta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

vec3 aces_tone_map(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    // Muestrar texturas
    vec4 albedo_sample = texture(albedo_tex, TexCoords);
    vec3 albedo        = albedo_sample.rgb * material.albedo_color;
    float alpha        = albedo_sample.a;

    vec2  mr_sample = texture(metallic_roughness_tex, TexCoords).bg; // B=metallic, G=roughness (glTF)
    float metallic  = mr_sample.x * material.metallic;
    float roughness = mr_sample.y * material.roughness;

    // Normal
    vec3 N;
    if (material.use_normal_map)
    {
        // Samplear el normal map y transformar de [0,1] a [-1,1]
        vec3 normal_sample = texture(normal_tex, TexCoords).rgb;
        normal_sample = normal_sample * 2.0 - 1.0;
        normal_sample.y = -normal_sample.y;  // invertir canal Y (DirectX → OpenGL)

        // Recalcular TBN en el fragment shader con derivadas parciales
        vec3 Q1  = dFdx(FragPos);
        vec3 Q2  = dFdy(FragPos);
        vec2 st1 = dFdx(TexCoords);
        vec2 st2 = dFdy(TexCoords);

        vec3 N_  = normalize(Normal);  // normal del vértice como base
        vec3 T   = normalize(Q1 * st2.t - Q2 * st1.t);
        T        = normalize(T - dot(T, N_) * N_); // Gram-Schmidt
        vec3 B   = normalize(cross(N_, T));
        mat3 TBN = mat3(T, B, N_);

        N = normalize(TBN * normal_sample);
    }
    else
    {
        N = normalize(Normal);
    }

    vec3 V = normalize(view_pos - FragPos);

    // F0: reflectancia en incidencia 0 grados
    // Para no metálicos es siempre ~0.04, para metálicos es el albedo
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);

    // Point Lights
    for (int i = 0; i < u_num_point_lights; i++)
    {
        vec3  L         = normalize(u_point_lights[i].position - FragPos);
        vec3  H         = normalize(V + L);
        float dist      = length(u_point_lights[i].position - FragPos);
        float atten     = 1.0 / (u_point_lights[i].constant
                        + u_point_lights[i].linear    * dist
                        + u_point_lights[i].quadratic * dist * dist);
        vec3  radiance  = u_point_lights[i].color * u_point_lights[i].intensity * atten;

        // Cook-Torrance BRDF
        float NDF = distribution_ggx(N, H, roughness);
        float G   = geometry_smith(N, V, L, roughness);
        vec3  F   = fresnel_schlick(max(dot(H, V), 0.0), F0);

        vec3  num         = NDF * G * F;
        float denom       = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3  specular    = num / denom;

        // kS = Fresnel (energía especular)
        // kD = resto (energía difusa), metales no tienen difuso
        vec3  kS = F;
        vec3  kD = (vec3(1.0) - kS) * (1.0 - metallic);

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // Directional Lights
    for (int i = 0; i < u_num_directional_lights; i++)
    {
        vec3  L        = normalize(-u_directional_lights[i].direction);
        vec3  H        = normalize(V + L);
        vec3  radiance = u_directional_lights[i].color * u_directional_lights[i].intensity;

        float NDF = distribution_ggx(N, H, roughness);
        float G   = geometry_smith(N, V, L, roughness);
        vec3  F   = fresnel_schlick(max(dot(H, V), 0.0), F0);

        vec3  num      = NDF * G * F;
        float denom    = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3  specular = num / denom;

        vec3  kS    = F;
        vec3  kD    = (vec3(1.0) - kS) * (1.0 - metallic);
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // Ambient mínimo para que las zonas sin luz no sean negro puro
    vec3 ambient = vec3(0.08) * albedo;
    vec3 color   = ambient + Lo;

    // Tone mapping + gamma correction
    // color = color / (color + vec3(1.0));  // Reinhard
    color = aces_tone_map(color);            // ACES
    color = pow(color, vec3(1.0 / 2.2));    // gamma correction

    FragColor = vec4(color, alpha);
}
