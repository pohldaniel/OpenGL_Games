// For conditions of distribution and use, see copyright notice in License.txt

#include "../IO/Log.h"
#include "FrameBufferTu.h"
#include "Graphics.h"
#include "RenderBuffer.h"
#include "TextureTu.h"

#include <GL/glew.h>
#include <tracy/Tracy.hpp>

static FrameBufferTu* boundDrawBuffer = nullptr;
static FrameBufferTu* boundReadBuffer = nullptr;

FrameBufferTu::FrameBufferTu()
{
    assert(ObjectTu::Subsystem<Graphics>()->IsInitialized());

    glGenFramebuffers(1, &buffer);
}

FrameBufferTu::~FrameBufferTu()
{
    // Context may be gone at destruction time. In this case just no-op the cleanup
    if (ObjectTu::Subsystem<Graphics>())
        Release();
}

void FrameBufferTu::Define(RenderBuffer* colorBuffer, RenderBuffer* depthStencilBuffer)
{
    ZoneScoped;

    Bind();

    IntVector2 size = IntVector2::ZERO;

    if (colorBuffer)
    {
        size = colorBuffer->Size();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer->GLBuffer());
    }
    else
    {
        glDrawBuffer(GL_NONE);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
    }

    if (depthStencilBuffer)
    {
        if (size != IntVector2::ZERO && size != depthStencilBuffer->Size())
            LOGWARNING("Framebuffer color and depth dimensions don't match");
        else
            size = depthStencilBuffer->Size();

        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthStencilBuffer->GLBuffer());
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilBuffer->Format() == FMT_D24S8 ? depthStencilBuffer->GLBuffer() : 0);
    }
    else
    {
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
    }

    LOGDEBUGF("Defined framebuffer width %d height %d", size.x, size.y);
}

void FrameBufferTu::Define(TextureTu* colorTexture, TextureTu* depthStencilTexture)
{
    ZoneScoped;

    Bind();

    IntVector2 size = IntVector2::ZERO;

    if (colorTexture && colorTexture->TexType() == TEX_2D)
    {
        size = colorTexture->Size2D();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture->GLTexture(), 0);
    }
    else
    {
        glDrawBuffer(GL_NONE);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    }

    if (depthStencilTexture)
    {
        if (size != IntVector2::ZERO && size != depthStencilTexture->Size2D())
            LOGWARNING("Framebuffer color and depth dimensions don't match");
        else
            size = depthStencilTexture->Size2D();

        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthStencilTexture->GLTexture(), 0);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTexture->Format() == FMT_D24S8 ? depthStencilTexture->GLTexture() : 0, 0);
    }
    else
    {
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    }

    LOGDEBUGF("Defined framebuffer width %d height %d", size.x, size.y);
}

void FrameBufferTu::Define(TextureTu* colorTexture, size_t cubeMapFace, TextureTu* depthStencilTexture)
{
    ZoneScoped;

    Bind();

    IntVector2 size = IntVector2::ZERO;

    if (colorTexture && colorTexture->TexType() == TEX_CUBE)
    {
        size = colorTexture->Size2D();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)cubeMapFace, colorTexture->GLTexture(), 0);
    }
    else
    {
        glDrawBuffer(GL_NONE);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    }

    if (depthStencilTexture)
    {
        if (size != IntVector2::ZERO && size != depthStencilTexture->Size2D())
            LOGWARNING("Framebuffer color and depth dimensions don't match");
        else
            size = depthStencilTexture->Size2D();

        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthStencilTexture->GLTexture(), 0);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTexture->Format() == FMT_D24S8 ? depthStencilTexture->GLTexture() : 0, 0);
    }
    else
    {
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    }

    LOGDEBUGF("Defined framebuffer width %d height %d from cube texture", size.x, size.y);
}

void FrameBufferTu::Define(const std::vector<TextureTu*>& colorTextures, TextureTu* depthStencilTexture)
{
    ZoneScoped;

    Bind();

    IntVector2 size = IntVector2::ZERO;

    std::vector<GLenum> drawBufferIds;
    for (size_t i = 0; i < colorTextures.size(); ++i)
    {
        if (colorTextures[i] && colorTextures[i]->TexType() == TEX_2D)
        {
            if (size != IntVector2::ZERO && size != colorTextures[i]->Size2D())
                LOGWARNING("Framebuffer color dimensions don't match");
            else
                size = colorTextures[i]->Size2D();

            drawBufferIds.push_back(GL_COLOR_ATTACHMENT0 + (GLenum)i);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (GLenum)i, GL_TEXTURE_2D, colorTextures[i]->GLTexture(), 0);
        }
        else
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (GLenum)i, GL_TEXTURE_2D, 0, 0);
    }

    if (drawBufferIds.size())
        glDrawBuffers((GLsizei)drawBufferIds.size(), &drawBufferIds[0]);
    else
        glDrawBuffer(GL_NONE);

    if (depthStencilTexture)
    {
        if (size != IntVector2::ZERO && size != depthStencilTexture->Size2D())
            LOGWARNING("Framebuffer color and depth dimensions don't match");
        else
            size = depthStencilTexture->Size2D();

        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthStencilTexture->GLTexture(), 0);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTexture->Format() == FMT_D24S8 ? depthStencilTexture->GLTexture() : 0, 0);
    }
    else
    {
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    }

    LOGDEBUGF("Defined MRT framebuffer width %d height %d", size.x, size.y);
}

void FrameBufferTu::Bind()
{
    if (!buffer || boundDrawBuffer == this)
        return;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer);
    boundDrawBuffer = this;
}

void FrameBufferTu::Bind(FrameBufferTu* draw, FrameBufferTu* read)
{
    if (boundDrawBuffer != draw)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw ? draw->buffer : 0);
        boundDrawBuffer = draw;
    }

    if (boundReadBuffer != read)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, read ? read->buffer : 0);
        boundReadBuffer = read;
    }
}

void FrameBufferTu::Unbind()
{
    if (boundDrawBuffer)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        boundDrawBuffer = nullptr;
    }
    if (boundReadBuffer)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        boundReadBuffer = nullptr;
    }
}

void FrameBufferTu::Release()
{
    if (buffer)
    {
        if (boundDrawBuffer == this || boundReadBuffer == this)
			FrameBufferTu::Unbind();

        glDeleteFramebuffers(1, &buffer);
        buffer = 0;
    }
}
