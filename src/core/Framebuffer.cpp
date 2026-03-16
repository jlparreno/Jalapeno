#include "Framebuffer.h"

Framebuffer::Framebuffer(const FramebufferSpec& spec) : m_spec(spec)
{
    create();
}

Framebuffer::~Framebuffer()
{
    destroy();
}

void Framebuffer::create()
{
    destroy();

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    auto texture_target = m_spec.samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    for (const auto& attachment : m_spec.attachments)
    {
        create_attachment(attachment);
    }

    // Tell OpenGL which color attachments to draw to
    if (!m_color_attachments.empty())
    {
        std::vector<GLenum> draw_buffers;
        for (int i = 0; i < (int)m_color_attachments.size(); i++)
        {
            draw_buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
        glDrawBuffers(static_cast<GLsizei>(draw_buffers.size()), draw_buffers.data());
    }
    else
    {
        // Depth-only FBO, no color output
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("Framebuffer is not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::create_attachment(const AttachmentSpec& attachment_spec)
{
    bool is_depth = (attachment_spec.attachment_point == GL_DEPTH_ATTACHMENT ||
                     attachment_spec.attachment_point == GL_DEPTH_STENCIL_ATTACHMENT);

    // Default type
    GLenum texture_target = GL_TEXTURE_2D;
    
    // Change type depending on spec
    if (m_spec.samples > 1)
    {
        texture_target = GL_TEXTURE_2D_MULTISAMPLE;
    }
    else
    {
        if (m_spec.is_cubemap)
            texture_target = GL_TEXTURE_CUBE_MAP;
    }

    // Choose format and data type depending on internal format
    GLenum format = is_depth ? GL_DEPTH_COMPONENT : GL_RGBA;
    GLenum data_type = GL_FLOAT;
    GLenum filter = is_depth ? GL_NEAREST : GL_LINEAR;

    if (attachment_spec.attachment_point == GL_DEPTH_STENCIL_ATTACHMENT)
    {
        format = GL_DEPTH_STENCIL;
        data_type = GL_UNSIGNED_INT_24_8;
    }

    // Create texture/s
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(texture_target, texture);

    if (m_spec.is_cubemap)
    {
        // Create the 6 faces of the cubemap
        for (int i = 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, attachment_spec.internal_format, m_spec.width, m_spec.height, 0, format, data_type, nullptr);
        }
        glTexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(texture_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glFramebufferTexture(GL_FRAMEBUFFER, attachment_spec.attachment_point, texture, 0);
    }
    else
    {
        if (m_spec.samples > 1)
        {
            glTexImage2DMultisample(texture_target, m_spec.samples, attachment_spec.internal_format, m_spec.width, m_spec.height, GL_TRUE);
        }
        else
        {
            glTexImage2D(texture_target, 0, attachment_spec.internal_format, m_spec.width, m_spec.height, 0, format, data_type, nullptr);

            glTexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, filter);
            glTexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, filter);

            // Shadow maps: clamp to border to avoid shadows outside the frustum
            if (is_depth)
            {
                glTexParameteri(texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                float border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
                glTexParameterfv(texture_target, GL_TEXTURE_BORDER_COLOR, border);
            }
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_spec.attachment_point, texture_target, texture, 0);
    }

    // Save created attachment texture ID
    if (is_depth)
        m_depth_attachment = texture;
    else
        m_color_attachments.push_back(texture);
}

void Framebuffer::destroy()
{
    if (m_fbo)
    {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }

    for (GLuint tex : m_color_attachments)
    {
        glDeleteTextures(1, &tex);
    }

    m_color_attachments.clear();

    if (m_depth_attachment)
    {
        glDeleteTextures(1, &m_depth_attachment);
        m_depth_attachment = 0;
    }
}

void Framebuffer::resize(int width, int height)
{
    // This call is ignored if FBO is not resizable
    if (!m_spec.resizable)
        return;

    if (width == m_spec.width && height == m_spec.height)
        return;

    SPDLOG_INFO("Resizing FBO to {}x{}", width, height);
    m_spec.width = width;
    m_spec.height = height;

    create();
}

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_spec.width, m_spec.height);
}

void Framebuffer::bind_depth_as_texture(int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    GLenum target = m_spec.is_cubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
    glBindTexture(target, m_depth_attachment);
}

void Framebuffer::bind_color_as_texture(int index, int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, get_color_attachment(index));
}

void Framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Framebuffer::get_color_attachment(int index) const
{
    if (index < 0 || index >= (int)m_color_attachments.size())
    {
        SPDLOG_WARN("No color attachment with index {}", index);
        return 0;
    }

    return m_color_attachments[index];
}
