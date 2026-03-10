#include "Sphere.h"

Sphere::Sphere(float radius, unsigned int sectors, unsigned int stacks)
{
    // Automatically call to geometry generation with provided parameters
    generate(radius, sectors, stacks);
}

void Sphere::generate(float radius, unsigned int sectors, unsigned int stacks)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Iterate over each stack (latitude band from top to bottom)
    for (unsigned int i = 0; i <= stacks; ++i)
    {
        // Stack angle goes from +PI/2 (top) to -PI/2 (bottom)
        float stack_angle = glm::pi<float>() / 2.0f - i * (glm::pi<float>() / stacks);

        // XY radius and Z height at this stack level
        float xy = radius * cosf(stack_angle);
        float z = radius * sinf(stack_angle);

        // Iterate over each sector (longitude slice around the sphere)
        for (unsigned int j = 0; j <= sectors; ++j)
        {
            // Sector angle goes from 0 to 2*PI
            float sector_angle = j * (2.0f * glm::pi<float>() / sectors);

            Vertex v;

            // Compute vertex position from spherical coordinates
            v.position = { xy * cosf(sector_angle), xy * sinf(sector_angle), z };

            // Normal is the normalized position vector since the sphere is centered at origin
            v.normal = glm::normalize(v.position);

            // UV coordinates map sectors to U [0,1] and stacks to V [0,1]
            v.tex_coords = {
                (float)j / sectors,
                (float)i / stacks
            };
            vertices.push_back(v);
        }
    }

    // Build the index buffer by connecting adjacent vertices into triangles
    for (unsigned int i = 0; i < stacks; ++i)
    {
        for (unsigned int j = 0; j < sectors; ++j)
        {
            // k1 and k2 are the indices of the top-left and bottom-left vertices
           // of the current quad in the vertex grid
            unsigned int k1 = i * (sectors + 1) + j;
            unsigned int k2 = k1 + sectors + 1;

            // First triangle of the quad (top-left, bottom-left, top-right)
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            // Second triangle of the quad (top-right, bottom-left, bottom-right)
            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }
    }

    // Build the mesh from the generated geometry with no material assigned
    m_meshes.emplace_back(vertices, indices, nullptr);
}