#include <GL/glew.h>
#include "Pixelbuffer.h"

Pixelbuffer::Pixelbuffer() {}

Pixelbuffer::~Pixelbuffer() {
	cleanup();
}

void Pixelbuffer::create(unsigned int size, bool doubleBuffer) {
	
	m_doubleBuffer = doubleBuffer;

	if (m_doubleBuffer) {
		glGenBuffers(2, m_pbo);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo[0]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, size, 0, GL_STREAM_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo[1]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, size, 0, GL_STREAM_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	}else {
		glGenBuffers(1, &m_pbo[0]);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo[0]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, size, 0, GL_STREAM_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
}

void Pixelbuffer::cleanup() {
	if (m_pbo[0]) {
		glDeleteFramebuffers(1, &m_pbo[0]);
		m_pbo[0] = 0;
	}

	if (m_pbo[1]) {
		glDeleteFramebuffers(1, &m_pbo[1]);
		m_pbo[1] = 0;
	}
}

void Pixelbuffer::mapData(const Texture& texture, unsigned char* data) {

	unsigned int dataSize = texture.getHeight() * texture.getWidth() * texture.getChannels();
	if (m_doubleBuffer) {
		read = !read;
		write = !read;
	}else {
		read = write = false;
	}

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo[read]);
	glBindTexture(GL_TEXTURE_2D, texture.getTexture());
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture.getWidth(), texture.getHeight(), texture.getFormat(), GL_UNSIGNED_BYTE, 0);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo[write]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, dataSize, NULL, GL_STREAM_DRAW);

	GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		memcpy(ptr, data, dataSize);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	}

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
