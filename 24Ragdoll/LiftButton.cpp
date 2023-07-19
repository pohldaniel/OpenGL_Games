#include "LiftButton.h"
#include "Globals.h"

LiftButton::LiftButton() {

}

LiftButton::~LiftButton() {

}


void LiftButton::updateHeight() {

	btTransform& transform = m_collisionObject->getWorldTransform();
	//transform.getOrigin()[1] = m_height + height;
	m_collisionObject->setWorldTransform(transform);
	setPosition(Physics::VectorFrom(transform.getOrigin()));
}

void LiftButton::create(btCollisionShape* shape, const btTransform& transform, btDynamicsWorld* physicsWorld, int collisionFilterGroup, int collisionFilterMask, void* rigidBodyUserPointer) {
	m_collisionObject = new btCollisionObject();

	m_collisionObject->setCollisionShape(shape);
	m_collisionObject->setWorldTransform(transform);

	m_collisionObject->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	m_collisionObject->forceActivationState(DISABLE_DEACTIVATION);

	physicsWorld->addCollisionObject(m_collisionObject, collisionFilterGroup, collisionFilterMask);
	//physicsWorld->setInternalTickCallback(TickCallback, this, true);

	setPosition(Physics::VectorFrom(transform.getOrigin()));
}

void LiftButton::draw(const Camera& camera) {
	glDisable(GL_CULL_FACE);
	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_model", getTransformationP());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix() * GetTransformation()));
	Globals::textureManager.get("lava").bind(0);
	Globals::shapeManager.get("platform").drawRaw();
	shader->unuse();
	glEnable(GL_CULL_FACE);
}

void LiftButton::TickCallback(btDynamicsWorld* world, btScalar timeStep) {
	//Lava* platform = (Lava*)world->getWorldUserInfo();
	//platform->updateHeight();
}

btCollisionObject* LiftButton::getCollisionObject() {
	return m_collisionObject;
}