#include "MaterialManager.h"

Material* MaterialManager::get_material(const std::string& name)
{
    if (!exists(name)) 
        return nullptr;
    
    return m_materials[name].get();
}

MaterialType MaterialManager::get_material_type(const aiMaterial* mat)
{
    // Check PBR attributes
    /*if (mat->GetTextureCount(aiTextureType_METALNESS) > 0) return MaterialType::PBR;
    if (mat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0) return MaterialType::PBR;

    float metallic, roughness;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_METALLIC_FACTOR, metallic)) return MaterialType::PBR;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness)) return MaterialType::PBR;*/

    // Check Phong
    /*aiColor3D diffuse, specular;
    float shininess;

    if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse)) return MaterialType::Phong;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, specular)) return MaterialType::Phong;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_SHININESS, shininess)) return MaterialType::Phong;*/

    // Generic case
    /*return MaterialType::Generic;*/

    // Only phong supported now
    return MaterialType::Phong;
}
