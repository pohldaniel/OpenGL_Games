#include <GL/glew.h>
#include <engine/input/Mouse.h>
#include <engine/input/Keyboard.h>
#include <engine/BuiltInShader.h>
#include <Utils/SolidIO.h>
#include "CharacterSkinned.h"
#include "KinematicCharacterController.h"
#include "Lift.h"
#include "Globals.h"

CharacterSkinned::CharacterSkinned(const AnimatedModel& ainamtedModel, Lift* lift, Camera& camera) :
	m_onGround(true), 
	m_okToJump(true), 
	m_jumpStarted(false),
	m_inAirTimer(0.0f), 
	m_weaponActionState(Weapon_Invalid),
	m_comboAnimsIdx(0), 
	m_equipWeapon(false), 
	m_lMouseB(false), 
	m_lift(lift), 
	camera(camera), 
	m_characterTriggerResultWeapon(m_damageTimer), 
	m_weaponDmgState(WeaponDmg_OFF) {

	m_animationNode = new AnimationNode(ainamtedModel);
	m_animationNode->setUpdateSilent(true);
	m_animationController = new AnimationController(m_animationNode);

	// combo anims
	m_weaponComboAnim.push_back("girl_slash_1");
	m_weaponComboAnim.push_back("girl_slash_2");
	m_weaponComboAnim.push_back("girl_slash_3");

	m_weaponActionState = Weapon_Unequipped;
	m_armorLocatorNode = m_animationNode->getRootBone();
	m_rightHandLocatorNode = m_animationNode->getRootBone()->findChild<BoneNode>("RighthandLocator", true);
	BoneNode* m_backLocatorNode = m_animationNode->getRootBone()->findChild<BoneNode>("BackLocator", true);
	m_swordLocatorNode = m_backLocatorNode->addChild<BoneNode>();
	m_swordLocatorNode->setPosition(-2.52264f, 1.71661e-05f, 22.4404f);

	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;
	std::vector<std::vector<Utils::GeometryDesc>> geomDescs;
	std::vector<std::array<unsigned int, 4>> boneIds;
	std::vector<std::array<float, 4>> weights;
	std::vector<std::string> boneList;
	std::vector<MeshBone> meshBones;
	BoundingBox box;

	Utils::MdlIO mdlConverter;
	mdlConverter.mdlToBuffer("res/models/Girlbot/Armor.mdl", 1.0f, vertexBuffer, indexBuffer, weights, boneIds, geomDescs, meshBones, box);
	m_armorShape.fromBuffer(vertexBuffer, indexBuffer, 8, weights, boneIds);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit(); weights.clear(); weights.shrink_to_fit(); boneIds.clear(); boneIds.shrink_to_fit();
	mdlConverter.mdlToBuffer("res/models/Girlbot/Sword.mdl", 100.0f, vertexBuffer, indexBuffer, weights, boneIds, geomDescs, meshBones, box);

	m_swordShape = Shape(vertexBuffer, indexBuffer, 8);
	m_sword = new ShapeNode(m_swordShape);

	m_swordLocatorNode->addChild(m_sword, true);

	m_armorShape.markForDelete();
	m_swordShape.markForDelete();

	m_kinematicController = new KinematicCharacterController(nullptr, Physics::collisiontypes::CHARACTER, Physics::collisiontypes::FLOOR);
	m_kinematicController->setUserPointer(this);
}

CharacterSkinned::~CharacterSkinned() {
	
	delete m_kinematicController;
	delete m_animationController;

	m_sword->eraseSelf();
	delete m_sword; 
	delete m_animationNode;
}

