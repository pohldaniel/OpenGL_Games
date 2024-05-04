// For conditions of distribution and use, see copyright notice in License.txt

#pragma once

#include <Octree/AutoPtr.h>
#include "../Object/Ptr.h"
#include "../Math/IntRect.h"
#include "GraphicsDefs.h"

#include <vector>

class RenderBuffer;
class TextureTu;

/// GPU framebuffer object for rendering. Combines color and depth-stencil textures or buffers.
class FrameBufferTu : public RefCounted
{
public:
    /// Construct. Graphics subsystem must have been initialized.
	FrameBufferTu();
    /// Destruct.
    ~FrameBufferTu();

    /// Define renderbuffers to render to. Leave buffer(s) null for color-only or depth-only rendering.
    void Define(RenderBuffer* colorBuffer, RenderBuffer* depthStencilBuffer);
    /// Define textures to render to. Leave texture(s) null for color-only or depth-only rendering.
    void Define(TextureTu* colorTexture, TextureTu* depthStencilTexture);
    /// Define cube map face to render to.
    void Define(TextureTu* colorTexture, size_t cubeMapFace, TextureTu* depthStencilTexture);
    /// Define MRT textures to render to.
    void Define(const std::vector<TextureTu*>& colorTextures, TextureTu* depthStencilTexture);
    /// Bind as draw framebuffer. No-op if already bound. Used also when defining.
    void Bind();

    /// Return the OpenGL object identifier.
    unsigned GLBuffer() const { return buffer; }

    /// Bind separate framebuffers for drawing and reading.
    static void Bind(FrameBufferTu* draw, FrameBufferTu* read);
    /// Unbind the current draw and read framebuffers and return to backbuffer rendering.
    static void Unbind();

private:
    /// Release the framebuffer object.
    void Release();

    /// OpenGL buffer object identifier.
    unsigned buffer;
};
