#pragma once
#include "_shader.h"
#include <GL/glew.h>
#include "drawableObject.h"

// refactors those shaders which write only into the screen space (e.g. post processing, ShaderToy.com), so only the fragment shader it's needed and the model is only two triangles
class ScreenSpaceShader : drawableObject
{
public:
	ScreenSpaceShader(const char * fragmentPath);
	~ScreenSpaceShader();

	Shader2 * const getShaderPtr() {
		return shad;
	}

	Shader2 & const getShader() {
		return *shad;
	}

	virtual void draw();
	static void drawQuad();

	static void disableTests() {
		//glDisable(GL_CLIP_DISTANCE0);
		glDisable(GL_DEPTH_TEST);
	}

	static void enableTests() {
		glEnable(GL_DEPTH_TEST);
	}



private:
	Shader2 * shad;
	static unsigned int quadVAO, quadVBO;
	static bool initialized;

	void initializeQuad();
};

