#include "Widget.h"
#include <iostream>
Widget::Widget(short posX, short posY, short width, short height) : m_posX(posX), m_posY(posY), m_width(width), m_height(height){

	if(static_cast<unsigned short>(m_posY + m_height) > ViewPort::get().getHeight()) {
		m_posY = ViewPort::get().getHeight() - m_height;
	}
}

Widget::~Widget() {

	for (unsigned short w = 0; w < m_childWidgets.size(); ++w) {
		delete m_childWidgets[w];
	}

	m_childWidgets.resize(0);
}

void Widget::draw(int posX, int posY) {
	if (m_childWidgets.size() > 0) {
		
		for (unsigned short w = 0; w < m_childWidgets.size(); ++w){
			m_childWidgets[w]->draw(posX, posY);
		}
	}
}

void Widget::update(int mouseX, int mouseY) {
	if (m_childWidgets.size() > 0) {
		for (unsigned short w = 0; w < m_childWidgets.size(); ++w) {
			m_childWidgets[w]->update(mouseX - m_posX, mouseY - m_posY);
		}
	}
}

void Widget::processInput() {
	Mouse &mouse = Mouse::instance();
	if (m_moveableFrame) {
		if (mouse.buttonDown(Mouse::BUTTON_LEFT)) {
			if (!m_movingFrame && isMouseOnTitlebar(mouse.xPosAbsolute(), mouse.yPosAbsolute())) {
				m_movingFrame = true;
				m_startMovingFrameXpos = mouse.xPosAbsolute();
				m_startMovingFrameYpos = mouse.yPosAbsolute();
			}
		}else {
			stopMovingFrame();
		}

		if (m_movingFrame)
			moveFrame(mouse.xPosAbsolute(), mouse.yPosAbsolute());
	}

	if (m_closeButton) {
		if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
			if (isMouseOnCloseButton(mouse.xPosAbsolute(), mouse.yPosAbsolute())) {
				m_visible = false;
			}
		}
	}
}

bool Widget::isMouseOnTitlebar(int mouseX, int mouseY) const {
	// if the frame has no titlebar we return false right away...
	if (m_moveableFrame == false) {
		return false;
	}

	if (mouseX < m_posX + m_titleOffsetX
		|| mouseY <  static_cast<int>(ViewPort::get().getHeight()) - (m_posY + m_titleOffsetY + m_titleHeight)
		|| mouseX > m_posX + m_titleOffsetX + m_titleWidth
		|| mouseY > static_cast<int>(ViewPort::get().getHeight()) - (m_posY + m_titleOffsetY)) {
		return false;
	}
	return true;
}

bool Widget::isMouseOnCloseButton(int mouseX, int mouseY) const {
	// if the frame has no closebutton we return false right away...
	if (m_closeButton == false){
		return false;
	}

	if (mouseX < m_posX + m_buttonOffsetX
		|| mouseY < static_cast<int>(ViewPort::get().getHeight()) - (m_posY + m_buttonOffsetY + m_buttonHeight)
		|| mouseX > m_posX + m_buttonOffsetX + m_buttonWidth
		|| mouseY > static_cast<int>(ViewPort::get().getHeight()) - (m_posY + m_buttonOffsetY)) {
		return false;
	}
	return true;
}

void Widget::moveFrame(int mouseX, int mouseY) {

	m_posX = (mouseX < 0 || mouseX >  static_cast<int>(ViewPort::get().getWidth())) ? m_posX : m_posX + mouseX - m_startMovingFrameXpos;
	m_posY -= mouseY - m_startMovingFrameYpos;

	m_posX = (std::max)(-m_titleWidth + 11, (std::min)(static_cast<int>(m_posX), static_cast<int>(ViewPort::get().getWidth()) - 33));
	m_posY = (std::max)(-m_titleOffsetY, (std::min)(static_cast<int>(m_posY), static_cast<int>(ViewPort::get().getHeight()) - (m_titleOffsetY + m_titleHeight)));

	m_startMovingFrameXpos = (std::max)(0, (std::min)(mouseX, static_cast<int>(ViewPort::get().getWidth())));
	m_startMovingFrameYpos = (std::max)(0, (std::min)(mouseY, static_cast<int>(ViewPort::get().getHeight())));
}

void Widget::stopMovingFrame() {
	m_movingFrame = false;
	m_startMovingFrameXpos = 0;
	m_startMovingFrameYpos = 0;
}

void Widget::addToParent(int posX, int posY, Widget* parent) {
	this->m_parentFrame = parent;
	setPosition(posX, posY);
}

void Widget::addChildWidget(int posX, int posY, std::auto_ptr<Widget> newChild) {
	newChild->addToParent(posX, posY, this);
	m_childWidgets.push_back(newChild.release());
}

void Widget::setPosition(int posX, int posY) {
	m_posX = posX;
	m_posY = posY;
}

void Widget::setSize(int width, int height) {
	m_width = width;
	m_height = height;
}

int Widget::getPosX() const {
	return m_posX;
}

int Widget::getPosY() const {
	return m_posY;
}

int Widget::getWidth() const {
	return m_width;
}

int Widget::getHeight() const {
	return m_height;
}

std::vector<Widget*> Widget::getChildWidgets() {
	return m_childWidgets;
}

void Widget::toggle() {

}

void Widget::addMoveableFrame(unsigned short titleWidth, unsigned short titleHeight, short titleOffsetX, short titleOffsetY) {
	m_moveableFrame = true;
	m_titleWidth = titleWidth;
	m_titleHeight = titleHeight;
	m_titleOffsetX = titleOffsetX;
	m_titleOffsetY = titleOffsetY;
}

void Widget::addCloseButton(unsigned short buttonWidth, unsigned short buttonHeight, short buttonOffsetX, short buttonOffsetY) {
	m_closeButton = true;
	m_buttonWidth = buttonWidth;
	m_buttonHeight = buttonHeight;
	m_buttonOffsetX = buttonOffsetX;
	m_buttonOffsetY = buttonOffsetY;
}

bool Widget::isVisible() const {
	return m_visible;
}

void Widget::setVisible(bool visible) {
	m_visible = visible;
}