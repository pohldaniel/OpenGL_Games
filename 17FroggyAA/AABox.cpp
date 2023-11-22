#include <stdio.h>
#include <iostream>

#include "AABox.h"

AABox::AABox() : bValid(false), vpx(0), vpy(0), vpw(0), vph(0), posx(0), posy(0) {
	m_quad = new Quad();

	prog[0] = new Shader("res/down.vert", "res/down1.frag");
	prog[1] = new Shader("res/down.vert", "res/down2.frag");
	prog[2] = new Shader("res/down.vert", "res/down3.frag");

}

AABox::~AABox() { }

void AABox::Destroy() {
	if (depth_rb)
		glDeleteRenderbuffersEXT(1, &depth_rb);
	if (color_rb)
		glDeleteRenderbuffersEXT(1, &color_rb);
	if (textureID)
		glDeleteTextures(1, &textureID);
	if (textureDepthID)
		glDeleteTextures(1, &textureDepthID);
	if (fb)
		glDeleteFramebuffersEXT(1, &fb);
	if (fbms)
		glDeleteFramebuffersEXT(1, &fbms);
	
	depth_rb = 0;
	color_rb = 0;
	textureID = 0;
	textureDepthID = 0;
	fb = 0;
	fbms = 0;
}

void CheckFramebufferStatus(){
	GLenum status;
	status = (GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
		printf("Unsupported framebuffer format\n");
		fprintf(stderr, "Unsupported framebuffer format");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
		printf("Framebuffer incomplete, missing attachment\n");
		fprintf(stderr, "Framebuffer incomplete, missing attachment");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		printf("Framebuffer incomplete, attached images must have same dimensions\n");
		fprintf(stderr, "Framebuffer incomplete, attached images must have same dimensions");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		printf("Framebuffer incomplete, attached images must have same format\n");
		fprintf(stderr, "Framebuffer incomplete, attached images must have same format");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
		printf("Framebuffer incomplete, missing draw buffer\n");
		fprintf(stderr, "Framebuffer incomplete, missing draw buffer");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
		printf("Framebuffer incomplete, missing read buffer\n");
		fprintf(stderr, "Framebuffer incomplete, missing read buffer");
		break;
	default:
		printf("Error %x\n", status);
		break;
	}
}

bool AABox::initRT(int depthSamples, int coverageSamples) {
	bool multisample = depthSamples > 0;
	bool csaa = coverageSamples > depthSamples;
	bool ret = true;
	int query;
	//
	// init the texture that will also be the buffer to render to
	//
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bufw, bufh, 0, GL_RGBA, GL_FLOAT, 0);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffersEXT(1, &fb);

	//
	// Handle multisample FBO's first
	//
	if (multisample) {
		//multisample : so we need to resolve from the MS FBO down to a FBO at non MS resolution
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureID, 0);
		CheckFramebufferStatus();

		//now handle the FBO in MS resolution
		glGenFramebuffersEXT(1, &fbms);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbms);
		// initialize color renderbuffer
		glGenRenderbuffersEXT(1, &color_rb);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, color_rb);

		if (csaa & bCSAA) {
			glRenderbufferStorageMultisampleCoverageNV(GL_RENDERBUFFER_EXT, coverageSamples, depthSamples, GL_RGBA8,
				bufw, bufh);

			glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_COVERAGE_SAMPLES_NV, &query);

			if (query < coverageSamples)
				ret = false;
			else if (query > coverageSamples) {
				// report back the actual number
				coverageSamples = query;
			}

			glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_COLOR_SAMPLES_NV, &query);

			if (query < depthSamples)
				ret = false;
			else if (query > depthSamples)
			{
				// report back the actual number
				depthSamples = query;
			}
		} else {

			// create a regular MSAA color buffer
			glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, depthSamples, GL_RGBA8, bufw, bufh);

			// check the number of samples
			glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_SAMPLES_EXT, &query);

			if (query < depthSamples)
				ret = false;
			else if (query > depthSamples) {
				depthSamples = query;
			}

		}

		// attach the multisampled color buffer
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, color_rb);
		CheckFramebufferStatus();

		// bind the multisampled depth buffer
		glGenRenderbuffersEXT(1, &depth_rb);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);

		// create the multisampled depth buffer (with or without coverage sampling)
		if (csaa & bCSAA) {

			// create a coverage sampled MSAA depth buffer
			glRenderbufferStorageMultisampleCoverageNV(GL_RENDERBUFFER_EXT, coverageSamples, depthSamples, GL_DEPTH_COMPONENT24,
				bufw, bufh);

			// check the number of coverage samples
			glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_COVERAGE_SAMPLES_NV, &query);

			if (query < coverageSamples)
				ret = false;
			else if (query > coverageSamples)
				// set the coverage samples value to return the actual value
				coverageSamples = query;

			// cehck the number of stored color samples (same as depth samples)
			glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_COLOR_SAMPLES_NV, &query);

			if (query < depthSamples)
				ret = false;
			else if (query > depthSamples)
				// set the depth samples value to return the actual value
				depthSamples = query;
		}else {

			// create a regular (not coverage sampled) MSAA depth buffer
			glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, depthSamples, GL_DEPTH_COMPONENT24, bufw, bufh);

			// check the number of depth samples
			glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_SAMPLES_EXT, &query);

			if (query < depthSamples)
				ret = false;
			else if (query < depthSamples)
				depthSamples = query;
		}

		// attach the depth buffer
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb);

		CheckFramebufferStatus();
	// if (multisample)
	// Depth buffer created as a texture...
	}else {
		//non-multisample, so bind things directly to the FBO
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureID, 0);

		glGenTextures(1, &textureDepthID);
		glBindTexture(GL_TEXTURE_2D, textureDepthID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, bufw, bufh, 0,
			GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, textureDepthID, 0);

		CheckFramebufferStatus();
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, multisample ? fbms : fb);
	glGetIntegerv(GL_RED_BITS, &query);
	if (query != 8) {
		printf("Got %d red bits expected %d\n", query, 8);
		ret = false;
	}
	glGetIntegerv(GL_DEPTH_BITS, &query);

	if (query != 24) {
		printf("Got %d depth bits expected %d\n", query, GL_DEPTH_COMPONENT24);;
		ret = false;
	}

	if (multisample) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
		glGetIntegerv(GL_RED_BITS, &query);
		if (query != 8) {
			printf("Got %d red bits expected %d\n", query, 8);
			ret = false;
		}
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	return ret;
}

