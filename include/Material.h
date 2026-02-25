#pragma once

#include <glm/glm.hpp>

#include "ShaderProgram.h"
#include "TextureManager.h"

#include <string>
#include <unordered_map>


class Material
{
public:

    Material(const std::string& name, ShaderProgram* shader);

    void load_textures(const std::vector<TextureInfo>& textures);

    void bind_textures() const;

    const std::string& get_name() const { return m_name; }

    ShaderProgram* get_shader() const { return m_shader; }

    void apply_uniforms();

private:

    std::string     m_name;

    ShaderProgram*  m_shader;

    std::unordered_map<std::string, Texture*> m_textures;
};