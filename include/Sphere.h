#pragma once

#include "Renderable.h"
#include "Mesh.h"

class Sphere : public Renderable
{
public:

    Sphere(float radius = 1.0f, unsigned int sectors = 36, unsigned int stacks = 18);

    void draw(ShaderProgram* shader) const override;

    const std::vector<Mesh>& get_meshes() const override { return m_meshes; };

    void set_material(Material* material) { m_meshes[0].set_material(material); }

private:

    // Will always have one unique mesh
    std::vector<Mesh> m_meshes;

    void generate(float radius, unsigned int sectors, unsigned int stacks);
};