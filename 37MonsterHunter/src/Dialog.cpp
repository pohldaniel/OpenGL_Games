#include <engine/input/Keyboard.h>
#include <engine/Fontrenderer.h>
#include <Entities/Player.h>
#include "Dialog.h"
#include "Globals.h"

std::vector<DialogDataNew> Dialog::DialogData;

Dialog::Dialog(const Camera& camera) : camera(camera), m_currentIndex(-1), m_finished(true){

}

Dialog::~Dialog() {

}

void Dialog::draw() {
	Globals::fontManager.get("dialog").bind();

	if (m_currentIndex >= 0) {
		const DialogDataNew& dialogData = DialogData[m_currentIndex];
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), dialogData.posX - camera.getPositionX(), dialogData.posY - camera.getPositionY(), dialogData.text, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
		Fontrenderer::Get().drawBuffer();
	}
}

void Dialog::addDialog(float posX, float posY, const std::string& text) {
	DialogData.push_back({posX, posY, text});
}

void Dialog::setFinished(bool finished) {
		m_finished = false;
}

void Dialog::processInput() {
	if (m_finished)
		return;

	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {		
			m_currentIndex++;
			if (m_currentIndex > (DialogData.size() - 1)) {
				m_finished = true;
				m_currentIndex = -1;
				DialogData.clear();
				DialogData.shrink_to_fit();
			}
	}
}

bool Dialog::isFinished() {
	return m_finished;
}