#version 460 core

layout (location = 0) in vec3 aPos;

// View matrix with translation removed — keeps the skybox centered on the camera
uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;

void main()
{
    // As we use a unit cube centered at the origin, the vertex position
    // directly serves as the cubemap sampling direction
    TexCoords  = aPos;

    vec4 pos   = projection * view * vec4(aPos, 1.0);
    
    // Set z to w to make depth always 1, placing the skybox at the maximum depth value
    gl_Position = pos.xyww;
}