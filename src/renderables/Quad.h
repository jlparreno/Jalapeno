#pragma once

#include "Renderable.h"

/**
 * @class Quad
 *
 * @brief Procedural quad renderable
 *
 * Generates a quad covering NDC space from -1 to 1 with correct
 * texture coordinates.
 */
class Quad : public Renderable
{
public:

    /**
     * @brief Constructs a Cube and generates its geometry.
     */
    Quad(const std::string& name);

    /**
     * @brief Returns the cube mesh
     *
     * A cube always contains exactly one mesh generated procedurally
     * at construction time.
     *
     * @return Const reference to the internal mesh vector
     */
    const std::vector<Mesh>& get_meshes() const override { return m_meshes; }

    /**
     * @brief Sets the material used to render this cube
     *
     * @param material Pointer to the material to assign
     */
    void set_material(Material* material) { m_meshes[0].set_material(material); }

private:

    /**
     * @brief Generates the cube vertex and index buffers.
     */
    void generate();

    // Internal mesh vector. Always contains exactly one procedurally generated mesh
    std::vector<Mesh> m_meshes;
};