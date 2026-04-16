#version 460 core

#include "lights.glsl"

const float PI = 3.14159265359;

const vec3 sample_offset_directions[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

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

// --- PBR Functions ---

// Normal Distribution Function (NDF) — Trowbridge-Reitz GGX
//
// Estimates the fraction of microfacets aligned with the half-vector H.
// Only microfacets aligned with H reflect L directly into V, so it quantifies
// specular intensity at this angle.
//
// Roughness is squared twice (alpha = r², then alpha²) for a perceptually linear response
// (Disney/Burley 2012).
//
// Returns the specular peak density that must be multiplied by the Geometry (G) and Fresnel (F) 
// terms to complete the Cook-Torrance BRDF.
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
//
// Models the probability that microfacets are NOT self-shadowed or self-occluded. 
// Given a view/light angle NdotV, returns the fraction of microfacets visible from that direction.

// Two failure modes exist: 
// - shadowing: L is blocked before reaching a microfacet.
// - masking: V is blocked on the way out. 
//
// Smith's method handles both independently and multiplies the results, which geometry_smith() does below.
//
// k is the remapped roughness for the denominator. The formula differs between
// - Direct lighting: k = (r+1)² / 8
// - IBL: k = r² / 2
//
// At NdotV = 1 (surface faces the observer straight on), returns 1.0 (no occlusion).
// At NdotV = 0 (grazing angle), returns a small but non-zero value.
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
//
// Models how much light is reflected vs refracted at a surface,
// depending on the viewing angle. This is a physical phenomenon: any surface,
// no matter how matte, becomes mirror-like at grazing angles (Fresnel effect).
//
// F0 is the base reflectance at normal incidence (cos_theta = 1, ray hits straight on).
// It encodes the material type as an RGB value.
//
// cos_theta = dot(H, V): the angle between the half-vector and the view direction.
//   cos_theta = 1: ray is perpendicular to the surface → returns F0 (minimum reflection)
//   cos_theta = 0: grazing angle                       → returns vec3(1.0) (full reflection)
//
// Returns a value between F0 and 1.0 per channel — the fraction of incoming light
// that is specularly reflected toward V.
vec3 fresnel_schlick(vec3 H, vec3 V, vec3 F0)
{
    float cos_theta = max(dot(H, V), 0.0);
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

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

// --- SHADOWS Functions ---

// Directional Lights Shadows
float compute_shadow(int index, vec3 normal, vec3 light_dir)
{
    vec4 frag_light_space = directional_lights.data[index].light_matrix * vec4(FragPos, 1.0);
    vec3 proj_coords      = frag_light_space.xyz / frag_light_space.w;
    proj_coords           = proj_coords * 0.5 + 0.5;

    if (proj_coords.z > 1.0)
        return 0.0;

    float shadow     = 0.0;
    vec2  texel_size = 1.0 / textureSize(directional_shadow_maps[index], 0);

    // PCF — 3x3 kernel
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcf_depth = texture(directional_shadow_maps[index], proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += proj_coords.z > pcf_depth ? 1.0 : 0.0;
        }
    }

    return shadow / 9.0;
}

// Point Lights Shadows
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
        closest_depth *= far_plane;   // undo mapping [0-1]
        if(current_depth - bias > closest_depth)
        {
            shadow += 1.0;
        }
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

    // View ray direction
    vec3 V = normalize(view_pos - FragPos);

    // Initialize reflectance
    // F0: Reflectance in 0 degrees
    // No metallics always ~0.04, for metallics always the albedo
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // Initialize final result, outgoing radiance
    vec3 Lo = vec3(0.0);

    // Point Lights
    for (int i = 0; i < point_lights.count; i++)
    {
        vec3  L         = normalize(point_lights.data[i].position.xyz - FragPos);
        vec3  H         = normalize(V + L);

        float dist      = length(point_lights.data[i].position.xyz - FragPos);
        float atten     = 1.0 / (point_lights.data[i].attenuation.x +
                                 point_lights.data[i].attenuation.y * dist +
                                 point_lights.data[i].attenuation.z * dist * dist);

        vec3  radiance  = point_lights.data[i].color.xyz * point_lights.data[i].color.w * atten;

        // Cook-Torrance BRDF
        float NDF = distribution_ggx(N, H, roughness);  // Normal Distribution Function
        float G   = geometry_smith(N, V, L, roughness); // Geometry function
        vec3  F   = fresnel_schlick(H, V, F0);          // Fresnel

        vec3  num         = NDF * G * F;
        float denom       = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // Add 0.0001 to prevent division by zero
        vec3  specular    = num / denom;

        // kS = Fresnel (Specular energy)
        // kD = Rest (Diffuse energy), metallics doesn't have diffuse
        vec3  kS = F;
        vec3  kD = (vec3(1.0) - kS) * (1.0 - metallic);

        // Compute point light shadow
        float shadow = 0.0;
        if (point_lights.data[i].shadow.x > 0.5)
        {
            shadow = compute_point_shadow(point_shadow_cubes[i], point_lights.data[i].position.xyz, point_lights.data[i].shadow.y);
        }

        // Final contribution of this light
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow);
    }

    // Directional Lights
    for (int i = 0; i < directional_lights.count; i++)
    {
        vec3  L        = normalize(-directional_lights.data[i].direction.xyz);
        vec3  H        = normalize(V + L);

        vec3  radiance = directional_lights.data[i].color.xyz * directional_lights.data[i].color.w;

        // Cook-Torrance BRDF
        float NDF = distribution_ggx(N, H, roughness);  // Normal Distribution Function
        float G   = geometry_smith(N, V, L, roughness); // Geometry function
        vec3  F   = fresnel_schlick(H, V, F0);          // Fresnel

        vec3  num      = NDF * G * F;
        float denom    = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // Add 0.0001 to prevent division by zero
        vec3  specular = num / denom;

        vec3  kS    = F;
        vec3  kD    = (vec3(1.0) - kS) * (1.0 - metallic);

        // Compute shadow for this light
        float shadow = 0.0;
        if (directional_lights.data[i].shadow.x > 0.5)
        {
            shadow = compute_shadow(i, N, L);
        }

        // Final contribution of this light
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow);
    }

    // Minimal ambient for dark zones
    vec3 ambient = vec3(0.08) * albedo;
    vec3 color   = ambient + Lo;

    // Tone mapping + gamma correction
    color = aces_tone_map(color);           // ACES tone map, as calculations are done in HDR we need to compress to LDR 0-1 before display
    color = pow(color, vec3(1.0 / 2.2));    // Gamma Correction, PBR requires all inputs to be linear so we adapt the output to sRGB

    FragColor = vec4(color, alpha);
}
