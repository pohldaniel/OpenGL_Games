#include "Player.h"

Player::Player() {
	//Create our box2D body
	b2BodyDef playerDef;
	playerDef.fixedRotation = true;
	playerDef.type = b2_dynamicBody;
	playerDef.position.Set(2 * 30 + 15, 15 * 30 + 15);
	//playerDef.position.Set(16, 16);
	m_playerBody = Globals::world->CreateBody(&playerDef);

	b2PolygonShape boundingBox;
	boundingBox.SetAsBox(15.0f, 15.0f);
	b2FixtureDef playerFixture;
	playerFixture.friction = 0.0f;
	playerFixture.shape = &boundingBox;
	playerFixture.density = 1.0f;
	m_playerBody->CreateFixture(&playerFixture);
}

void Player::render() {

	#if DEBUGCOLLISION
	b2Vec2 position = m_playerBody->GetPosition();
	b2PolygonShape *boxShape = static_cast<b2PolygonShape*>(m_playerBody->GetFixtureList()->GetShape());
	
	b2Vec2 v1 = boxShape->m_vertices[0];
	b2Vec2 v2 = boxShape->m_vertices[1];
	b2Vec2 v3 = boxShape->m_vertices[2];
	b2Vec2 v4 = boxShape->m_vertices[3];

	//std::cout << position.x << "  " << position.y << std::endl;

	Matrix4f transProj = Globals::projection.transpose();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&transProj[0][0]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor3f(1, 1, 0);

	//left bottom corner
	float xpos = position.x + v1.x;
	float ypos = position.y + v1.y;
	float w = v2.x - v1.x;
	float h = v4.y - v1.y;

	glVertex3f(xpos, ypos, 0.0f);
	glVertex3f(xpos, (ypos + h), 0.0f);
	glVertex3f(xpos + w, (ypos + h), 0.0f);
	glVertex3f(xpos + w, ypos, 0.0f);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	#endif*/
}