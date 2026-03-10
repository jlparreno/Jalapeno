#pragma once

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "types.h"
#include <vector>

/**
 * @class Framebuffer
 *
 * @brief OpenGL Framebuffer Object (FBO) supporting multiple attachment configurations
 *
 * Supports color attachments, depth attachments, and multisample (MSAA) 
 * framebuffers. The configuration is fully defined by a FramebufferSpec 
 * passed at construction time.
 *
 * Spec example (Color + Depth):
 * 
 * FramebufferSpec spec;
 * spec.width = 1920; spec.height = 1080; spec.samples = 4;
 * spec.attachments = {
 *     { GL_RGBA16F,            GL_COLOR_ATTACHMENT0  },
 *     { GL_DEPTH24_STENCIL8,   GL_DEPTH_STENCIL_ATTACHMENT }
 * };
 */
class Framebuffer
{
public:

    /**
     * @brief Constructs and initializes the Framebuffer from the given specification
     * 
     * @param spec Full configuration of the framebuffer
     */
    Framebuffer(const FramebufferSpec& spec);

    /**
     * @brief Destroys the Framebuffer and releases all GPU resources.
     */
    ~Framebuffer();

    /**
      * @brief Recreates the framebuffer with a new size, keeping the same spec
      * 
      * @param width  New width in pixels
      * @param height New height in pixels
      */
    void resize(int width, int height);

    /**
     * @brief Binds the framebuffer as the current render target
     */
    void bind() const;

    /**
     * @brief Binds the depth attachment of this framebuffer as a texture to the given slot
     *
     * Activates the specified texture unit and binds the depth attachment texture,
     * making it available for sampling in a shader.
     *
     * @param slot Texture unit index to bind the depth attachment to
     */
    void bind_depth_as_texture(int slot) const;

    /**
     * @brief Binds a color attachment of this framebuffer as a texture to the given slot
     *
     * Activates the specified texture unit and binds the color attachment at the
     * given index, making it available for sampling in a shader.
     *
     * @param index Index of the color attachment to bind
     * @param slot  Texture unit index to bind the color attachment to
     */
    void bind_color_as_texture(int index, int slot) const;

    /**
     * @brief Unbinds this framebuffer, restoring the default framebuffer
     */
    void unbind();

    /**
     * @brief Returns the OpenGL FBO ID
     *
     * @return FBO ID or 0 if this FBO is not created
     */
    GLuint get_ID() const { return m_fbo; };

    /**
     * @brief Returns a color attachment texture by index
     * 
     * @param index Index of the color attachment
     * 
     * @return OpenGL texture ID, or 0 if index is out of range
     */
    GLuint get_color_attachment(int index = 0) const;

    /**
     * @brief Returns the depth attachment texture
     * 
     * @return OpenGL texture ID, or 0 if no depth attachment exists
     */
    GLuint get_depth_attachment() const { return m_depth_attachment; }

    /**
     * @brief Returns the width of the framebuffer in pixels
     * 
     * @return Current framebuffer width
     */
    int get_width()   const { return m_spec.width; }

    /**
     * @brief Returns the height of the framebuffer in pixels
     * 
     * @return Current framebuffer height
     */
    int get_height()  const { return m_spec.height; }

    /**
     * @brief Returns the number of samples used for multisampling
     *
     * A value of 1 indicates no multisampling. Values greater than 1
     * indicate MSAA with the given sample count.
     *
     * @return Number of samples per pixel
     */
    int get_samples() const { return m_spec.samples; }

private:

    /**
     * @brief Creates the OpenGL framebuffer and all its attachments from the current spec
     *
     * Calls destroy() first to release any existing GPU resources, then allocates
     * a new FBO and iterates the attachment list in the spec to create each one.
     * Verifies framebuffer completeness after all attachments are created.
     */
    void create();

    /**
     * @brief Releases all GPU resources associated with this framebuffer
     *
     * Deletes the FBO and all color and depth attachment textures. Safe to call
     * even if the framebuffer has not been initialized. Called automatically
     * by create() and the destructor.
     */
    void destroy();

    /**
     * @brief Creates and attaches a single texture attachment to the framebuffer
     *
     * Allocates an OpenGL texture based on the given AttachmentSpec, configures
     * its format, filtering, and wrapping parameters, and attaches it to the
     * currently bound FBO.
     *
     * @param attachment_spec Specification of the attachment to create
     */
    void create_attachment(const AttachmentSpec& attachment_spec);

    
    // Full configuration of this framebuffer
    FramebufferSpec m_spec;

    // OpenGL handle to the framebuffer object (FBO)
    GLuint m_fbo{ 0 };

    // OpenGL texture handles for all color attachments, indexed by attachment order
    std::vector<GLuint> m_color_attachments;

    // OpenGL texture handle for the depth or depth-stencil attachment
    GLuint m_depth_attachment{ 0 };
};
