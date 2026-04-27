#pragma once

#include <glad/glad.h>

/**
 * @class PipelineState
 *
 * @brief GPU pipeline state for a render pass
 *
 * Groups all mutable OpenGL state that each pass needs into a single object.
 * Calling apply() sets the full state in one place, replacing scattered
 * glEnable/glDisable/glCullFace calls that previously lived inside each pass.
 *
 * Each render pass should declare its state as a static const member.
 */
class PipelineState
{
public:

    // --- Setters (return *this for method chaining) ---

    PipelineState& set_depth_test(bool v)    { m_depth_test  = v;   return *this; }
    PipelineState& set_depth_write(bool v)   { m_depth_write = v;   return *this; }
    PipelineState& set_depth_func(GLenum v)  { m_depth_func  = v;   return *this; }
    PipelineState& set_cull_face(bool v)     { m_cull_face   = v;   return *this; }
    PipelineState& set_cull_mode(GLenum v)   { m_cull_mode   = v;   return *this; }
    PipelineState& set_polygon_mode(GLenum v){ m_polygon_mode = v;  return *this; }

    /**
     * @brief Applies this state to the OpenGL context
     *
     * Sets all fields unconditionally. Each pass should call this at the
     * start of execute() so state from previous passes is always overwritten.
     */
    void apply() const;

    /**
     * @brief Enables or disables the global wireframe override
     *
     * When active, all passes render in wireframe regardless of their
     * polygon_mode. Called by Renderer::set_wireframe().
     *
     * @param enabled True to force wireframe, false to restore per-pass polygon_mode
     */
    static void set_wireframe_override(bool enabled);

private:

    // Depth
    bool   m_depth_test  { true };
    bool   m_depth_write { true };
    GLenum m_depth_func  { GL_LESS };

    // Rasterization
    bool   m_cull_face    { true };
    GLenum m_cull_mode    { GL_BACK };
    GLenum m_polygon_mode { GL_FILL };

    // Global wireframe override
    static bool s_wireframe_override;
};
