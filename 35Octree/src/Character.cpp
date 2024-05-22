#include <iostream>
#include <engine/scene/AnimationNode.h>

#include "Character.h"
#include "KinematicCharacterController.h"
#include "MovingPlatform.h"
#include "Lift.h"
#include "Globals.h"

Character::Character(const AnimatedModel& ainamtedModel, Lift* lift, Camera& camera)
	: m_onGround(false),
	m_okToJump(true),
	m_inAirTimer(0.0f),
	m_jumpStarted(false),
	camera(camera) {

	m_animationNode = new AnimationNode(ainamtedModel);
	m_animationNode->setUpdateSilent(true);
	m_animationController = new AnimationController(m_animationNode);

	m_kinematicController = new KinematicCharacterController(nullptr, Physics::collisiontypes::CHARACTER, Physics::collisiontypes::FLOOR);
	m_kinematicController->setUserPointer(this);
	m_lift = lift;
}

Character::~Character() {
	delete m_kinematicController;
	delete m_animationController;
	delete m_animationNode;
}

void Character::draw(const Camera& camera) {
	Globals::textureManager.get("null").bind();
	auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadVector("u_light", Vector3f(1.0f, 1.0f, 1.0f));
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadVector("u_color", Vector4f(0.371822f, 0.75123f, 0.9091f, 1.0f));
	m_animationNode->drawRaw();
	shader->unuse();
}

void Character::drawOverview(const Matrix4f perspective, const Matrix4f view) {
	Globals::textureManager.get("null").bind();
	auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadVector("u_light", Vector3f(1.0f, 1.0f, 1.0f));
	shader->loadMatrix("u_projection", perspective);
	shader->loadMatrix("u_view", view);
	shader->loadVector("u_color", Vector4f(0.371822f, 0.75123f, 0.9091f, 1.0f));
	m_animationNode->drawRaw();

	shader->unuse();
}

void Character::rotate(const float pitch, const float yaw, const float roll) {
	m_animationNode->rotate(pitch, yaw, roll);
}

const Vector3f& Character::getWorldPosition() const {
	return m_animationNode->getWorldPosition();
}

const btVector3 Character::getBtPosition() const {
	return m_kinematicController->getBtPosition();
}

void Character::update(const float dt) {
	m_animationController->update(dt);
	m_animationNode->update(dt);
}

void Character::fixedUpdate(float fdt) {
	Keyboard &keyboard = Keyboard::instance();

	// Update the in air timer. Reset if grounded
	if (!m_onGround)
		m_inAirTimer += fdt;
	else
		m_inAirTimer = 0.0f;
	// When character has been in air less than 1/10 second, it's still interpreted as being on ground
	bool softGrounded = m_inAirTimer < INAIR_THRESHOLD_TIME;

	// Update movement & animation
	const Quaternion& rot = m_animationNode->getOrientation();
	Vector3f moveDir = Vector3f::ZERO;
	m_onGround = m_kinematicController->onGround();

	if (keyboard.keyDown(Keyboard::KEY_W))
		moveDir += Vector3f::BACK;
	if (keyboard.keyDown(Keyboard::KEY_S))
		moveDir += Vector3f::FORWARD;
	if (keyboard.keyDown(Keyboard::KEY_A))
		moveDir += Vector3f::LEFT;
	if (keyboard.keyDown(Keyboard::KEY_D))
		moveDir += Vector3f::RIGHT;

	// Normalize move vector so that diagonal strafing is not faster
	if (moveDir.lengthSq() > 0.0f)
		Vector3f::Normalize(moveDir);

	// rotate movedir
	m_curMoveDir = Quaternion::Rotate(rot, moveDir);

	m_kinematicController->setWalkDirection(m_curMoveDir * (softGrounded ? MOVE_FORCE : INAIR_MOVE_FORCE));

	if (softGrounded){
		m_isJumping = false;
		// Jump. Must release jump control between jumps
		if (keyboard.keyDown(Keyboard::KEY_SPACE)){
			m_isJumping = true;
			if (m_okToJump){
				m_okToJump = false;
				m_jumpStarted = true;
				m_kinematicController->jump();

				m_animationController->stopLayer(0);
				m_animationController->playExclusive("beta_jump_start", 0, false, 0.2f);
				m_animationController->setTime("beta_jump_start", 0);
			}
		}else{
			m_okToJump = true;
		}
	}

	if (!m_onGround || m_jumpStarted){
		if (m_jumpStarted){
			if (m_animationController->isAtEnd("beta_jump_start")){
				m_animationController->playExclusive("beta_jump_loop", 0, true, 0.3f);
				m_animationController->setTime("beta_jump_loop", 0);
				m_jumpStarted = false;
			}
		}else{
			const float maxDistance = 50.0f;
			const float segmentDistance = 10.01f;
			PhysicsRaycastResult result;

			Physics::RaycastSingleSegmented(result, m_animationNode->getPosition(), Vector3f::DOWN, maxDistance, segmentDistance, Physics::collisiontypes::RAY, Physics::collisiontypes::FLOOR);

			//float slopeDot = result.normal_.DotProduct(Vector3(0.0f, 1.0f, 0.0f));

			if (result.distance_ > 0.7f) {
				m_animationController->playExclusive("beta_jump_loop", 0, true, 0.2f);
			}
			//else if (result.body_ == NULL)
			//{
			//    // fall to death animation
			//}
		}
	}else{
		// Play walk animation if moving on ground, otherwise fade it out
		if ((softGrounded) && !moveDir.compare(Vector3f::ZERO, EPSILON)){
			m_animationController->playExclusive("beta_run", 0, true, 0.2f);
		}else{
			m_animationController->playExclusive("beta_idle", 0, true, 0.2f);
		}
	}
}