void CharacterSkinned::fixedUpdate(float fdt) {
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
	// Velocity on the XZ plane
	//const Vector3f velocity = m_kinematicController->GetLinearVelocity();
	//Vector3f planeVelocity(velocity[0], 0.0f, velocity[2]);

	if (keyboard.keyDown(Keyboard::KEY_W))
		moveDir += Vector3f::BACK;
	if (keyboard.keyDown(Keyboard::KEY_S))
		moveDir += Vector3f::FORWARD;
	if (keyboard.keyDown(Keyboard::KEY_A))
		moveDir += Vector3f::LEFT;
	if (keyboard.keyDown(Keyboard::KEY_D))
		moveDir += Vector3f::RIGHT;

	m_weaponDmgState = WeaponDmg_OFF;
	unsigned int prevState = m_weaponActionState;
	processWeaponAction(m_equipWeapon, m_lMouseB);
	m_equipWeapon = false;
	m_lMouseB = false;
	
	if (m_weaponActionState == Weapon_AttackAnim) {
		float time = m_animationController->getTime(m_weaponActionAnim);
		if (m_comboAnimsIdx == 0 && 0.2f < time && time < 0.8f)
			m_weaponDmgState = WeaponDmg_ON;
		else if (m_comboAnimsIdx == 1 && 0.2f < time && time < 0.9f)
			m_weaponDmgState = WeaponDmg_ON;
		else if (m_comboAnimsIdx == 2 && 0.4f < time && time < 0.9f)
			m_weaponDmgState = WeaponDmg_ON;

		if (prevState == Weapon_Equipped){
			m_kinematicController->setLinearVelocity(Vector3f::ZERO);
		}

		m_onGround = false;
		return;
	}

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
				m_animationController->playExclusive("girl_jump_start", 0, false, 0.2f);
				m_animationController->setTime("girl_jump_start", 0);
			}
		}else{
			m_okToJump = true;
		}
	}

	if (!m_onGround || m_jumpStarted) {
		if (m_jumpStarted) {
			if (m_animationController->isAtEnd("girl_jump_start")) {
				m_animationController->playExclusive("girl_jump_loop", 0, true, 0.3f);
				m_animationController->setTime("girl_jump_loop", 0);
				m_jumpStarted = false;
			}
		}else {
			const float maxDistance = 50.0f;
			const float segmentDistance = 10.01f;
			PhysicsRaycastResult result;

			Physics::RaycastSingleSegmented(result, m_animationNode->getPosition(), Vector3f::DOWN, maxDistance, segmentDistance, Physics::collisiontypes::RAY, Physics::collisiontypes::FLOOR);
			if (result.distance_ > 0.7f) {
				m_animationController->playExclusive("girl_jump_loop", 0, true, 0.2f);
			}else if (result.body_ == NULL){
			    // fall to death animation
			}
		}
	}else {
		// Play walk animation if moving on ground, otherwise fade it out
		if ((softGrounded) && !moveDir.compare(Vector3f::ZERO, EPSILON)) {
			m_animationController->playExclusive("girl_run", 0, true, 0.2f);
		}else {
			m_animationController->playExclusive("girl_idle", 0, true, 0.2f);
		}

		// Set walk animation speed proportional to velocity
		 //float spd = Math::Clamp(planeVelocity.length() * 0.3f, 0.5f, 2.0f);
		 //m_animationController->SetSpeed("girl_run", spd);
	}

}

void CharacterSkinned::update(const float dt) {
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();

	m_equipWeapon = keyboard.keyPressed(Keyboard::KEY_Q) | m_equipWeapon;
	m_lMouseB = mouse.buttonPressed(Mouse::BUTTON_LEFT) | m_lMouseB;
	m_animationController->update(dt);
	m_animationNode->update(dt);
}

void CharacterSkinned::processWeaponAction(bool equip, bool lMouseB) {
	m_queInput.update();

	// eval state
	switch (m_weaponActionState) {

	case Weapon_Unequipped:
		if (equip) {
			m_weaponActionAnim = "girl_unsheath";
			m_animationController->play(m_weaponActionAnim, WeaponLayer, false, 0.0f);
			m_animationController->setTime(m_weaponActionAnim, 0.0f);
			m_sword->eraseSelf();
			m_rightHandLocatorNode->addChild(m_sword, true);
			m_sword->OnTransformChanged();
			m_weaponActionState = Weapon_Equipping;

		}
		break;

	case Weapon_Equipping:
		if (lMouseB && m_queInput.empty()) {
			m_queInput.setInput(Mouse::BUTTON_LEFT);
		}

		m_animationController->play(m_weaponActionAnim, WeaponLayer, false, 0.1f);

		if (m_animationController->isAtEnd(m_weaponActionAnim)) {
			if (m_queInput.empty()) {
				m_animationController->playExclusive("girl_equiped_idle", WeaponLayer, true, 0.1f);
			}
			m_weaponActionState = Weapon_Equipped;
		}
		break;
	case Weapon_Equipped:
		if (equip) {

			m_weaponActionAnim = "girl_sheath";
			m_animationController->play(m_weaponActionAnim, WeaponLayer, false, 0.1f);
			m_animationController->setTime(m_weaponActionAnim, 0.0f);
			m_weaponActionState = Weapon_UnEquipping;
			break;
		}

		if (lMouseB || !m_queInput.empty()) {
			if (m_onGround) {
				// reset input queue
				m_queInput.reset();

				m_weaponActionAnim = m_weaponComboAnim[m_comboAnimsIdx];

				if (m_animationController->playExclusive(m_weaponActionAnim, NormalLayer, false, 0.1f)) {
					m_animationController->setTime(m_weaponActionAnim, 0.0f);
					m_animationController->stopLayer(WeaponLayer);
					m_weaponActionState = Weapon_AttackAnim;
				}
			}
			else {
				if (lMouseB && m_queInput.empty()) {
					m_queInput.setInput(Mouse::BUTTON_LEFT);
				}
			}
		}
		break;
	case Weapon_UnEquipping:
		m_animationController->play(m_weaponActionAnim, WeaponLayer, false, 0.1f);
		if (m_animationController->isAtEnd(m_weaponActionAnim)) {
			m_animationController->stopLayer(WeaponLayer, 0.2f);
			m_sword->eraseSelf();
			m_swordLocatorNode->addChild(m_sword, true);
			m_sword->OnTransformChanged();
			m_weaponActionState = Weapon_Unequipped;
			
		}
		break;
	case Weapon_AttackAnim:
		if (lMouseB && m_queInput.empty()) {
			m_queInput.setInput(Mouse::BUTTON_LEFT);
		}

		m_animationController->playExclusive(m_weaponActionAnim, NormalLayer, false, 0.1f);
		AnimationState* state = m_animationController->getAnimationState(StringHash(m_weaponActionAnim));

		if (m_animationController->isAtEnd(m_weaponActionAnim)) {

			if (m_queInput.empty()) {
				m_comboAnimsIdx = 0;
				m_animationController->playExclusive("girl_equiped_idle", WeaponLayer, true, 0.1f);
			}else {
				m_comboAnimsIdx = ++m_comboAnimsIdx % m_weaponComboAnim.size();
			}

			m_weaponActionState = Weapon_Equipped;
		}

		break;
	}
}

