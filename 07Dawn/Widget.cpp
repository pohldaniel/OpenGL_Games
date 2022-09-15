#include "Widget.h"

Widget::Widget(short posX, short posY, short width, short height) : m_posX(posX), m_posY(posY), m_width(width), m_height(height){

	if(m_posY + m_height > HEIGHT) {
		m_posY = HEIGHT - m_height;
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
		//glTranslatef(posX, posY, 0.0f);

		m_transform.translate(0, m_posY, 0.0f);

		auto fontShader = Globals::shaderManager.getAssetPointer("font");
		glUseProgram(fontShader->m_program);
		fontShader->loadMatrix("u_model", m_transform.getTransformationMatrix());
		glUseProgram(0);

		for (unsigned short w = 0; w < m_childWidgets.size(); ++w){
			//std::cout << m_childWidgets[w]->getPosX() << std::endl;

			m_childWidgets[w]->draw();
		}

		m_transform.translate(0, -m_posY, 0.0f);
		glUseProgram(fontShader->m_program);
		fontShader->loadMatrix("u_model", Matrix4f::IDENTITY);
		glUseProgram(0);
		//glTranslatef(-posX, -posY, 0.0f);
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