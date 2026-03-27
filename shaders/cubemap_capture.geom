#version 460 core

// Input: triangles from the vertex shader
// Output: triangle strips, 18 vertices max (3 vertices × 6 cubemap faces)
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

// One view matrix per cubemap face
uniform mat4 capture_views[6];
uniform mat4 projection;

out vec3 local_pos;

void main()
{
    // Emit the input triangle to each of the 6 cubemap faces in a single draw call
    for (int face = 0; face < 6; face++)
    {
        // gl_Layer routes this primitive to the corresponding cubemap face
        gl_Layer = face;

        for (int i = 0; i < 3; i++)
        {
            // Pass world-space position to the fragment shader
            local_pos   = vec3(gl_in[i].gl_Position);

            // Transform to clip space using the face's view and projection matrices
            gl_Position = projection * capture_views[face] * gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}