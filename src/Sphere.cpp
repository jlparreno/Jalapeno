#include "Sphere.h"

Sphere::Sphere(float radius, unsigned int sectors, unsigned int stacks)
{
    generate(radius, sectors, stacks);
}

void Sphere::draw(ShaderProgram* shader) const
{
    //Call to draw the sphere mesh
    m_meshes[0].draw(shader);
}

void Sphere::generate(float radius, unsigned int sectors, unsigned int stacks)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i <= stacks; ++i)
    {
        float stack_angle = glm::pi<float>() / 2.0f - i * (glm::pi<float>() / stacks);
        float xy = radius * cosf(stack_angle);
        float z = radius * sinf(stack_angle);

        for (unsigned int j = 0; j <= sectors; ++j)
        {
            float sector_angle = j * (2.0f * glm::pi<float>() / sectors);

            Vertex v;
            v.position = { xy * cosf(sector_angle), xy * sinf(sector_angle), z };
            v.normal = glm::normalize(v.position);
            v.tex_coords = {
                (float)j / sectors,
                (float)i / stacks
            };
            vertices.push_back(v);
        }
    }

    for (unsigned int i = 0; i < stacks; ++i)
    {
        for (unsigned int j = 0; j < sectors; ++j)
        {
            unsigned int k1 = i * (sectors + 1) + j;
            unsigned int k2 = k1 + sectors + 1;

            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }
    }

    m_meshes.emplace_back(vertices, indices, nullptr);
}