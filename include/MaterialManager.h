#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <spdlog/spdlog.h>

#include "Material.h"

/**
 * @class MaterialManager
 * 
 * @brief Singleton class for managing materials in the engine
 *
 * This class allows loading, storing, and retrieving materials by name
 * It ensures that each material is loaded only once.
 */
class MaterialManager
{
public:

    /**
     * @brief Retrieves the singleton instance of the MaterialManager
     *
     * @return Reference to the unique MaterialManager instance
     */
    static MaterialManager& instance()
    {
        static MaterialManager instance;
        return instance;
    }

    /**
     * @brief Deleted copy constructor to prevent copying of the singleton
     */
    MaterialManager(const MaterialManager&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying of the singleton
     */
    MaterialManager& operator=(const MaterialManager&) = delete;

    /**
     * @brief Deleted move constructor to prevent moving of the singleton
     */
    MaterialManager(MaterialManager&&) = delete;

    /**
     * @brief Deleted move assignment operator to prevent moving of the singleton
     */
    MaterialManager& operator=(MaterialManager&&) = delete;

    /**
     * @brief Adds a new material of type T to the manager.
     *
     * @tparam T The material type, derived from Material.
     * @param name Unique name of the material.
     * 
     * @return Pointer to the created or existing material casted to type T.
     *
     * If a material with the given name already exists, it attempts to cast it
     * to the requested type T and returns it. Otherwise, it creates a new material
     * of type T, stores it internally, and returns a pointer to it.
     */
    template<typename T>
    T* add_material(const std::string& name)
    {
        // Si ya existe, intentamos castear al tipo solicitado
        if (exists(name)) {
            return dynamic_cast<T*>(m_materials[name].get());
        }

        // Crear el material
        auto mat = std::make_unique<T>(name);
        T* raw = mat.get();

        // Guardarlo como Material
        m_materials[name] = std::move(mat);

        return raw;
    }

    /**
     * @brief Retrieves a material by its name
     *
     * @param name Name of the material to retrieve
     * 
     * @return Pointer to the Material object if it exists, nullptr otherwise
     */
    Material* get_material(const std::string& name);

    /**
     * @brief Gets all the materials loaded in the manager
     *
     * @return A map with all the materials. For each one, the key is the material name
     * and the value is a pointer to the Material object
     */
    const std::unordered_map<std::string, std::unique_ptr<Material>>& get_all_materials() const { return m_materials; }

    /**
     * @brief Retrieves the type of an Assimp material.
     *
     * @param mat Pointer to an aiMaterial object from Assimp.
     * 
     * @return MaterialType corresponding to the imported material.
     *
     * This function is used during model import to map Assimp materials
     * to the engine's MaterialType enumeration.
     */
    MaterialType get_material_type(const aiMaterial* mat);

    /**
     * @brief Checks if a maetrial with the given name exists in the manager
     *
     * @param name Name of the material to check
     *
     * @return true if the material exists, false otherwise
     */
    bool exists(const std::string& name) const
    {
        return m_materials.find(name) != m_materials.end();
    }

private:

    // Private constructor and destructor to enforce singleton pattern
    MaterialManager() = default;
    ~MaterialManager() = default;

    // Unique name of the material
    std::string m_name;

    // Map storing materials by their unique ID
    // Uses unique_ptr to ensure proper memory management
    std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
};

