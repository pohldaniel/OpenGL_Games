#include "Pixelbuffer.h"

Pixelbuffer::Pixelbuffer() {}

Pixelbuffer::~Pixelbuffer() {
	cleanup();
}

void Pixelbuffer::create() {
	
}

void Pixelbuffer::create(unsigned int width, unsigned int height) {
	
}

void Pixelbuffer::cleanup() {
	if (m_pbo) {
		glDeleteFramebuffers(1, &m_pbo);
		m_pbo = 0;
	}
}