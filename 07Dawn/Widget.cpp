#include "Widget.h"

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