#pragma once

#include "Renderable.h"
#include "Material.h"

/**
 * @class Sphere
 *
 * @brief Procedurally generated sphere primitive
 *
 * Sphere generates a triangulated sphere mesh at construction time
 * from the given radius, sector and stack counts. The geometry is built
 * using spherical coordinates and uploaded to the GPU via a single Mesh.
 *
 * Sectors define the number of horizontal subdivisions around the sphere
 * (longitude), while stacks define the number of vertical subdivisions
 * (latitude). Higher values produce a smoother sphere at the cost of
 * additional geometry.
 */
class Sphere : public Renderable
{
public:

    /**
     * @brief Constructs a Sphere with the given geometric parameters
     *
     * @param radius  Radius of the sphere in world units
     * @param sectors Number of horizontal subdivisions (longitude)
     * @param stacks  Number of vertical subdivisions (latitude)
     */
    Sphere(float radius = 1.0f, unsigned int sectors = 36, unsigned int stacks = 18);

    /**
     * @brief Draws the sphere using the given shader
     *
     * Binds the material and submits the sphere's mesh to the GPU.
     * The shader is assumed to be already bound before this call.
     *
     * @param shader Pointer to the ShaderProgram to use for rendering
     */
    void draw(ShaderProgram* shader) const override;

    /**
     * @brief Returns the sphere mesh
     *
     * A sphere always contains exactly one mesh generated procedurally
     * at construction time.
     *
     * @return Const reference to the internal mesh vector
     */
    const std::vector<Mesh>& get_meshes() const override { return m_meshes; }

    /**
     * @brief Sets the material used to render this sphere
     *
     * @param material Pointer to the material to assign.
     */
    void set_material(Material* material) { m_meshes[0].set_material(material); }

private:

    /**
     * @brief Generates the sphere geometry and uploads it to the GPU
     *
     * Computes vertex positions, normals and texture coordinates using
     * spherical coordinates, then builds the index buffer from the resulting
     * vertex grid. The generated mesh is stored in m_meshes.
     *
     * @param radius  Radius of the sphere in world units
     * @param sectors Number of horizontal subdivisions (longitude)
     * @param stacks  Number of vertical subdivisions (latitude)
     */
    void generate(float radius, unsigned int sectors, unsigned int stacks);

    // Internal mesh vector. Always contains exactly one procedurally generated mesh
    std::vector<Mesh> m_meshes;
};