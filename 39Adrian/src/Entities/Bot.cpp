#include <engine/scene/ShapeNode.h>
#include <Physics/Shapedrawer.h>
#include "Bot.h"

Bot::Bot(const Md2Model& md2Model) : Md2Entity(md2Model) {
	m_start.set(620.0f, 0.0f, -380.0f);
	m_end.set(820.0f, 0.0f, -280.0f);
	m_currentWaypoint = m_end;
	setAnimationType(AnimationType::RUN);
	setSpeed(0.3f);
	m_moveSpeed = 35.0f;
	m_isInRange = false;
	m_isDeath = false;
	setRigidBody(Physics::AddKinematicRigidBody(Physics::BtTransform(Physics::VectorFrom(getWorldPosition())), new btBoxShape(btVector3(0.5f, 0.5f, 0.5f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER, this, false));
	m_rigidBody->setUserIndex(1);
	ShapeDrawer::Get().addToCache(m_rigidBody->getCollisionShape());
}

Bot::~Bot() {

}

void Bot::fixedUpdate(float fdt) {
	if (m_isDeath)
		return;

	const BoundingBox& aabb = getLocalBoundingBox();
	const Vector3f& pos = getWorldPosition();
	const Quaternion& rot = getWorldOrientation();
	const Vector3f size = aabb.getSize();
	const Vector3f pivot1(0.0f, aabb.min[1] + size[1] * 0.5f, 0.0f);

	m_rigidBody->setWorldTransform(Physics::BtTransform(pos + pivot1, rot));
	m_rigidBody->getCollisionShape()->setLocalScaling(Physics::VectorFrom(size * 0.75));
	m_segmentBody->setWorldTransform(Physics::BtTransform(pos, rot));
	m_triggerBody->setWorldTransform(Physics::BtTransform(pos, rot));
	m_isInRange = false;
}

void Bot::update(const float dt) {
	Md2Node::update(dt);

	if (m_moveSpeed == 0.0f)
		return;

	followPath(dt);
}

void Bot::followPath(float dt) {

	if (m_isDeath)
		return;

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
	m_segmentBody = Physics::AddKinematicRigidBody(Physics::BtTransform(Physics::VectorFrom(getWorldPosition())), Physics::CreateCollisionShape(&shape, btVector3(2.3f, 2.3f, 2.3f)), Physics::collisiontypes::ENEMY, Physics::collisiontypes::TRIGGER_2, nullptr, false);
	m_triggerBody = Physics::AddKinematicRigidBody(Physics::BtTransform(Physics::VectorFrom(getWorldPosition())), new btCylinderShape(btVector3(20.0f, 1.0f, 0.0f)), Physics::collisiontypes::ENEMY, Physics::collisiontypes::TRIGGER_1, this, false);
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

btRigidBody* Bot::getSegmentBody() {
	return m_segmentBody;
}


btRigidBody* Bot::getTriggerBody() {
	return m_triggerBody;
}


void Bot::handleCollision(btCollisionObject* collisionObject) {
	if (!m_isDeath)
		Physics::GetDynamicsWorld()->contactPairTest(m_triggerBody, collisionObject, m_triggerResult);
}

void Bot::setEnemyType(EnemyType enemyType) {
	m_enemyType = enemyType;
}

EnemyType Bot::getEnemyType() {
	return m_enemyType;
}

void Bot::setIsInRange(bool isInRange) {
	m_isInRange = isInRange;
}

bool Bot::isInRange() {
	return m_isInRange;
}

btScalar Bot::TriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
	Bot* bot = reinterpret_cast<Bot*>(colObj0Wrap->getCollisionObject()->getUserPointer());
	bot->setIsInRange(true);
	return 0;	
}

void Bot::death() {
	setAnimationType(AnimationType::DEATH_BACK);
	setLoopAnimation(false);
	m_isDeath = true;

	ShapeNode* shape = findChild<ShapeNode>("disk");
	if (shape) {
		shape->OnOctreeSet(nullptr);
		shape->eraseSelf();
	}

	shape = findChild<ShapeNode>("segment");
	if (shape) {
		shape->OnOctreeSet(nullptr);
		shape->eraseSelf();
	}

	Physics::DeleteCollisionObject(getRigidBody());
}

bool Bot::isDeath() {
	return m_isDeath;
}