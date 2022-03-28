#include "SeekerBar.h"

SeekerBar::SeekerBar(Vector2f position, const unsigned blocks, const unsigned currentBlock) : m_blocks(blocks), m_currentBlock(currentBlock) {
	for (unsigned i = 1; i <= m_blocks; i++) {
		const Vector2f pos = Vector2f(position[0] + (m_size[0] + 5) * i, position[1] - m_size[1] / 2.0f);
		addQuad(pos);
	}

	short stride = 7, offset = 3;
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);	
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	
	//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_shader = Globals::shaderManager.getAssetPointer("quad_color");

	{
		const Vector2f pos = Vector2f(m_vertices.front() - 50, position[1]);
		m_buttonLeft = new Button("<", Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f));
		m_buttonLeft->setPosition(pos);
		m_buttonLeft->setOrigin(m_buttonLeft->getSize() * 0.5f);
		m_buttonLeft->setOutlineThickness(4.0f);
	}

	{
		const Vector2f pos = Vector2f(m_vertices[m_vertices.size() - 21] + 50, position[1]);
		m_buttonRight = new Button(">", Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 80.0f / 255.0f));
		m_buttonRight->setPosition(pos);
		m_buttonRight->setOrigin(m_buttonRight->getSize() * 0.5f);
		m_buttonRight->setOutlineThickness(4.0f);
	}
}

SeekerBar::~SeekerBar() {
	if (m_buttonLeft) {
		delete m_buttonLeft;
		m_buttonLeft = NULL;
	}

	if (m_buttonRight) {
		delete m_buttonRight;
		m_buttonRight = NULL;
	}
}


void SeekerBar::addQuad(const Vector2f& pos) {
	Vector4f color = Vector4f(180, 180, 180, 255) * (1.0f / 255.0f);

	m_vertices.push_back(pos[0]            ); m_vertices.push_back(pos[1]            ); m_vertices.push_back(0.0); m_vertices.push_back(color[0]); m_vertices.push_back(color[1]); m_vertices.push_back(color[2]); m_vertices.push_back(color[3]);
	m_vertices.push_back(pos[0] + m_size[0]); m_vertices.push_back(pos[1]            ); m_vertices.push_back(0.0); m_vertices.push_back(color[0]); m_vertices.push_back(color[1]); m_vertices.push_back(color[2]); m_vertices.push_back(color[3]);
	m_vertices.push_back(pos[0] + m_size[0]); m_vertices.push_back(pos[1] + m_size[1]); m_vertices.push_back(0.0); m_vertices.push_back(color[0]); m_vertices.push_back(color[1]); m_vertices.push_back(color[2]); m_vertices.push_back(color[3]);
	m_vertices.push_back(pos[0]            ); m_vertices.push_back(pos[1] + m_size[1]); m_vertices.push_back(0.0); m_vertices.push_back(color[0]); m_vertices.push_back(color[1]); m_vertices.push_back(color[2]); m_vertices.push_back(color[3]);
}

void SeekerBar::setQuadColor(Vector4f& color1, Vector4f& color2) {
	short stride = 7;

	for (unsigned int i = 0; i < m_currentBlock * stride * 4; i = i + stride * 4) {
		for (unsigned int vert = 0; vert < 4; vert++) {

			for (unsigned int j = i + (3 + vert * stride), k = 0; k < 4; j++, k++) {
				m_vertices[j] = color1[k];
			}
		}
	}

	for (unsigned int i = m_currentBlock * stride * 4; i < m_blocks * stride * 4; i = i + stride * 4) {
		for (unsigned int vert = 0; vert < 4; vert++) {

			for (unsigned int j = i + (3 + vert * stride), k = 0; k < 4; j++, k++) {
				m_vertices[j] = color2[k];
			}
		}
	}
}

void SeekerBar::setQuadColor(unsigned int currentBlock, Vector4f& color) {
	short stride = 7;

	for (unsigned int i = currentBlock * stride * 4; i < (currentBlock + 1) * stride * 4; i = i + stride * 4) {
		for (unsigned int vert = 0; vert < 4; vert++) {
			for (unsigned int j = i + (3 + vert * stride), k = 0; k < 4; j++, k++) {
				m_vertices[j] = color[k];
			}
		}
	}
}

void SeekerBar::update() {
	//setQuadColor(Vector4f(220.0f, 220.0f, 220.0f, 255.0f) * (1.0f / 255.0f), Vector4f(180.0f, 180.0f, 180.0f, 120.0f) * (1.0f / 255.0f));

	m_buttonLeft->update();
	m_buttonRight->update();

	if (m_buttonLeft->isPressed()) {
		if (m_currentBlock > 0)
			m_currentBlock--;
	}else if (m_buttonRight->isPressed()) {
		if (m_currentBlock < m_blocks)
			m_currentBlock++;
	}

	for (int i = m_currentBlock; i < m_blocks; i++)
		setQuadColor(i, Vector4f(180.0f, 180.0f, 180.0f, 120.0f) * (1.0f / 255.0f));

	for (int i = 0; i < m_currentBlock; i++)
		setQuadColor(i, Vector4f(220.0f, 220.0f, 220.0f, 255.0f) * (1.0f / 255.0f));
}

void SeekerBar::render() {
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(float), &m_vertices[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnable(GL_BLEND);
	m_buttonLeft->render();
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("projection", Globals::projection);
	glBindVertexArray(m_vao);
	glDrawArrays(GL_QUADS, 0, 4 * m_blocks);
	glBindVertexArray(0);
	glUseProgram(0);
	m_buttonRight->render();
	glDisable(GL_BLEND);
}