#include "Framebuffer.h"

Framebuffer::Framebuffer(int width, int height, int samples)
{
    create(width, height, samples);
}

Framebuffer::~Framebuffer()
{
    destroy();
}

void Framebuffer::create(int width, int height, int samples)
{
    destroy(); // In case it already existed

    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    create_attachments(samples);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("Framebuffer is not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(int width, int height)
{
    if (width == m_width && height == m_height)
        return;

    SPDLOG_INFO("Resizing FBO...");
    create(width, height);
}

void Framebuffer::destroy()
{
    if (m_fbo)
    {
        glDeleteFramebuffers(1, &m_fbo);
    }

    for (GLuint tex : m_color_attachments)
    {
        glDeleteTextures(1, &tex);
    }

    if (m_depth_attachment)
    {
        glDeleteTextures(1, &m_depth_attachment);
    }

    m_color_attachments.clear();
    m_depth_attachment = 0;
}

void Framebuffer::create_attachments(int samples)
{
    auto texture_mode = samples>1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    // Color Attachment
    GLuint color_texture;
    glGenTextures(1, &color_texture);
    glBindTexture(texture_mode, color_texture);

    if (samples > 1)
    {
        glTexImage2DMultisample(texture_mode, samples, GL_RGBA16F, m_width, m_height, GL_TRUE);
    }
    else
    {
        glTexImage2D(texture_mode, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
    }

    glTexParameteri(texture_mode, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture_mode, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_mode, color_texture, 0);
    m_color_attachments.push_back(color_texture);

    // Depth-Stencil Attachment
    glGenTextures(1, &m_depth_attachment);
    glBindTexture(texture_mode, m_depth_attachment);

    if (samples > 1)
    {
        glTexImage2DMultisample(texture_mode, samples, GL_DEPTH24_STENCIL8, m_width, m_height, GL_TRUE);
    }
    else
    {
        glTexImage2D(texture_mode, 0, GL_DEPTH24_STENCIL8, m_width, m_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    }
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, texture_mode, m_depth_attachment, 0);
}

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
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
