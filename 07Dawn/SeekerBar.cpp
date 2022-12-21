#include "SeekerBar.h"
#include "Dialog.h"

SeekerBar::SeekerBar(float& value) : m_value(value) {
	m_width = 15;
	m_height = 15;
}

SeekerBar::~SeekerBar() {}

void SeekerBar::draw() {
	
	//TextureManager::BindTexture(DialogCanvas::TextureAtlas, true, 2);
	TextureManager::DrawTextureBatched(DialogCanvas::Textures[10], m_parentWidget->getPosX() + getPosX(), m_parentWidget->getPosY() + getPosY(), static_cast<float>(DialogCanvas::Textures[10].width - 10), static_cast<float>(DialogCanvas::Textures[10].height - 10), false, false);
	TextureManager::DrawTextureBatched(DialogCanvas::Textures[11], m_parentWidget->getPosX() + getPosX() + 150, m_parentWidget->getPosY() + getPosY(), static_cast<float>(DialogCanvas::Textures[11].width - 10), static_cast<float>(DialogCanvas::Textures[11].height - 10), false, false);
	TextureManager::DrawTextureBatched(DialogCanvas::Textures[9], m_parentWidget->getPosX() + getPosX() + 22, m_parentWidget->getPosY() + getPosY() + 4, (150.0f - 22.0f) * m_value, 13.0f, false, false);
	//TextureManager::UnbindTexture(true, 2);

}

void SeekerBar::processInput() {

	if (Mouse::instance().buttonPressed(Mouse::BUTTON_LEFT)) {
		float value = m_value;
		if (m_parentWidget->getPosX() + getPosX() < ViewPort::Get().getCursorPosRelX() && ViewPort::Get().getCursorPosRelX() < m_parentWidget->getPosX() + getPosX() + 22 &&
			m_parentWidget->getPosY() + getPosY() < ViewPort::Get().getCursorPosRelY() && ViewPort::Get().getCursorPosRelY() < m_parentWidget->getPosY() + getPosY() + 22) {
			m_value -= 0.1f;
			m_value = std::max(0.0f, std::min(m_value, 1.0f));
		}

		if (m_parentWidget->getPosX() + getPosX() + 150 < ViewPort::Get().getCursorPosRelX() && ViewPort::Get().getCursorPosRelX() < m_parentWidget->getPosX() + getPosX() + 150 + 22 &&
			m_parentWidget->getPosY() + getPosY() < ViewPort::Get().getCursorPosRelY() && ViewPort::Get().getCursorPosRelY() < m_parentWidget->getPosY() + getPosY() + 22) {
			m_value += 0.1f;
			m_value = std::max(0.0f, std::min(m_value, 1.0f));

		}

		if (value != m_value)
			m_onClicked();
	}
}

void SeekerBar::setOnClicked(std::function<void()> fun) {
	m_onClicked = fun;
}