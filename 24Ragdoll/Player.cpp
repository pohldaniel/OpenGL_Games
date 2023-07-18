#include "Player.h"
#include "Application.h"
#include "Globals.h"

Player::Player(Camera& camera) : camera(camera), m_fade(true), m_prevFraction(1.0f), m_useGravity(true) { }

Player::~Player() {
	// will be deleted at Physics.cpp
	//delete m_characterController;
}

void Player::init() {
	m_color.set(1.0f, 1.0f, 1.0f, 1.0f);
	m_position = Vector3f(0.0f, 0.0f, 0.0f);

	//create dynamic character
	btSphereShape* playerShape = new btSphereShape(0.5f);
	btTransform playerTransform;
	playerTransform.setIdentity();
	playerTransform.setOrigin(Physics::VectorFrom(m_position));
	btVector3 localInertiaChar(0, 0, 0);
	playerShape->calculateLocalInertia(100.0f, localInertiaChar);


	btDefaultMotionState* playerMotionState = new btDefaultMotionState(playerTransform);
	btRigidBody::btRigidBodyConstructionInfo cInfoChar(100.0f, playerMotionState, playerShape, localInertiaChar);

	m_characterController = new CharacterController();
	m_characterController->create(new btRigidBody(cInfoChar), Physics::GetDynamicsWorld(), Physics::collisiontypes::CHARACTER, Physics::collisiontypes::FLOOR, this);

	m_characterController->setSlopeAngle(60.0f);
	m_characterController->setJumpDistanceOffset(RADIUS + 0.1f);
	m_characterController->setOnGroundDistanceOffset(RADIUS + 0.01f);

	m_characterController->setAngularFactor(btVector3(1.0f, 0.0f, 1.0f));
	m_characterController->setSleepingThresholds(0.0f, 0.0f);
	m_characterController->setRollingFriction(1.0f);
	m_characterController->setDamping(0.0f, 0.7f);
	m_characterController->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));
	m_characterController->setGravity(btVector3(0.0f, -9.81f * 3.0f, 0.0f));

	//it seems the contactPairTest isn't right at the moment so I have to use a smaller sphere for this test
	m_collisionObject = new btCollisionObject();
	m_collisionObject->setCollisionShape(new btSphereShape(0.25f));
	m_collisionObject->setWorldTransform(playerTransform);
	m_collisionObject->setUserPointer(this);

	m_collisionObject->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	m_collisionObject->forceActivationState(DISABLE_DEACTIVATION);

	Physics::GetDynamicsWorld()->addCollisionObject(m_collisionObject, Physics::collisiontypes::CHARACTER, Physics::collisiontypes::TRIGGER);
}

void Player::draw(const Camera& camera) {


	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_model", getTransformationOP());
	
	Globals::textureManager.get("player").bind(0);

	Globals::shapeManager.get("sphere").drawRaw();
	shader->unuse();

}

void Player::update(const float dt) {
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();

	Vector3f direction = Vector3f();
	m_move = false;
	
	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 0.0f, 1.0f);
		m_move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, 0.0f, -1.0f);
		m_move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		direction += Vector3f(-0.5f, 0.0f, 0.0f);
		m_move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(0.5f, 0.0f, 0.0f);
		m_move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		direction += Vector3f(0.0f, -1.0f, 0.0f);
		m_move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		direction += Vector3f(0.0f, 1.0f, 0.0f);
		m_move |= true;
	}

	if (keyboard.keyPressed(Keyboard::KEY_F10)) {
		m_useGravity = !m_useGravity;
		m_characterController->setGravity(btVector3(0.0f, static_cast<float>(m_useGravity) * (-9.81f * 3.0f), 0.0f));
	}

	if (mouse.buttonDown(Mouse::BUTTON_RIGHT)) {
		m_useGravity ? m_characterController->setLinearVelocityXZ(Physics::VectorFrom(direction * Vector3f(0.0f, 0.0f, 0.0f))) : m_characterController->setLinearVelocity(Physics::VectorFrom(direction * Vector3f(0.0f, 0.0f, 0.0f)));
		m_characterController->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));

	}else if (m_move) {
		direction = camera.getViewSpaceDirection(direction);
		m_useGravity ? m_characterController->setLinearVelocityXZ(Physics::VectorFrom(direction * Vector3f(15.0f, 15.0f, 15.0f))) : m_characterController->setLinearVelocity(Physics::VectorFrom(direction * Vector3f(15.0f, 15.0f, 15.0f)));
	}else if(!m_useGravity) {
		m_characterController->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		m_characterController->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
	}

	if (keyboard.keyPressed(Keyboard::KEY_LALT)) {
		m_characterController->jump(btVector3(0.0f, 1.0f, 0.0f), 20.0f);	
	}

	btTransform t;
	m_characterController->getWorldTransform(t);
	Vector3f playerPos = Physics::VectorFrom(t.getOrigin());

	Object::setPosition(playerPos);
	Object::setOrientation(Physics::QuaternionFrom(t.getRotation()));

	camera.Camera::setTarget(playerPos);

	float dx = 0.0f;
	float dy = 0.0f;
	dx = mouse.xPosRelative();
	dy = mouse.yPosRelative();


	if (dx || dy) {
		camera.rotate(dx, dy, playerPos);
	}

	btVector3 cameraPosition = Physics::VectorFrom(camera.getPosition());

	float fraction = Physics::SweepSphere(t.getOrigin(), cameraPosition, 0.2f, Physics::collisiontypes::CAMERA, Physics::collisiontypes::FLOOR);

	if (m_prevFraction < fraction) {
		m_prevFraction += CAMERA_SMOOTHING_SPEED * dt;
		if (m_prevFraction > fraction) m_prevFraction = fraction;
	}else {
		m_prevFraction = fraction;
	}

	cameraPosition.setInterpolate3(t.getOrigin(), cameraPosition, m_prevFraction);
	camera.setPosition(Physics::VectorFrom(cameraPosition));

	t.setRotation(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f));
	m_collisionObject->setWorldTransform(t);
}

void Player::fixedUpdate(const float fdt) {
	m_color.set(1.0f, 1.0f, 1.0f, 1.0f);
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

bool Player::isMoving() {
	return m_move;
}

void Player::setColor(const Vector4f &color) {
	m_color = color;
}

btCollisionObject* Player::getContactObject() {
	return m_collisionObject;
}

void Player::setFade(bool fade) {
	m_fade = fade;
}