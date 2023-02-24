#pragma once

#include <GL/glew.h>
#include "engine/Shader.h"
#include "engine/Quad.h"
#include "engine/Framebuffer.h"

#define FB_SS 0
class AABox {
public:
	AABox();
	~AABox();

	bool Initialize(int w, int h, float ssfact, int depthSamples, int coverageSamples);
	void Destroy();

	void Activate(int x = 0, int y = 0);
	void Deactivate();
	void Draw(int technique);

protected:
	bool          bValid;
	bool          bCSAA;

	int           vpx, vpy, vpw, vph;
	int           posx, posy;
	int           bufw, bufh;

	
	GLuint        textureID;
	GLuint        textureDepthID;
	
	GLuint        fb;
	GLuint        fbms;
	GLuint        depth_rb;
	GLuint        color_rb;

	Shader* prog[3];
	Quad* m_quad;
	Framebuffer m_buffer;

	bool          initRT(int depthSamples, int coverageSamples);
};