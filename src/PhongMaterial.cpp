#include "PhongMaterial.h"

PhongMaterial::PhongMaterial(const std::string& name) : 
	Material(name, MaterialType::Phong), 
	m_shininess(32.0f)
{
}

void PhongMaterial::apply_uniforms(ShaderProgram* shader) const
{
	shader->set_uniform("material.shininess", m_shininess);
}