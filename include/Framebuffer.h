#pragma once

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <vector>

/**
 * @class Framebuffer
 * 
 * @brief Wrapper for an OpenGL FBO
 *
 * This class encapsulates the creation, configuration and destruction of
 * an OpenGL framebuffer (FBO) and its attachments. It supports a single
 * color attachment and a depth-stencil attachment, but can be extended
 * for more advanced pipelines if needed.
 *
 * The framebuffer automatically recreates its attachments when resized and
 * ensures proper cleanup of OpenGL resources on destruction.
 */
class Framebuffer
{
public:

    /**
     * @brief Creates a framebuffer with the given dimensions.
     * 
     * @param width Width of the framebuffer in pixels.
     * @param height Height of the framebuffer in pixels.
     * @param samples Num of samples of the framebuffer (MSAA)
     */
    Framebuffer(int width, int height, int samples = 1);

    /**
     * @brief Destroys the framebuffer and all its attachments.
     */
    ~Framebuffer();

    /**
     * @brief Creates or recreates the framebuffer and attachments.
     * 
     * @param width New width in pixels.
     * @param height New height in pixels.
     * @param samples Num of samples of the framebuffer (MSAA)
     *
     * If the framebuffer already existed, it is destroyed first.
     */
    void create(int width, int height, int samples = 1);

    /**
     * @brief Resizes the framebuffer, recreating all attachments.
     * 
     * @param width New width in pixels.
     * @param height New height in pixels.
     */
    void resize(int width, int height);

    /**
     * @brief Binds the framebuffer for rendering.
     */
    void bind() const;

    /**
     * @brief Unbinds any framebuffer, binding the default framebuffer (0).
     */
    static void unbind();

    /**
     * @brief Returns the OpenGL FBO ID.
     *
     * @return FBO ID or 0 if this FBO is not created
     */
    GLuint get_ID() const { return m_fbo; };

    /**
     * @brief Returns the OpenGL texture ID of a color attachment.
     * 
     * @param index Index of the color attachment (default: 0).
     * @return Texture ID or 0 if index is invalid.
     */
    GLuint get_color_attachment(int index = 0) const;

    /**
     * @brief Returns the depth-stencil attachment texture ID.
     */
    GLuint get_depth_attachment() const { return m_depth_attachment; }

    /**
     * @brief Returns the framebuffer width in pixels.
     */
    int get_width() const { return m_width; }

    /**
     * @brief Returns the framebuffer height in pixels.
     */
    int get_height() const { return m_height; }

private:

    /**
     * @brief Deletes the framebuffer and all its attachments.
     */
    void destroy();

    /**
     * @brief Creates the color and depth-stencil attachments
     * 
     * @param samples Num of samples of the attachments (MSAA).
     *
     * The attachments are allocated with floating HDR color format (RGBA16F) and depth-stencil format (DEPTH24_STENCIL8).
     * By default, simple attachments without MSAA are created. If samples > 1 then MSAA attachments are created.
     */
    void create_attachments(int samples = 1);

    // Framebuffer Object ID
    GLuint m_fbo = 0;

    // Color texture attachments
    std::vector<GLuint> m_color_attachments;

    // Depth-stencil attachment
    GLuint m_depth_attachment = 0;

    // Width in pixels
    int m_width = 0;

    // Height in pixels
    int m_height = 0;
};
