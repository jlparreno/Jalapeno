#version 460 core

const float PI = 3.14159265359;

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

    bool        shadows_enabled;
    mat4        light_matrix;
    sampler2D   shadow_tex;
};

#define MAX_POINT_LIGHTS 4
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform int        num_point_lights;

#define MAX_DIRECTIONAL_LIGHTS 2
uniform DirectionalLight directional_lights[MAX_DIRECTIONAL_LIGHTS];
uniform int              num_directional_lights;

struct Material 
{
    bool    use_normal_map;
    vec3    albedo_color;
    float   metallic;
    float   roughness;
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

// Tone - mapping
vec3 aces_tone_map(vec3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

//Directional Light Shadows
float compute_shadow(int index, vec3 normal, vec3 light_dir)
{
    vec4  frag_light_space = directional_lights[index].light_matrix * vec4(FragPos, 1.0);
    vec3  proj_coords      = frag_light_space.xyz / frag_light_space.w;
    proj_coords            = proj_coords * 0.5 + 0.5;

    if (proj_coords.z > 1.0)
        return 0.0;

    // Bias for shadow acne, at the moment giving Peter panning
    //float bias        = max(0.005 * (1.0 - dot(normal, light_dir)), 0.0001);
    float shadow      = 0.0;
    vec2  texel_size  = 1.0 / textureSize(directional_lights[index].shadow_tex, 0);

    // PCF function, 9 samples around fragment
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcf_depth = texture(directional_lights[index].shadow_tex, proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += proj_coords.z /*- bias*/ > pcf_depth ? 1.0 : 0.0;
        }
    }

    return shadow / 9.0;  // 9 samples average
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
    // Sample textures
    vec4 albedo_sample = texture(albedo_tex, TexCoords);
    vec3 albedo        = albedo_sample.rgb * material.albedo_color;
    float alpha        = albedo_sample.a;

    vec2  mr_sample = texture(metallic_roughness_tex, TexCoords).bg; // B=metallic, G=roughness (glTF)
    float metallic  = mr_sample.x * material.metallic;
    float roughness = mr_sample.y * material.roughness;

    // Normal, from data or normal texture, depending on uniform flag
    vec3 N;
    if (material.use_normal_map)
    {
        // Sample normal map and transform from [0,1] to [-1,1]
        vec3 normal_sample = texture(normal_tex, TexCoords).rgb;
        normal_sample = normal_sample * 2.0 - 1.0;
        normal_sample.y = -normal_sample.y;  // Invert Y (DirectX → OpenGL)

        // Recalculate TBN here, to avoid original incorrect data
        vec3 Q1  = dFdx(FragPos);
        vec3 Q2  = dFdy(FragPos);
        vec2 st1 = dFdx(TexCoords);
        vec2 st2 = dFdy(TexCoords);

        vec3 N_  = normalize(Normal);  // Vertex normal as base
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

    // F0: Reflectance in 0 degrees
    // No metallics always ~0.04, for metallics always the albedo
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);

    // Point Lights
    for (int i = 0; i < num_point_lights; i++)
    {
        vec3  L         = normalize(point_lights[i].position - FragPos);
        vec3  H         = normalize(V + L);
        float dist      = length(point_lights[i].position - FragPos);
        
        float atten     = 1.0 / (point_lights[i].constant + 
                                 point_lights[i].linear * dist + 
                                 point_lights[i].quadratic * dist * dist);

        vec3  radiance  = point_lights[i].color * point_lights[i].intensity * atten;

        // Cook-Torrance BRDF
        float NDF = distribution_ggx(N, H, roughness);
        float G   = geometry_smith(N, V, L, roughness);
        vec3  F   = fresnel_schlick(max(dot(H, V), 0.0), F0);

        vec3  num         = NDF * G * F;
        float denom       = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3  specular    = num / denom;

        // kS = Fresnel (Specular energy)
        // kD = resto (Diffuse energy), metallics doesn't have diffuse
        vec3  kS = F;
        vec3  kD = (vec3(1.0) - kS) * (1.0 - metallic);

        float NdotL = max(dot(N, L), 0.0);

        // Compute point light shadow
        float shadow = 0.0;
        if (point_lights[i].shadows_enabled)
        {
            shadow = compute_point_shadow(point_lights[i].shadow_cube, point_lights[i].position, point_lights[i].far_plane);
        }

        Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow);
    }

    // Directional Lights
    for (int i = 0; i < num_directional_lights; i++)
    {
        vec3  L        = normalize(-directional_lights[i].direction);
        vec3  H        = normalize(V + L);
        vec3  radiance = directional_lights[i].color * directional_lights[i].intensity;

        float NDF = distribution_ggx(N, H, roughness);
        float G   = geometry_smith(N, V, L, roughness);
        vec3  F   = fresnel_schlick(max(dot(H, V), 0.0), F0);

        vec3  num      = NDF * G * F;
        float denom    = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3  specular = num / denom;

        vec3  kS    = F;
        vec3  kD    = (vec3(1.0) - kS) * (1.0 - metallic);
        float NdotL = max(dot(N, L), 0.0);

        // Compute shadow for this light
        float shadow = 0.0;
        if(directional_lights[i].shadows_enabled)
        {
            shadow = compute_shadow(i, N, L);
        }

        Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow);
    }

    // Minimal ambient for dark zones
    vec3 ambient = vec3(0.08) * albedo;
    vec3 color   = ambient + Lo;

    // Tone mapping + gamma correction
    color = aces_tone_map(color);           // ACES
    color = pow(color, vec3(1.0 / 2.2));    // gamma correction

    FragColor = vec4(color, alpha);
}
