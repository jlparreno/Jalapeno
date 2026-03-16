#version 460 core

in vec4 FragPos;

uniform vec3 light_pos;
uniform float far_plane;

void main()
{
    float dist = length(vec3(FragPos) - light_pos);
    gl_FragDepth = dist / far_plane; // normalize to [0,1]
}