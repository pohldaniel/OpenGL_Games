#include <GL/glew.h>
#include <engine/input/Mouse.h>
#include <engine/input/Keyboard.h>
#include <engine/BuiltInShader.h>
#include <Utils/SolidIO.h>
#include "CharacterSkinned.h"
#include "KinematicCharacterController.h"
#include "Lift.h"
#include "Globals.h"

CharacterSkinned::CharacterSkinned(KinematicCharacterController* kcc, SceneNodeLC* button, Lift* lift, Camera& camera) : m_onGround(true), m_okToJump(true), m_jumpStarted(false), m_inAirTimer(0.0f), m_jumpTimer(0.0f), weaponActionState_(Weapon_Invalid),
comboAnimsIdx_(0), equipWeapon(false), lMouseB(false), m_kinematicController(kcc), m_lift(lift), camera(camera) {
	m_model.loadModelMdl("res/models/Girlbot/Girlbot.mdl");
	m_model.m_meshes[0]->createBones();

	m_animationNode = new AnimationNode(m_model);
	m_animationNode->setUpdateSilent(true);
	m_animationController = new AnimationController(m_animationNode);

	// combo anims
	weaponComboAnim_.push_back("girl_slash_1");
	weaponComboAnim_.push_back("girl_slash_2");
	weaponComboAnim_.push_back("girl_slash_3");

	weaponActionState_ = Weapon_Unequipped;
	m_armorLocatorNode = m_animationNode->getRootBone();
	m_rightHandLocatorNode = m_animationNode->getRootBone()->FindChildOfType(StringHash("RighthandLocator"), true);
	BoneNode* m_backLocatorNode = m_animationNode->getRootBone()->FindChildOfType(StringHash("BackLocator"), true);
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

	m_characterTriggerResult.m_button = button;
	m_kinematicController->setUserPointer(this);
}

CharacterSkinned::~CharacterSkinned() {
	delete m_sword;
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
	m_onGround = m_kinematicController->OnGround();

	if (keyboard.keyDown(Keyboard::KEY_W))
		moveDir += Vector3f::BACK;
	if (keyboard.keyDown(Keyboard::KEY_S))
		moveDir += Vector3f::FORWARD;
	if (keyboard.keyDown(Keyboard::KEY_A))
		moveDir += Vector3f::LEFT;
	if (keyboard.keyDown(Keyboard::KEY_D))
		moveDir += Vector3f::RIGHT;

	unsigned int prevState = weaponActionState_;

	processWeaponAction(equipWeapon, lMouseB);
	equipWeapon = false;
	lMouseB = false;

	if (weaponActionState_ == Weapon_AttackAnim) {
		m_onGround = m_jumpTimer == 0.0f;
		return;
	}

	// Normalize move vector so that diagonal strafing is not faster
	if (moveDir.lengthSq() > 0.0f)
		Vector3f::Normalize(moveDir);

	// rotate movedir
	m_curMoveDir = Quaternion::Rotate(rot, moveDir);

	m_kinematicController->SetWalkDirection(m_curMoveDir * (softGrounded ? MOVE_FORCE : INAIR_MOVE_FORCE));

	if (softGrounded){
		m_isJumping = false;
		// Jump. Must release jump control between jumps
		if (keyboard.keyDown(Keyboard::KEY_SPACE)){
			m_isJumping = true;
			if (m_okToJump){
				m_okToJump = false;
				m_jumpStarted = true;
				m_kinematicController->Jump();

				m_animationController->StopLayer(0);
				m_animationController->PlayExclusive("girl_jump_start", 0, false, 0.2f);
				m_animationController->SetTime("girl_jump_start", 0);
			}
		}else{
			m_okToJump = true;
		}
	}

	if (!m_onGround || m_jumpStarted) {
		if (m_jumpStarted) {
			if (m_animationController->IsAtEnd("girl_jump_start")) {
				m_animationController->PlayExclusive("girl_jump_loop", 0, true, 0.3f);
				m_animationController->SetTime("girl_jump_loop", 0);
				m_jumpStarted = false;
			}
		}else {
			const float maxDistance = 50.0f;
			const float segmentDistance = 10.01f;
			PhysicsRaycastResult result;

			Physics::RaycastSingleSegmented(result, m_animationNode->getPosition(), Vector3f::DOWN, maxDistance, segmentDistance, Physics::collisiontypes::RAY, Physics::collisiontypes::FLOOR);

			//float slopeDot = result.normal_.DotProduct(Vector3(0.0f, 1.0f, 0.0f));

			if (result.distance_ > 0.7f) {
				m_animationController->PlayExclusive("girl_jump_loop", 0, true, 0.2f);
			}
			//else if (result.body_ == NULL)
			//{
			//    // fall to death animation
			//}
		}
	}else {
		// Play walk animation if moving on ground, otherwise fade it out
		if ((softGrounded) && !moveDir.compare(Vector3f::ZERO, EPSILON)) {
			m_animationController->PlayExclusive("girl_run", 0, true, 0.2f);
		}else {
			m_animationController->PlayExclusive("girl_idle", 0, true, 0.2f);
		}
	}
}

void CharacterSkinned::update(const float dt) {
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();

	equipWeapon = keyboard.keyPressed(Keyboard::KEY_Q) | equipWeapon;
	lMouseB = mouse.buttonPressed(Mouse::BUTTON_LEFT) | lMouseB;
	m_animationController->Update(dt);
	m_animationNode->update(dt);
}

