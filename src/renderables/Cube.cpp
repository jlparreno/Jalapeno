#include "Cube.h"

Cube::Cube(const std::string& name) :
    Renderable(name)
{
    generate();
}

void Cube::generate()
{
    std::vector<Vertex> vertices = {
        // Back face 
        // position             normal              uv
        { glm::vec3(-1,-1,-1),  glm::vec3(0,0,-1),  glm::vec2(0,0) },
        { glm::vec3( 1, 1,-1),  glm::vec3(0,0,-1),  glm::vec2(1,1) },
        { glm::vec3( 1,-1,-1),  glm::vec3(0,0,-1),  glm::vec2(1,0) },
        { glm::vec3(-1, 1,-1),  glm::vec3(0,0,-1),  glm::vec2(0,1) },
        // Front face
        { glm::vec3(-1,-1, 1),  glm::vec3(0,0, 1),  glm::vec2(0,0) },
        { glm::vec3( 1,-1, 1),  glm::vec3(0,0, 1),  glm::vec2(1,0) },
        { glm::vec3( 1, 1, 1),  glm::vec3(0,0, 1),  glm::vec2(1,1) },
        { glm::vec3(-1, 1, 1),  glm::vec3(0,0, 1),  glm::vec2(0,1) },
        // Left face
        { glm::vec3(-1, 1, 1),  glm::vec3(-1,0,0),  glm::vec2(1,0) },
        { glm::vec3(-1, 1,-1),  glm::vec3(-1,0,0),  glm::vec2(1,1) },
        { glm::vec3(-1,-1,-1),  glm::vec3(-1,0,0),  glm::vec2(0,1) },
        { glm::vec3(-1,-1, 1),  glm::vec3(-1,0,0),  glm::vec2(0,0) },
        // Right face
        { glm::vec3( 1, 1, 1),  glm::vec3(1,0,0),   glm::vec2(1,0) },
        { glm::vec3( 1,-1,-1),  glm::vec3(1,0,0),   glm::vec2(0,1) },
        { glm::vec3( 1, 1,-1),  glm::vec3(1,0,0),   glm::vec2(1,1) },
        { glm::vec3( 1,-1, 1),  glm::vec3(1,0,0),   glm::vec2(0,0) },
        // Bottom face
        { glm::vec3(-1,-1,-1),  glm::vec3(0,-1,0),  glm::vec2(0,1) },
        { glm::vec3( 1,-1,-1),  glm::vec3(0,-1,0),  glm::vec2(1,1) },
        { glm::vec3( 1,-1, 1),  glm::vec3(0,-1,0),  glm::vec2(1,0) },
        { glm::vec3(-1,-1, 1),  glm::vec3(0,-1,0),  glm::vec2(0,0) },
        // Top face
        { glm::vec3(-1, 1,-1),  glm::vec3(0,1,0),   glm::vec2(0,1) },
        { glm::vec3( 1, 1, 1),  glm::vec3(0,1,0),   glm::vec2(1,0) },
        { glm::vec3( 1, 1,-1),  glm::vec3(0,1,0),   glm::vec2(1,1) },
        { glm::vec3(-1, 1, 1),  glm::vec3(0,1,0),   glm::vec2(0,0) },
    };

    std::vector<unsigned int> indices = {
        0,  1,  2,   0,  3,  1,   // back
        4,  5,  6,   4,  6,  7,   // front
        8,  9,  10,  8,  10, 11,  // left
        12, 13, 14,  12, 15, 13,  // right
        16, 17, 18,  16, 18, 19,  // bottom
        20, 21, 22,  20, 23, 21   // top
    };

    m_meshes.emplace_back(vertices, indices, nullptr);
}