#version 460 core

// Precomputed constant for spherical to UV mapping
// 1 / (2π) and 1 / π, used to normalize the atan and asin results to [0, 1]
const vec2 inv_atan = vec2(0.1591, 0.3183);

// Equirectangular HDR texture to sample from
uniform sampler2D equirect_map;

in vec3 local_pos;
out vec4 FragColor;

// Converts a normalized direction vector to equirectangular UV coordinates.
// Uses spherical coordinates: phi = atan(z, x) for longitude,
// theta = asin(y) for latitude, both remapped to [0, 1].
vec2 sample_spherical_map(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= inv_atan;
    uv += 0.5;
    return uv;
}

void main()
{
    // Convert the local position direction to spherical UV coordinates
    vec2 uv    = sample_spherical_map(normalize(local_pos));

    // Sample the equirectangular map and output the color
    vec3 color = texture(equirect_map, uv).rgb;
    FragColor = vec4(color, 1.0);
}