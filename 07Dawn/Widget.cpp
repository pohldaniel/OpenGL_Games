#include "Widget.h"

Widget::Widget(short posX, short posY, short width, short height, short offsetX, short offsetY) : m_posX(posX), m_posY(posY), m_width(width), m_height(height), m_offsetX(offsetX), m_offsetY(offsetY) {

	//To create the singeltons on startup the HEIGHT has to be known
	//Don't use ViewPort::Get().getHeight()
	if(static_cast<unsigned short>(m_posY + m_height) > HEIGHT) {
		m_posY = ViewPort::Get().getHeight() - m_height;
	}
}

Widget::~Widget() {

	for (unsigned short w = 0; w < m_childWidgets.size(); ++w) {
		delete m_childWidgets[w];
	}

	m_childWidgets.resize(0);
}

void Widget::draw() {
	if (m_childWidgets.size() > 0) {
		
		for (unsigned short w = 0; w < m_childWidgets.size(); ++w){
			m_childWidgets[w]->draw();
		}
	}
}

void Widget::processInput() {

	Mouse &mouse = Mouse::instance();

	if (m_moveableFrame) {
		if (mouse.buttonDown(Mouse::BUTTON_LEFT)) {
			if (!m_movingFrame && isMouseOnTitlebar(mouse.xPosAbsolute(), mouse.yPosAbsolute())) {
				m_movingFrame = true;
				m_startMovingXpos = mouse.xPosAbsolute();
				m_startMovingYpos = mouse.yPosAbsolute();
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
				if (m_onClose)
					m_onClose();
			}
		}
	}

	if (m_childWidgets.size() > 0) {
		for (unsigned short w = 0; w < m_childWidgets.size(); ++w) {
			m_childWidgets[w]->processInput();
		}
	}
}

void Widget::activate() {
	if (m_onActivate)
		m_onActivate();
}

void Widget::close() {
	if(m_onClose)
		m_onClose();
}

bool Widget::isMouseOnTitlebar(int mouseX, int mouseY) const {
	// if the frame has no titlebar we return false right away...
	if (m_moveableFrame == false) {
		return false;
	}

	if (mouseX < m_posX + m_titleOffsetX
		|| mouseY <  static_cast<int>(ViewPort::Get().getHeight()) - (m_posY + m_titleOffsetY + m_titleHeight)
		|| mouseX > m_posX + m_titleOffsetX + m_titleWidth
		|| mouseY > static_cast<int>(ViewPort::Get().getHeight()) - (m_posY + m_titleOffsetY)) {
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
		|| mouseY < static_cast<int>(ViewPort::Get().getHeight()) - (m_posY + m_buttonOffsetY + m_buttonHeight)
		|| mouseX > m_posX + m_buttonOffsetX + m_buttonWidth
		|| mouseY > static_cast<int>(ViewPort::Get().getHeight()) - (m_posY + m_buttonOffsetY)) {
		return false;
	}
	return true;
}

bool Widget::isMouseOnFrame(int mouseX, int mouseY) const {
	if (!m_visible){
		return false;
	}

	if (mouseX < m_posX + m_offsetX
		|| mouseY < static_cast<int>(ViewPort::Get().getHeight()) - (m_posY + m_offsetY + m_height)
		|| mouseX >  m_posX + m_offsetX + m_width
		|| mouseY >  static_cast<int>(ViewPort::Get().getHeight()) - (m_posY + m_offsetY)) {
		return false;
	}

	return true;
}

void Widget::moveFrame(int mouseX, int mouseY) {

	m_posX = (mouseX < 0 || mouseX >  static_cast<int>(ViewPort::Get().getWidth())) ? m_posX : m_posX + mouseX - m_startMovingXpos;
	m_posY -= mouseY - m_startMovingYpos;

	m_posX = (std::max)(-m_titleWidth + 11, (std::min)(static_cast<int>(m_posX), static_cast<int>(ViewPort::Get().getWidth()) - 33));
	m_posY = (std::max)(-m_titleOffsetY, (std::min)(static_cast<int>(m_posY), static_cast<int>(ViewPort::Get().getHeight()) - (m_titleOffsetY + m_titleHeight)));

	m_startMovingXpos = (std::max)(0, (std::min)(mouseX, static_cast<int>(ViewPort::Get().getWidth())));
	m_startMovingYpos = (std::max)(0, (std::min)(mouseY, static_cast<int>(ViewPort::Get().getHeight())));
}

void Widget::stopMovingFrame() {
	m_movingFrame = false;
	m_startMovingXpos = 0;
	m_startMovingYpos = 0;
}

void Widget::addToParent(int posX, int posY, Widget* parent) {
	this->m_parentWidget = parent;
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

void Widget::resize(int deltaW, int deltaH) {
	//top right corner
	if (m_posX + m_titleWidth > static_cast<int>(ViewPort::Get().getWidth()) && deltaW < 0)
		m_posX += deltaW;

	if (deltaH < 0 && m_posY > 0) {
		m_posY += deltaH;
	}

	if (deltaH > 0) {
		m_posY += deltaH;
	}
	////////////

	//left bottom corner
	//m_posX += deltaW;
}

void Widget::setOnClose(std::function<void()> fun) {
	m_onClose = fun;
}

void  Widget::setOnActivate(std::function<void()> fun) {
	m_onActivate = fun;
}