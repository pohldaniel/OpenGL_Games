#include "CharacterController_old.h"

CharacterControllerOld::CharacterControllerOld(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	m_postion = b2Vec2(200.0f, 600.0f);

	b2BodyDef playerDef;
	playerDef.fixedRotation = true;
	playerDef.type = b2_kinematicBody;
	playerDef.position = m_postion;

	m_body = Globals::world->CreateBody(&playerDef);

	b2PolygonShape boundingBox;
	boundingBox.SetAsBox(15.0f, 15.0f);
	b2FixtureDef playerFixture;
	playerFixture.shape = &boundingBox;
	playerFixture.friction = 0.0f;
	playerFixture.density = 1.0f;
	playerFixture.userData.pointer = 1;
	m_body->CreateFixture(&playerFixture);
}

CharacterControllerOld::~CharacterControllerOld() {

}

void CharacterControllerOld::render() {


#if DEBUGCOLLISION
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&Globals::projection[0][0]);
	if (m_callback.m_hit) {
		m_target = m_callback.m_point;
		glBegin(GL_LINES);
		glColor3f(0, 1, 0);
		glVertex3f(m_postion.x, m_postion.y, 0.0f);
		glVertex3f(m_target.x, m_target.y, 0.0f);
		glEnd();

	}
	else {
		glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3f(m_postion.x, m_postion.y, 0.0f);
		glVertex3f(m_target.x, m_target.y, 0.0f);
		glEnd();
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	b2Vec2 position = m_body->GetPosition();
	b2PolygonShape *boxShape = static_cast<b2PolygonShape*>(m_body->GetFixtureList()->GetShape());

	b2Vec2 v1 = boxShape->m_vertices[0];
	b2Vec2 v2 = boxShape->m_vertices[1];
	b2Vec2 v3 = boxShape->m_vertices[2];
	b2Vec2 v4 = boxShape->m_vertices[3];

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

#endif
}

void CharacterControllerOld::update() {
	if (Globals::CONTROLLS & Globals::KEY_A) {
		m_callback.reset();
		m_target = m_postion - b2Vec2(m_distance, 0.0f);
		Globals::world->RayCast(&m_callback, m_postion, m_target);

		float distance = m_distance;
		if (m_callback.m_hit) {
			float ajustedDistance = m_callback.m_fraction * distance;
			distance = ajustedDistance < distance ? ajustedDistance : distance;

			if (distance < m_skinWidth) {
				m_postion = m_postion + b2Vec2(m_skinWidth - distance, 0.0f);
				m_velocity.x = 0.0f;
			}
		}
		else {
			m_velocity.x = -m_movementSpeed;
		}

	}
	else if (Globals::CONTROLLS & Globals::KEY_D) {
		m_callback.reset();
		m_target = m_postion + b2Vec2(m_distance, 0.0f);
		Globals::world->RayCast(&m_callback, m_postion, m_target);

		float distance = m_distance;
		if (m_callback.m_hit) {
			float ajustedDistance = m_callback.m_fraction * distance;
			distance = ajustedDistance < distance ? ajustedDistance : distance;
			if (distance < m_skinWidth) {
				m_postion = m_postion - b2Vec2(m_skinWidth - distance, 0.0f);
				m_velocity.x = 0.0f;
			}
		}
		else {
			m_velocity.x = m_movementSpeed;
		}
	}
	else {
		//immediately stop 
		m_velocity.x = 0.0f;
	}

	if (Globals::CONTROLLS & Globals::KEY_W && m_grounded) {
		m_grounded = false;

		m_callback.reset();
		m_target = m_postion + b2Vec2(0.0f, m_distance);
		Globals::world->RayCast(&m_callback, m_postion, m_target);
		float distance = m_distance;
		if (m_callback.m_hit) {
			float ajustedDistance = m_callback.m_fraction * distance;
			distance = ajustedDistance < distance ? ajustedDistance : distance;
			if (distance < m_skinWidth) {
				m_postion = m_postion - b2Vec2(0.0f, m_skinWidth - distance);
				m_velocity.y = 0.0f;
			}
		}
		else {
			m_velocity.y = m_jumpHeight;
		}
	}

	bool bottomLeft = false;
	bool bottomRight = false;
	float fraction = 0.0f;

	m_callback.reset();
	m_target = m_postion - b2Vec2(m_skinWidth - 1.0f, 0.0f) - (b2Vec2(0.0f, m_distance));
	Globals::world->RayCast(&m_callback, m_postion - b2Vec2(m_skinWidth - 1.0f, 0.0f), m_target);
	bottomLeft = m_callback.m_hit;
	fraction = m_callback.m_fraction;

	m_callback.reset();
	m_target = m_postion + b2Vec2(m_skinWidth - 1.0f, 0.0f) - (b2Vec2(0.0f, m_distance));
	Globals::world->RayCast(&m_callback, m_postion + b2Vec2(m_skinWidth - 1.0f, 0.0f), m_target);
	bottomRight = m_callback.m_hit;
	fraction = std::max(fraction, m_callback.m_fraction);

	float distance = m_distance;
	if (bottomLeft || bottomRight) {
		float ajustedDistance = fraction * distance;
		distance = ajustedDistance < distance ? ajustedDistance : distance;
		if (distance < m_skinWidth) {
			//correct position
			m_postion = m_postion + b2Vec2(0.0f, (m_skinWidth - distance));
			//reset velocity
			m_velocity.y = 0.0f;
			m_grounded = true;
		}
	}
}

void CharacterControllerOld::fixedUpdate() {
	move();
}

void CharacterControllerOld::move() {
	m_velocity.y -= m_gravity * m_fdt;
	m_postion = m_postion + (m_fdt * m_velocity);
	m_body->SetTransform(m_postion, 0.0f);
}