/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
bool AABox::Initialize(int w, int h, float ssfact, int depthSamples, int coverageSamples) {
	if (!glewIsSupported( "GL_VERSION_2_0 " "GL_EXT_framebuffer_object " )) {
		printf("Unable to load extensions\n");
		return false;
	}

	bCSAA = glewIsExtensionSupported("GL_NV_framebuffer_multisample_coverage") ? true : false;
	if ((!bCSAA) && coverageSamples) {
		printf("Note: GL_NV_framebuffer_multisample_coverage unavailable, using regular MSAA only\n");
	}

	Destroy();

	bufw = (int)(ssfact*(float)w);
	bufh = (int)(ssfact*(float)h);
	//
	// FBO
	//
	initRT(depthSamples, coverageSamples);

	CheckFramebufferStatus();
	
	glUseProgram(prog[1]->m_program);
	prog[1]->loadVector("SSTexelSize", Vector2f(1.0f / (float)bufw, 1.0f / (float)bufh));
	glUseProgram(0);

	glUseProgram(prog[2]->m_program);
	prog[2]->loadVector("SSTexelSize", Vector2f(1.0f / (float)bufw, 1.0f / (float)bufh));
	glUseProgram(0);
	
	bValid = true;
	return true;
}

/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
void AABox::Draw(int technique) {

	if (technique > 4)
		return;
	else  bValid = true;
	if (!bValid)
		return;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//
	// if this FBO is multisampled, resolve it, so it can be displayed
	// the blit will allow the multisampled buffer to be stretched to a normal buffer at res bufw/bufh
	//
	if (fbms){
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, fbms);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, fb);
		glBlitFramebufferEXT(0, 0, bufw, bufh, 0, 0, bufw, bufh, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glUseProgram(prog[technique]->m_program);

	
	m_quad->draw(textureID);
	glUseProgram(0);
}

/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
void AABox::Activate(int x, int y) {
	if (!bValid)
		return;
	glBindTexture(GL_TEXTURE_2D, 0);
	//
	// Bind the framebuffer to render on : can be either Multisampled one or normal one
	//
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbms ? fbms : fb);

	glPushAttrib(GL_VIEWPORT_BIT);
	posx = x;
	posy = y;
	glViewport(0, 0, bufw, bufh);
}
/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
void AABox::Deactivate() {
	if (!bValid)
		return;
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glPopAttrib();
}