#include "CheckBox.h"
#include "Dialog.h"

CheckBox::CheckBox() {
	baseColor = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	selectColor = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);	

	m_width = 15;
	m_height = 15;
}

CheckBox::~CheckBox() {

}

void CheckBox::setBaseColor(const Vector4f &color) {
	baseColor = color;
}

void CheckBox::setSelectColor(const Vector4f &color) {
	selectColor = color;
}

void CheckBox::draw() {

	TextureManager::BindTexture(DialogCanvas::TextureAtlas, true, 2);
	TextureManager::DrawTexture(DialogCanvas::Textures[9], m_parentWidget->getPosX() + getPosX(), m_parentWidget->getPosY() + getPosY(), 16.0f, 16.0f, Vector4f(1.0f, 1.0f, 0.0f, 1.0f), false, false);
	
	if(m_checked)
		TextureManager::DrawTexture(DialogCanvas::Textures[9], m_parentWidget->getPosX() + getPosX() + 2, m_parentWidget->getPosY() + getPosY() + 2, 12.0f, 12.0f, Vector4f(0.0f, 1.0f, 0.0f, 1.0f), false, false);
	TextureManager::UnbindTexture(true, 2);
	
}

void CheckBox::processInput() {

	if (Mouse::instance().buttonPressed(Mouse::BUTTON_LEFT)) {
		if (ViewPort::Get().getCursorPosRelX() > m_parentWidget->getPosX() + getPosX() + 2 && ViewPort::Get().getCursorPosRelX() < m_parentWidget->getPosX() + getPosX() + 16 &&
			ViewPort::Get().getCursorPosRelY() > m_parentWidget->getPosY() + getPosY() + 2 && ViewPort::Get().getCursorPosRelY() < m_parentWidget->getPosY() + getPosY() + getHeight() + 10) {
			m_checked = !m_checked;
			if (m_checked)
				m_onChecked();
			else
				m_onUnchecked();
		}	
	}
}

void CheckBox::setOnChecked(std::function<void()> fun) {
	m_onChecked = fun;
}

void CheckBox::setOnUnchecked(std::function<void()> fun) {
	m_onUnchecked = fun;
}