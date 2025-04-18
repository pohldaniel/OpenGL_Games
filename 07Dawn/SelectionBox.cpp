#include "SelectionBox.h"
#include "Dialog.h"

SelectionBox::SelectionBox(const CharacterSet& font, const CharacterSet& selectFont, int& selected) : m_selected(selected) {
	setFont(font);
	setSelectFont(selectFont);
	entries.clear();
	baseColor = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	selectColor = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	m_onSelected = NULL;
	maxWidth = 0;
	maxSelectedWidth = 0;
	state = OPEN;
}

SelectionBox::~SelectionBox() {}

void SelectionBox::setFont(const CharacterSet& font) {
	this->font = &font;
}

void SelectionBox::setSelectFont(const CharacterSet& selectFont) {
	this->selectFont = &selectFont;
}

void SelectionBox::setEntries(std::vector<std::string> entries, int initialSelected) {
	this->entries = entries;
	m_selected = initialSelected;
	maxWidth = 0;
	maxSelectedWidth = 0;
	for (size_t curEntryNr = 0; curEntryNr<entries.size(); ++curEntryNr) {
		int curWidth = font->getWidth(entries[curEntryNr]);
		maxWidth = std::max(maxWidth, curWidth);
		int curSelectedWidth = selectFont->getWidth(entries[curEntryNr]);
		maxSelectedWidth = std::max(maxSelectedWidth, curSelectedWidth);
	}
	maxWidth += 20; // border and stuff like that
	this->state = CLOSED;
}

int SelectionBox::getSelected() const {
	return m_selected;
}

void SelectionBox::setSelected(int selected) {
	state = CLOSED;

	
	if (m_onSelected) {
		m_onSelected(selected);
	}

	m_selected = selected;
}

void SelectionBox::setOnSelected(std::function<void(int)> fun) {
	m_onSelected = fun;
}

int SelectionBox::getWidth() const {
	return maxWidth;
}

int SelectionBox::getHeight() const {
	return font->lineHeight;
}

void SelectionBox::setBaseColor(const Vector4f &color) {
	baseColor = color;
}

void SelectionBox::setSelectColor(const Vector4f &color) {
	selectColor = color;
}

void SelectionBox::draw() {

	if (m_selected >= 0) {
		bool mouseInsideBox = (ViewPort::Get().getCursorPosRelX() >= getPosX() && ViewPort::Get().getCursorPosRelX() < getPosX() + getWidth() && ViewPort::Get().getCursorPosRelY() >= getPosY() && ViewPort::Get().getCursorPosRelY() < getPosY() + getHeight());
		Vector4f color = mouseInsideBox ? selectColor : baseColor;
		Fontrenderer::Get().addText(*font, m_parentWidget->getPosX() + getPosX(), m_parentWidget->getPosY() + getPosY(), entries[m_selected], color);
	}

	if (state == OPEN) {
		TextureManager::DrawTextureBatched(DialogCanvas::Textures[4], m_parentWidget->getPosX() + getPosX(), m_parentWidget->getPosY() + getPosY() - selectFont->lineHeight * 1.2f * (entries.size() - 1) - 15, std::max(maxSelectedWidth + 10, getWidth() - 15), selectFont->lineHeight * 1.2f * (entries.size() - 1) + 15, false, false);
		// draw the entries and highlight the one the mouse is over (if any)
		int curX = m_parentWidget->getPosX() + getPosX() + 10;
		int curY = m_parentWidget->getPosY() + getPosY() - selectFont->lineHeight * 1.2f;
		for (size_t curEntryNr = 0; curEntryNr<entries.size(); ++curEntryNr) {
			bool mouseOverEntry = (ViewPort::Get().getCursorPosRelX() > curX && ViewPort::Get().getCursorPosRelX() <= curX + maxSelectedWidth && ViewPort::Get().getCursorPosRelY() > curY && ViewPort::Get().getCursorPosRelY() < curY + (selectFont->lineHeight * 1.2f));
			
			Vector4f color = mouseOverEntry ? selectColor : baseColor;
			Fontrenderer::Get().addText(*selectFont, curX, curY, entries[curEntryNr], color);
			curY -= (selectFont->lineHeight * 1.2f);
		}
	}
}

void SelectionBox::processInput() {

	if (Mouse::instance().buttonPressed(Mouse::BUTTON_LEFT)) {
		if (ViewPort::Get().getCursorPosRelX() > m_parentWidget->getPosX() + getPosX() + 5 && ViewPort::Get().getCursorPosRelX() < m_parentWidget->getPosX() + getPosX() + getWidth() &&
			ViewPort::Get().getCursorPosRelY() > m_parentWidget->getPosY() + getPosY() && ViewPort::Get().getCursorPosRelY() < m_parentWidget->getPosY() + getPosY() + getHeight()) {
			state = state == OPEN ? CLOSED : OPEN;	
		}
	
	
		if (state == OPEN) {
			int curX = m_parentWidget->getPosX() + getPosX() + 10;
			int curY = m_parentWidget->getPosY() + getPosY() - selectFont->lineHeight * 1.2f;
			for (size_t curEntryNr = 0; curEntryNr<entries.size(); ++curEntryNr) {
				bool mouseOverEntry = (ViewPort::Get().getCursorPosRelX() > curX && ViewPort::Get().getCursorPosRelX() <= curX + maxSelectedWidth && ViewPort::Get().getCursorPosRelY() > curY && ViewPort::Get().getCursorPosRelY() < curY + (selectFont->lineHeight * 1.2f));
				if (mouseOverEntry) {
					setSelected(curEntryNr);
					break;
				}
				curY -= (selectFont->lineHeight * 1.2f);
			}
		}
	}
}

void SelectionBox::processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button) {

	if (button == Event::MouseButtonEvent::MouseButton::BUTTON_LEFT) {
		if (mouseX > m_parentWidget->getPosX() + getPosX() + 5 && mouseX < m_parentWidget->getPosX() + getPosX() + getWidth() &&
			mouseY > m_parentWidget->getPosY() + getPosY() && mouseY < m_parentWidget->getPosY() + getPosY() + getHeight()) {
			state = state == OPEN ? CLOSED : OPEN;
		}

		if (state == OPEN) {
			int curX = m_parentWidget->getPosX() + getPosX() + 10;
			int curY = m_parentWidget->getPosY() + getPosY() - selectFont->lineHeight * 1.2f;
			for (size_t curEntryNr = 0; curEntryNr<entries.size(); ++curEntryNr) {
				bool mouseOverEntry = (mouseX > curX && mouseX <= curX + maxSelectedWidth && ViewPort::Get().getCursorPosRelY() > curY && mouseY < curY + (selectFont->lineHeight * 1.2f));
				if (mouseOverEntry) {
					setSelected(curEntryNr);
					break;
				}
				curY -= (selectFont->lineHeight * 1.2f);
			}
		}
	}
}