void CharacterSkinned::draw(const Camera& camera) {

	Globals::textureManager.get("null").bind();
	auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadVector("u_light", Vector3f(1.0f, 1.0f, 1.0f));
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadVector("u_color", Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	m_animationNode->drawRaw();

	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_model", m_sword->getWorldTransformation());
	Globals::textureManager.get("sword").bind(0);
	m_sword->getShape().drawRaw();
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadVector("u_light", Vector3f(1.0f, 1.0f, 1.0f));
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Globals::textureManager.get("sword").bind(0);
	m_armorShape.drawRaw();
	shader->unuse();
}

void CharacterSkinned::drawOverview(const Matrix4f perspective, const Matrix4f view) {
	Globals::textureManager.get("null").bind();
	auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadVector("u_light", Vector3f(1.0f, 1.0f, 1.0f));
	shader->loadMatrix("u_projection", perspective);
	shader->loadMatrix("u_view", view);
	shader->loadVector("u_color", Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	m_animationNode->drawRaw();

	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", perspective);
	shader->loadMatrix("u_view", view);
	shader->loadMatrix("u_model", m_sword->getWorldTransformation());
	Globals::textureManager.get("sword").bind(0);
	m_sword->getShape().drawRaw();
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadVector("u_light", Vector3f(1.0f, 1.0f, 1.0f));
	shader->loadMatrix("u_projection", perspective);
	shader->loadMatrix("u_view", view);
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Globals::textureManager.get("sword").bind(0);
	m_armorShape.drawRaw();
	shader->unuse();
}

void CharacterSkinned::rotate(const float pitch, const float yaw, const float roll) {
	m_animationNode->rotate(pitch, yaw, roll);
}

const Vector3f& CharacterSkinned::getWorldPosition() const {
	return m_animationNode->getWorldPosition();
}

const btVector3 CharacterSkinned::getBtPosition() const {
	return m_kinematicController->getBtPosition();
}

void CharacterSkinned::setPosition(const Vector3f& position) {
	m_animationNode->setPosition(position);
	Vector3f offset = Quaternion::Rotate(m_swordLocatorNode->getWorldOrientation(), Vector3f(0.0f, 0.0f, -0.65f));
	m_swordBody = Physics::AddRigidBody(0.0f, Physics::BtTransform(Physics::VectorFrom(m_swordLocatorNode->getWorldPosition() + offset), Physics::QuaternionFrom(m_swordLocatorNode->getWorldOrientation(false))), new btBoxShape(btVector3(0.25f, 0.15f, 1.2f) * 0.5f), Physics::collisiontypes::SWORD, Physics::collisiontypes::DUMMY, btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	m_swordBody->forceActivationState(DISABLE_DEACTIVATION);
	m_swordBody->setUserPointer(this);
}

void CharacterSkinned::fixedPostUpdate(float fdt) {
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
	if (m_swordLocatorNode->getChildren().size()) {
		Vector3f offset = Quaternion::Rotate(m_swordLocatorNode->getWorldOrientation(), Vector3f(0.0f, 0.0f, -0.65f));
		m_swordBody->getMotionState()->setWorldTransform(Physics::BtTransform(Physics::VectorFrom(m_swordLocatorNode->getWorldPosition() + offset), Physics::QuaternionFrom(m_swordLocatorNode->getWorldOrientation(false))));
	}else {
		Vector3f offset = Quaternion::Rotate(m_rightHandLocatorNode->getWorldOrientation(), Vector3f(0.0f, 0.0f, -0.65f));
		m_swordBody->getMotionState()->setWorldTransform(Physics::BtTransform(Physics::VectorFrom(m_rightHandLocatorNode->getWorldPosition() + offset), Physics::QuaternionFrom(m_rightHandLocatorNode->getWorldOrientation(false))));
	}
	
	// shift and clear
	m_movingData[1] = m_movingData[0];
	m_movingData[0].m_node = nullptr;
}

void CharacterSkinned::nodeOnMovingPlatform(SceneNodeLC *node) {
	m_movingData[0].m_node = node;
	m_movingData[0].m_transform = node->getWorldTransformation();
}

void CharacterSkinned::processCollision() {
	m_characterTriggerResultButton.prevCollision.first = m_characterTriggerResultButton.currentCollision.first;
	m_characterTriggerResultButton.prevCollision.second = m_characterTriggerResultButton.currentCollision.second;
}

void CharacterSkinned::handleCollision(btCollisionObject* collisionObject) {
	Physics::GetDynamicsWorld()->contactPairTest(m_kinematicController->getPairCachingGhostObject(), collisionObject, m_characterTriggerResult);
}

void CharacterSkinned::handleCollisionButton(btCollisionObject* collisionObject) {
	m_characterTriggerResultButton.currentCollision = std::make_pair(nullptr, nullptr);
	Physics::GetDynamicsWorld()->contactPairTest(m_kinematicController->getPairCachingGhostObject(), collisionObject, m_characterTriggerResultButton);
}

void CharacterSkinned::handleCollisionWeapon(btCollisionObject* collisionObject) {

	// exit if not in the proper state
	if (m_weaponDmgState == WeaponDmg_OFF){
		return;
	}

	Physics::GetDynamicsWorld()->contactPairTest(m_swordBody, collisionObject, m_characterTriggerResultWeapon);
}

void CharacterSkinned::beginCollision() {
	if (m_characterTriggerResultButton.currentCollision.first && m_characterTriggerResultButton.prevCollision.second == nullptr) {
		Lift* lift = reinterpret_cast<Lift*>(m_characterTriggerResultButton.currentCollision.second->getUserPointer());
		lift->handleButtonStartCollision();
	}
}

void CharacterSkinned::endCollision() {
	if (m_characterTriggerResultButton.currentCollision.first == nullptr && m_characterTriggerResultButton.prevCollision.second) {
		Lift* lift = reinterpret_cast<Lift*>(m_characterTriggerResultButton.prevCollision.second->getUserPointer());
		lift->handleButtonEndCollision();
	}
}

btScalar CharacterSkinned::CharacterTriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
	CharacterSkinned* characterSkinned = reinterpret_cast<CharacterSkinned*>(colObj0Wrap->getCollisionObject()->getUserPointer());
	SceneNodeLC* model = reinterpret_cast<SceneNodeLC*>(colObj1Wrap->getCollisionObject()->getUserPointer());
	characterSkinned->nodeOnMovingPlatform(model);
	return 0;
}

btScalar CharacterSkinned::CharacterTriggerCallbackButton::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
	currentCollision.first = colObj0Wrap->getCollisionObject();
	currentCollision.second = colObj1Wrap->getCollisionObject();
	return 0;
}

btScalar CharacterSkinned::CharacterTriggerCallbackWeapon::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
	//CharacterSkinned* characterSkinned = reinterpret_cast<CharacterSkinned*>(colObj0Wrap->getCollisionObject()->getUserPointer());
	
	if(damageTimer.getElapsedTimeMilli() > 400u)
		damageTimer.reset();
	return 0;
}

const Vector4f CharacterSkinned::getDummyColor() const {
	return m_damageTimer.getElapsedTimeMilli() < 400u ? Vector4f(1.0f, 0.0f, 0.0f, 1.0f) : Vector4f(0.70064f, 0.23256f, 0.0f, 1.0f);
}