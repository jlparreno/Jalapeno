#include "Light.h"

Light::Light(const std::string& name, const glm::vec3& color, float intensity) :
	m_name(name), 
	m_color(color), 
	m_intensity(intensity),
	m_shadows_enabled(false)
{
}

void Light::set_shadows_enabled(bool value) 
{ 
	m_shadows_enabled = value; 

	// If not initialized, init shadow map FBO for this light
	if (!m_shadow_fbo)
		init_shadow_map(SHADOWMAPS_SIZE);
}
