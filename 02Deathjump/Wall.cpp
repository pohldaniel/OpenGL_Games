#include "Wall.h"

Wall::Wall(Vector2f position, Vector2f size) {
	initCollider(position, size);
}

Wall::~Wall() {

}

Collision Wall::getCollider() {
	return m_collider;
}

void Wall::initCollider(const Vector2f& position, const Vector2f& size) {
	m_collider.size = size;
	m_collider.position = position;
}

void Wall::render() {
	//Debug colider
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&Globals::projection[0][0]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor3f(1, 1, 0);

	float xpos = m_collider.position[0];
	float ypos = m_collider.position[1];
	float w = m_collider.size[0];
	float h = m_collider.size[1];

	glVertex3f(xpos, HEIGHT - ypos, 0.0f);
	glVertex3f(xpos, HEIGHT - (ypos + h), 0.0f);
	glVertex3f(xpos + w, HEIGHT - (ypos + h), 0.0f);
	glVertex3f(xpos + w, HEIGHT - ypos, 0.0f);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
