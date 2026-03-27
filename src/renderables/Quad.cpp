#include "Quad.h"

Quad::Quad(const std::string& name) :
    Renderable(name)
{
    generate();
}

void Quad::generate()
{
    std::vector<Vertex> vertices = {
        // position             normal              uv
        { glm::vec3(-1, -1, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0) },
        { glm::vec3( 1, -1, 0), glm::vec3(0, 0, 1), glm::vec2(1, 0) },
        { glm::vec3( 1,  1, 0), glm::vec3(0, 0, 1), glm::vec2(1, 1) },
        { glm::vec3(-1,  1, 0), glm::vec3(0, 0, 1), glm::vec2(0, 1) },
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3
    };

    m_meshes.emplace_back(vertices, indices, nullptr);
}