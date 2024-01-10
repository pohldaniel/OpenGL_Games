#include "Application.h"
#include "MousePickerFBO.h"

MousePickerFBO::MousePickerFBO() {

	m_mousePickBuffer.create(Application::Width, Application::Height);
	m_mousePickBuffer.attachTexture2D(AttachmentTex::RGBA);
	m_mousePickBuffer.attachRenderbuffer(AttachmentRB::DEPTH24);
	const int DATA_SIZE = Application::Width * Application::Height * 4;

	glGenBuffers(PBO_COUNT, pboIds);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[0]);
	glBufferData(GL_PIXEL_PACK_BUFFER, DATA_SIZE, 0, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[1]);
	glBufferData(GL_PIXEL_PACK_BUFFER, DATA_SIZE, 0, GL_STREAM_READ);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void MousePickerFBO::updateObjectId(unsigned int posX, unsigned int posY) {

	index = (index + 1) % 2;
	nextIndex = (index + 1) % 2;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_mousePickBuffer.getFramebuffer());
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[index]);
	glReadPixels(posX, Application::Height - posY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[nextIndex]);
	GLubyte* src = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	if (src) {
		m_pickedId = src[0] + src[1] * 256 + src[2] * 256 * 256 /*+ src[3] * 256 * 256 * 256*/;
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	}
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

unsigned int MousePickerFBO::getPickedId() const {
	return m_pickedId;
}

const Framebuffer& MousePickerFBO::getBuffer() const {
	return m_mousePickBuffer;
}