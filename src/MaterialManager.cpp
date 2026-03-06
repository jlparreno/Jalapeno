#include "MaterialManager.h"

Material* MaterialManager::get_material(const std::string& name)
{
    if (!exists(name)) 
        return nullptr;
    
    return m_materials[name].get();
}

MaterialType MaterialManager::get_material_type(const aiMaterial* mat)
{
    // Check PBR attributes: metallic o roughness
    float metallic = 0.0f;
    float roughness = 0.0f;

    bool has_metallic = mat->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS;
    bool has_roughness = mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS;

    // If there is any PBR attribute, then Material is PBRMaterial
    if (has_metallic || has_roughness)
        return MaterialType::PBR;

    // Phong is the default
    return MaterialType::Phong;
}
