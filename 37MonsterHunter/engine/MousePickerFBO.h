#pragma once

#include "Camera.h"
#include "Framebuffer.h"

class MousePickerFBO {

public:
	MousePickerFBO(unsigned int width, unsigned int height);

	void updateObjectId(unsigned int posX, unsigned int posY);
	unsigned int getPickedId() const;

	const Framebuffer& getBuffer() const;

private:


	Framebuffer m_mousePickBuffer;

	const float m_transitionSpeed = 1.5f;
	bool m_transitionEnd = false;
	bool m_fadeIn = false;
	bool m_fadeOut = true;
	float m_radius = 0.0f;

	unsigned int m_pickedId = 0;
	const int PBO_COUNT = 2;
	unsigned int pboIds[2];
	int index = 0;
	int nextIndex = 0;
	unsigned int m_width, m_height;
};