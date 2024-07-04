#include <engine/input/Keyboard.h>
#include <engine/Fontrenderer.h>
#include <Entities/Player.h>
#include "Dialog.h"
#include "Globals.h"

std::vector<Dialog> DialogTree::DialogData;

DialogTree::DialogTree(const Camera& camera) : camera(camera), m_currentIndex(-1), m_finished(true){

}

DialogTree::~DialogTree() {

}

void DialogTree::draw() {
	Globals::fontManager.get("dialog").bind();

	if (m_currentIndex >= 0) {
		const Dialog& dialogData = DialogData[m_currentIndex];
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), dialogData.posX - camera.getPositionX(), dialogData.posY - camera.getPositionY(), dialogData.text, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);
		Fontrenderer::Get().drawBuffer();
	}
}

void DialogTree::addDialog(float posX, float posY, const std::string& text) {
	DialogData.push_back({posX, posY, text});
}

void DialogTree::setFinished(bool finished) {
		m_finished = false;
}

void DialogTree::processInput() {
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

bool DialogTree::isFinished() {
	return m_finished;
}