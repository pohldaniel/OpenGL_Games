#include "Application.h"
#include "i-game-state.hpp"

#include <GL/glew.h>
#include "constants.hpp"

IGameState::IGameState() : InputHandler(Application::Emitter) {}

void IGameState::restoreGpuState() {
	// Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Application::Width, Application::Height);
	glClearStencil(0);

	// Z-buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	// Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1, 1, 1, 1);

	// Clear window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
