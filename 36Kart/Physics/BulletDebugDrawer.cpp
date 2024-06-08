#include <GL/glew.h>
#include "BulletDebugDrawer.h"

BulletDebugDrawer::BulletDebugDrawer(unsigned int shaderProgram)
	: m_debugMode(btIDebugDraw::DBG_DrawWireframe), m_shader(shaderProgram), m_vao(0u), m_vbo(0u) {
	// Setup the VAO and VBO for debug drawing
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW); // Initial buffer, no data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

BulletDebugDrawer::~BulletDebugDrawer() {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0u;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0u;
	}
}

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)  {
	GLfloat line[] = {
		from.x(), from.y(), from.z(), color.x(), color.y(), color.z(),
		to.x(), to.y(), to.z(), color.x(), color.y(), color.z()
	};
	lineVertices.insert(lineVertices.end(), line, line + 12);
}

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) {
	GLfloat line[] = {
		from.x(), from.y(), from.z(), fromColor.x(), fromColor.y(), fromColor.z(),
		to.x(), to.y(), to.z(), toColor.x(), toColor.y(), toColor.z()
	};
	lineVertices.insert(lineVertices.end(), line, line + 12);
}

void BulletDebugDrawer::flushLines() {
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	// Update the buffer with all lines
	glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(GLfloat), lineVertices.data(), GL_DYNAMIC_DRAW);

	// Use shader program to draw lines
	glUseProgram(m_shader);
	// Set uniforms as necessary for the shader program
	glDrawArrays(GL_LINES, 0, lineVertices.size() / 6);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Clear the line vertices for the next update
	lineVertices.clear();
}

int BulletDebugDrawer::getDebugMode() const { 
	return m_debugMode; 
}

void BulletDebugDrawer::setDebugMode(int debugMode) {
	m_debugMode = debugMode;
}