void CharacterSkinned::processWeaponAction(bool equip, bool lMouseB) {
	queInput_.Update();

	// eval state
	switch (weaponActionState_) {

	case Weapon_Unequipped:
		if (equip) {
			weaponActionAnim_ = "girl_unsheath";
			m_animationController->Play(weaponActionAnim_, WeaponLayer, false, 0.0f);
			m_animationController->SetTime(weaponActionAnim_, 0.0f);
			m_sword->eraseSelf();
			m_rightHandLocatorNode->addChild(m_sword, true);
			m_sword->OnTransformChanged();
			weaponActionState_ = Weapon_Equipping;
		}
		break;

	case Weapon_Equipping:
		if (lMouseB && queInput_.Empty()) {

			queInput_.SetInput(Mouse::BUTTON_LEFT);
		}

		m_animationController->Play(weaponActionAnim_, WeaponLayer, false, 0.1f);

		if (m_animationController->IsAtEnd(weaponActionAnim_)) {

			if (queInput_.Empty()) {
				m_animationController->PlayExclusive("girl_equiped_idle", WeaponLayer, true, 0.1f);
			}
			weaponActionState_ = Weapon_Equipped;
		}
		break;
	case Weapon_Equipped:
		if (equip) {

			weaponActionAnim_ = "girl_sheath";
			m_animationController->Play(weaponActionAnim_, WeaponLayer, false, 0.1f);
			m_animationController->SetTime(weaponActionAnim_, 0.0f);
			weaponActionState_ = Weapon_UnEquipping;
			break;
		}

		if (lMouseB || !queInput_.Empty()) {
			if (m_onGround) {
				// reset input queue
				queInput_.Reset();

				weaponActionAnim_ = weaponComboAnim_[comboAnimsIdx_];

				if (m_animationController->PlayExclusive(weaponActionAnim_, NormalLayer, false, 0.1f)) {
					m_animationController->SetTime(weaponActionAnim_, 0.0f);
					m_animationController->StopLayer(WeaponLayer);

					weaponActionState_ = Weapon_AttackAnim;
				}
			}
			else {
				if (lMouseB && queInput_.Empty()) {
					queInput_.SetInput(Mouse::BUTTON_LEFT);
				}
			}
		}
		break;
	case Weapon_UnEquipping:
		m_animationController->Play(weaponActionAnim_, WeaponLayer, false, 0.1f);
		if (m_animationController->IsAtEnd(weaponActionAnim_)) {
			m_animationController->StopLayer(WeaponLayer, 0.2f);
			m_sword->eraseSelf();
			m_swordLocatorNode->addChild(m_sword, true);
			m_sword->OnTransformChanged();
			weaponActionState_ = Weapon_Unequipped;
		}
		break;
	case Weapon_AttackAnim:
		if (lMouseB && queInput_.Empty()) {
			queInput_.SetInput(Mouse::BUTTON_LEFT);
		}

		m_animationController->PlayExclusive(weaponActionAnim_, NormalLayer, false, 0.1f);
		AnimationState* state = m_animationController->GetAnimationState(StringHash(weaponActionAnim_));

		if (m_animationController->IsAtEnd(weaponActionAnim_)) {

			if (queInput_.Empty()) {
				comboAnimsIdx_ = 0;
				m_animationController->PlayExclusive("girl_equiped_idle", WeaponLayer, true, 0.1f);
			}
			else {
				comboAnimsIdx_ = ++comboAnimsIdx_ % weaponComboAnim_.size();
			}

			weaponActionState_ = Weapon_Equipped;
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

void CharacterSkinned::rotate(const float pitch, const float yaw, const float roll) {
	m_animationNode->rotate(pitch, yaw, roll);
}

const Vector3f& CharacterSkinned::getWorldPosition() const {
	return m_animationNode->getWorldPosition();
}

void CharacterSkinned::setPosition(const Vector3f& position) {
	m_animationNode->setPosition(position);
}

void CharacterSkinned::fixedPostUpdate(float fdt) {
	if (m_movingData[0] == m_movingData[1]) {
		Matrix4f delta = m_movingData[0].transform_ * m_movingData[1].transform_.inverse();
		// add delta
		Vector3f kPos;
		Quaternion kRot;
		m_kinematicController->GetTransform(kPos, kRot);

		Matrix4f matKC(kPos, kRot, Vector3f::ONE);
		matKC = delta * matKC;
		m_kinematicController->SetTransform(matKC.getTranslation(), Quaternion(matKC.getRotation()));

		float deltaYaw = Quaternion(delta.getRotation()).getYaw();
		m_animationNode->rotate(Quaternion(Vector3f::UP, deltaYaw));
		camera.rotate(-deltaYaw * 1.0f / camera.getRotationSpeed(), 0.0f, kPos);
	}

	// update node position
	m_animationNode->setPosition(m_kinematicController->GetPosition());

	// shift and clear
	m_movingData[1] = m_movingData[0];
	m_movingData[0].node_ = 0;
}

void CharacterSkinned::nodeOnMovingPlatform(SceneNodeLC *node) {
	m_movingData[0].node_ = node;
	m_movingData[0].transform_ = node->getWorldTransformation();
}

void CharacterSkinned::processCollision() {
	m_characterTriggerResultButton.prevCollision.first = m_characterTriggerResultButton.currentCollision.first;
	m_characterTriggerResultButton.prevCollision.second = m_characterTriggerResultButton.currentCollision.second;
}

void CharacterSkinned::handleCollision(btCollisionObject* collisionObject) {
	Physics::GetDynamicsWorld()->contactPairTest(m_kinematicController->pairCachingGhostObject_.get(), collisionObject, m_characterTriggerResult);
}

void CharacterSkinned::handleCollisionButton(btCollisionObject* collisionObject) {
	m_characterTriggerResultButton.currentCollision = std::make_pair(nullptr, nullptr);
	Physics::GetDynamicsWorld()->contactPairTest(m_kinematicController->pairCachingGhostObject_.get(), collisionObject, m_characterTriggerResultButton);
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