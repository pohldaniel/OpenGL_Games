#include <engine/Fontrenderer.h>

#include "PBOInterface.h"
#include "Application.h"
#include "Globals.h"

const int    IMAGE_WIDTH = 2048;
const int    IMAGE_HEIGHT = 2048;
const int    CHANNEL_COUNT = 4;
const int    DATA_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT * CHANNEL_COUNT;
const GLenum PIXEL_FORMAT = GL_BGRA;

void PBOInterface::updatePixels(GLubyte* dst, int size) {
	static int color = 0;

	if (!dst)
		return;

	int* ptr = (int*)dst;

	// copy 4 bytes at once
	for (int i = 0; i < IMAGE_HEIGHT; ++i)
	{
		for (int j = 0; j < IMAGE_WIDTH; ++j)
		{
			*ptr = color;
			++ptr;
		}
		color += 257;
	}
	++color;            // scroll down
}

PBOInterface::PBOInterface(StateMachine& machine) : State(machine, CurrentState::PBOINTERFACE) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	
	// allocate texture buffer
	imageData = new GLubyte[DATA_SIZE];
	memset(imageData, 128, DATA_SIZE);

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, IMAGE_WIDTH, IMAGE_HEIGHT, 0, PIXEL_FORMAT, GL_UNSIGNED_BYTE, (GLvoid*)imageData);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenBuffers(2, pboIds);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[0]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[1]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
}

PBOInterface::~PBOInterface() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void PBOInterface::fixedUpdate() {}

void PBOInterface::update() {}

void PBOInterface::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static int index = 0;
	int nextIndex = 0;

	glBindTexture(GL_TEXTURE_2D, textureId);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[index]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[nextIndex]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
	GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	if (ptr){
		updatePixels(ptr, DATA_SIZE);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	}
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadInt("u_texture", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	Globals::shapeManager.get("quad").drawRaw();
	shader->unuse();
}

void PBOInterface::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void PBOInterface::OnMouseButtonDown(Event::MouseButtonEvent& event) {

}

void PBOInterface::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void PBOInterface::resize(int deltaW, int deltaH) {
	
}