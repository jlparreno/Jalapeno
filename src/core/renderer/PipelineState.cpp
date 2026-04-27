#include "PipelineState.h"

bool PipelineState::s_wireframe_override = false;

void PipelineState::apply() const
{
    // Depth test
    m_depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

    // Depth write
    glDepthMask(m_depth_write ? GL_TRUE : GL_FALSE);

    // Depth function
    glDepthFunc(m_depth_func);

    // Face culling
    if (m_cull_face)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(m_cull_mode);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }

    // Polygon mode: wireframe override takes precedence
    glPolygonMode(GL_FRONT_AND_BACK, s_wireframe_override ? GL_LINE : m_polygon_mode);
}

void PipelineState::set_wireframe_override(bool enabled)
{
    s_wireframe_override = enabled;
}
