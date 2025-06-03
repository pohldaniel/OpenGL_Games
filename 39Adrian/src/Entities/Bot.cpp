#include "Bot.h"

Bot::Bot(const Md2Model& md2Model) : Md2Entity(md2Model) {
	m_start.set(620.0f, 0.0f, -380.0f);
	m_end.set(820.0f, 0.0f, -280.0f);
	m_currentWaypoint = m_end;
	setAnimationType(AnimationType::RUN);
	setSpeed(0.3f);
	m_moveSpeed = 35.0f;
}

Bot::~Bot() {

}

void Bot::update(const float dt) {
	Md2Node::update(dt);

	if (m_moveSpeed == 0.0f)
		return;

	followPath(dt);
}

void Bot::followPath(float dt) {
	Vector3f nextWaypoint = m_currentWaypoint;
	Vector3f pos = getWorldPosition();

	float move = m_moveSpeed * dt;
	float distance = (pos - nextWaypoint).length();

	if (move > distance)
		move = distance;

	setOrientation(Quaternion(pos, nextWaypoint));
	translateRelative(Vector3f::FORWARD * move);

	if (distance < 0.2f) {
		m_currentWaypoint = m_start;
		m_start = m_end;
		m_end = m_currentWaypoint;
	}
}

void Bot::setStart(const Vector3f& start) {
	m_start = start;
}

void Bot::setEnd(const Vector3f& end) {
	m_end = end;
	m_currentWaypoint = end;
}

void Bot::setStart(const float x, const float y, const float z) {
	m_start.set(x, y, z);
}

void Bot::setEnd(const float x, const float y, const float z) {
	m_end.set(x, y, z);
	m_currentWaypoint.set(x, y, z);
}

void Bot::setMoveSpeed(const float moveSpeed) {
	m_moveSpeed = moveSpeed;
}