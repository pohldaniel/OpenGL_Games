#include <Physics/Shapedrawer.h>
#include "Bot.h"

Bot::Bot(const Md2Model& md2Model) : Md2Entity(md2Model) {
	m_start.set(620.0f, 0.0f, -380.0f);
	m_end.set(820.0f, 0.0f, -280.0f);
	m_currentWaypoint = m_end;
	setAnimationType(AnimationType::RUN);
	setSpeed(0.3f);
	m_moveSpeed = 35.0f;
	
	setRigidBody(Physics::AddKinematicRigidBody(Physics::BtTransform(Physics::VectorFrom(getWorldPosition())), new btBoxShape(btVector3(0.5f, 0.5f, 0.5f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER, nullptr, false));
}

Bot::~Bot() {

}

void Bot::fixedUpdate(float fdt) {
	const BoundingBox& aabb = getLocalBoundingBox();
	const Vector3f& pos = getWorldPosition();
	const Quaternion& rot = getWorldOrientation();
	const Vector3f size = aabb.getSize();
	const Vector3f pivot1(0.0f, aabb.min[1] + size[1] * 0.5f, 0.0f);
	const Vector3f pivot2(0.0f, 0.5f, 0.0f);

	m_rigidBody->setWorldTransform(Physics::BtTransform(pos + pivot1, rot));
	m_rigidBody->getCollisionShape()->setLocalScaling(Physics::VectorFrom(size * 0.75));
	//Physics::GetDynamicsWorld()->updateSingleAabb(m_rigidBody, true);

	m_segmentBody->setWorldTransform(Physics::BtTransform(pos + pivot2, rot));
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

void Bot::init(const Shape& shape) {
	m_segmentBody = Physics::AddKinematicRigidBody(Physics::BtTransform(Physics::VectorFrom(getWorldPosition())), Physics::CreateCollisionShape(&shape, btVector3(2.3f, 2.3f, 2.3f)), Physics::collisiontypes::SPHERE, Physics::collisiontypes::CHARACTER, nullptr, false);
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

void Bot::setRigidBody(btRigidBody* rigidBody) {
	m_rigidBody = rigidBody;
}

btRigidBody* Bot::getRigidBody() {
	return m_rigidBody;
}

btRigidBody* Bot::getSegmentRigidBody() {
	return m_segmentBody;
}