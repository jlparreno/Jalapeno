#include "PhongMaterial.h"

PhongMaterial::PhongMaterial(const std::string& name, ShaderProgram* shader) : 
	Material(name, shader),
	m_shininess(32.0f)
{
}

void PhongMaterial::apply_uniforms() const
{
	m_shader->set_uniform("material.shininess", m_shininess);
}