void Character::fixedPostUpdate(float fdt) {
	if (m_movingData[0] == m_movingData[1]) {
		Matrix4f delta = m_movingData[0].m_transform * m_movingData[1].m_transform.inverse();
		// add delta
		Vector3f kPos;
		Quaternion kRot;
		m_kinematicController->getTransform(kPos, kRot);

		Matrix4f matKC(kPos, kRot, Vector3f::ONE);	
		matKC = delta * matKC;
		m_kinematicController->setTransform(matKC.getTranslation(), Quaternion(matKC.getRotation()));

		float deltaYaw = Quaternion(delta.getRotation()).getYaw();
		m_animationNode->rotate(Quaternion(Vector3f::UP, deltaYaw));
		camera.rotate(-deltaYaw * 1.0f / camera.getRotationSpeed(), 0.0f, kPos);	
	}

	// update node position
	m_animationNode->setPosition(m_kinematicController->getPosition());

	// shift and clear
	m_movingData[1] = m_movingData[0];
	m_movingData[0].m_node = nullptr;
}

void Character::nodeOnMovingPlatform(SceneNodeLC* node) {
	m_movingData[0].m_node = node;
	m_movingData[0].m_transform = node->getWorldTransformation();
}

void Character::processCollision() {
	m_characterTriggerResultButton.prevCollision.first = m_characterTriggerResultButton.currentCollision.first;
	m_characterTriggerResultButton.prevCollision.second = m_characterTriggerResultButton.currentCollision.second;
}

void Character::handleCollision(btCollisionObject* collisionObject) {
	Physics::GetDynamicsWorld()->contactPairTest(m_kinematicController->getPairCachingGhostObject(), collisionObject, m_characterTriggerResult);
}

void Character::handleCollisionButton(btCollisionObject* collisionObject) {
	m_characterTriggerResultButton.currentCollision = std::make_pair(nullptr, nullptr);
	Physics::GetDynamicsWorld()->contactPairTest(m_kinematicController->getPairCachingGhostObject(), collisionObject, m_characterTriggerResultButton);
}

void Character::beginCollision() {
	if (m_characterTriggerResultButton.currentCollision.first && m_characterTriggerResultButton.prevCollision.second == nullptr) {
		Lift* lift = reinterpret_cast<Lift*>(m_characterTriggerResultButton.currentCollision.second->getUserPointer());
		lift->handleButtonStartCollision();
	}
}

void Character::endCollision() {
	if (m_characterTriggerResultButton.currentCollision.first == nullptr && m_characterTriggerResultButton.prevCollision.second) {
		Lift* lift = reinterpret_cast<Lift*>(m_characterTriggerResultButton.prevCollision.second->getUserPointer());
		lift->handleButtonEndCollision();
	}
}

btScalar Character::CharacterTriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
	Character* character = reinterpret_cast<Character*>(colObj0Wrap->getCollisionObject()->getUserPointer());
	SceneNodeLC* model = reinterpret_cast<SceneNodeLC*>(colObj1Wrap->getCollisionObject()->getUserPointer());
	character->nodeOnMovingPlatform(model);
	return 0;
}

btScalar Character::CharacterTriggerCallbackButton::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
	currentCollision.first = colObj0Wrap->getCollisionObject();
	currentCollision.second = colObj1Wrap->getCollisionObject();
	return 0;
}