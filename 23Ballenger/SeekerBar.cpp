#include "SeekerBar.h"
#include "Globals.h"

SeekerBar::SeekerBar() : Widget() , m_batchrenderer(new Batchrenderer()) {
	m_batchrenderer->init(10);

	m_buttonLeft = new Button();
	m_buttonLeft->setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttonLeft->setOutlineThickness(4.0f);
	m_buttonLeft->setText("<");
	m_buttonLeft->setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));
	
	m_buttonRight = new Button();
	m_buttonRight->setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttonRight->setOutlineThickness(4.0f);
	m_buttonRight->setText(">");
	m_buttonRight->setShader(Globals::shaderManager.getAssetPointer("quad_color_uniform"));
	
	m_size[0] = m_buttonLeft->getSize()[1] + 2.0f * m_buttonLeft->getTickness();
	m_size[1] = m_buttonLeft->getSize()[1] + 2.0f * m_buttonLeft->getTickness();

	m_blocks = 10;
	m_currentBlock = 2;
}

SeekerBar::SeekerBar(SeekerBar const& rhs) : Widget(rhs) {
	m_blocks = rhs.m_blocks;
	m_currentBlock = rhs.m_currentBlock;

	if (rhs.m_buttonLeft) {
		m_buttonLeft = new Button();
		std::swap(*m_buttonLeft, *rhs.m_buttonLeft);
	}

	if (rhs.m_buttonRight) {
		m_buttonRight = new Button();
		std::swap(*m_buttonRight, *rhs.m_buttonRight);
	}

	if (rhs.m_batchrenderer) {
		m_batchrenderer = new Batchrenderer();
		std::swap(*m_batchrenderer, *rhs.m_batchrenderer);
	}
}

SeekerBar& SeekerBar::operator=(const SeekerBar& rhs) {
	Widget::operator=(rhs);
	
	m_blocks = rhs.m_blocks;
	m_currentBlock = rhs.m_currentBlock;

	if (rhs.m_buttonLeft) {
		m_buttonLeft = new Button();
		std::swap(*m_buttonLeft, *rhs.m_buttonLeft);
	}

	if (rhs.m_buttonRight) {
		m_buttonRight = new Button();
		std::swap(*m_buttonRight, *rhs.m_buttonRight);
	}

	if (rhs.m_batchrenderer) {
		m_batchrenderer = new Batchrenderer();
		std::swap(*m_batchrenderer, *rhs.m_batchrenderer);
	}

	return *this;
}

SeekerBar::~SeekerBar() {
	delete m_buttonLeft;
	delete m_buttonRight;
	delete m_batchrenderer;
}

void SeekerBar::draw() {
	Vector2f pos;
	for (unsigned int i = 0; i < m_currentBlock; i++) {

		pos = Vector2f(m_position[0] + (m_size[0] + 5.0f) * i, m_position[1]);
		m_batchrenderer->addQuadAA(Vector4f(pos[0], pos[1], m_size[0], m_size[1]), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 0u);
	}

	for (unsigned int i = m_currentBlock; i < m_blocks; i = i++) {
		pos = Vector2f(m_position[0] + (m_size[0] + 5.0f) * i, m_position[1]);
		m_batchrenderer->addQuadAA(Vector4f(pos[0], pos[1], m_size[0], m_size[1]), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 0u);
	}

	m_batchrenderer->drawBuffer();
	m_buttonLeft->draw();
	m_buttonRight->draw();
}

void SeekerBar::processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button) {
	m_buttonLeft->processInput(mouseX, mouseY, button);
	m_buttonRight->processInput(mouseX, mouseY, button);

	if (m_buttonLeft->isPressed()) {
		if (m_currentBlock > 0)
			m_currentBlock--;
	}else if (m_buttonRight->isPressed()) {
		if (m_currentBlock < m_blocks)
			m_currentBlock++;
	}
}

void SeekerBar::setLeftFunction(std::function<void()> fun) {
	m_buttonLeft->setFunction(fun);
}

void SeekerBar::setRightFunction(std::function<void()> fun) {
	m_buttonRight->setFunction(fun);
}

void SeekerBar::setShader(const Shader* shader) {
	shader->use();
	shader->loadMatrix("u_transform", Orthographic);
	shader->unuse();
	m_batchrenderer->setShader(shader);
}

void SeekerBar::setPosition(const float x, const float y) {
	Widget::setPosition(x, y);
	m_buttonLeft->setPosition(m_position[0] - (m_buttonLeft->getSize()[0] + 2.0f * m_buttonLeft->getTickness() + 5.0f), m_position[1]);
	m_buttonRight->setPosition(m_position[0] + (m_size[0] + 5.0f) * m_blocks , m_position[1]);
}

void SeekerBar::setPosition(const Vector2f& position) {
	Widget::setPosition(position);
	m_buttonLeft->setPosition(m_position[0] - (m_buttonLeft->getSize()[0]  + 2.0f * m_buttonLeft->getTickness() + 5.0f), m_position[1]);
	m_buttonRight->setPosition(m_position[0] + (m_size[0] + 5.0f) * m_blocks, m_position[1]);
}

void SeekerBar::setGridSize(const float sx) {
	m_size[0] = sx;
}

void SeekerBar::setCurrentBlock(unsigned int block) {
	m_currentBlock = block;
}