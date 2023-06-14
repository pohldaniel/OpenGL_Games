#include "Player.h"
#include "Application.h"
#include "Terrain.h"
#include "Globals.h"

Player::Player(Camera& camera) : m_camera(camera) { }

Player::~Player() {

}

void Player::init(const Terrain& terrain) {

	m_pos = Vector3f(512.0f, (terrain.heightAt(512.0f, 512.0f) + RADIUS), 512.0f);

	//create dynamic character
	btSphereShape* playerShape = new btSphereShape(0.5f);
	btTransform playerTransform;
	playerTransform.setIdentity();
	playerTransform.setOrigin(btVector3(btScalar(m_pos[0]), btScalar(m_pos[1]), btScalar(m_pos[2])));
	btVector3 localInertiaChar(0, 0, 0);
	playerShape->calculateLocalInertia(100.0f, localInertiaChar);


	btDefaultMotionState* playerMotionState = new btDefaultMotionState(playerTransform);
	btRigidBody::btRigidBodyConstructionInfo cInfoChar(100.0f, playerMotionState, playerShape, localInertiaChar);

	m_characterController = new CharacterController();
	m_characterController->create(new btRigidBody(cInfoChar), Physics::GetDynamicsWorld(), Physics::collisiontypes::CHARACTER, Physics::collisiontypes::TERRAIN, this);

	m_characterController->setSlopeAngle(60.0f);
	m_characterController->setJumpDistanceOffset(RADIUS + 0.1f);
	m_characterController->setOnGroundDistanceOffset(RADIUS + 0.01f);

	m_characterController->setAngularFactor(btVector3(1.0f, 0.0f, 1.0f));
	m_characterController->setSleepingThresholds(0.0f, 0.0f);
	m_characterController->setRollingFriction(1.0f);
	m_characterController->setDamping(0.0f, 0.7f);
	m_characterController->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));
	m_characterController->setGravity(btVector3(0.0f, -9.81f * 3.0f, 0.0f));
}

void Player::draw(const Camera& camera) {

	auto shader = Globals::shaderManager.getAssetPointer("texture_new");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_model", getTransformationOP());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix() * getTransformationOP()));
	Globals::textureManager.get("player").bind(0);
	Globals::shapeManager.get("sphere").drawRaw();
	shader->unuse();

}

void Player::update(const float dt) {
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();

	Vector3f direction = Vector3f();
	bool move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		direction += Vector3f(-0.5f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(0.5f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		direction += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		direction += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	if (mouse.buttonDown(Mouse::BUTTON_RIGHT)) {
		m_characterController->setLinearVelocityXZ(btVector3(0.0f, 0.0f, 0.0f));
		m_characterController->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));

	}else if (move) {
		direction = m_camera.getViewSpaceDirection(direction);
		m_characterController->setLinearVelocityXZ(Physics::VectorFrom(direction * Vector3f(15.0f, 1.0f, 15.0f)));
	}

	if (keyboard.keyDown(Keyboard::KEY_LALT)) {
		m_characterController->jump(btVector3(0.0f, 1.0f, 0.0f), 20.0f);
	}

	btTransform t;
	m_characterController->getWorldTransform(t);
	Vector3f playerPos = Physics::VectorFrom(t.getOrigin());
	m_camera.Camera::setTarget(playerPos);

	float dx = 0.0f;
	float dy = 0.0f;
	dx = mouse.xPosRelative();
	dy = mouse.yPosRelative();


	if (dx || dy) {
		m_camera.rotate(dx, dy, playerPos);
	}

	btVector3 cameraPosition = Physics::VectorFrom(m_camera.getPosition());

	cameraPosition.setInterpolate3(t.getOrigin(), cameraPosition, Physics::SweepSphere(t.getOrigin(), cameraPosition, 0.2f, Physics::collisiontypes::CAMERA, Physics::collisiontypes::TERRAIN));
	m_camera.setPosition(Physics::VectorFrom(cameraPosition));

	Object::setPosition(playerPos);
	Object::setOrientation(Physics::QuaternionFrom(t.getRotation()));

}

CharacterController* Player::getCharacterController() {
	return m_characterController;
}

void Player::setPosition(const Vector3f &position) {
	m_characterController->setLinearVelocityXZ(btVector3(0.0f, 0.0f, 0.0f));
	m_characterController->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
	m_characterController->setPosition(Physics::VectorFrom(position));
}

void Player::setPosition(const float x, const float y, const float z) {
	m_characterController->setLinearVelocityXZ(btVector3(0.0f, 0.0f, 0.0f));
	m_characterController->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
	m_characterController->setPosition(btVector3(x, y, z));
}

void Player::resetOrientation() {
	m_characterController->resetOrientation();
}

Vector3f& Player::getInitialPosition() {
	return m_pos;
}