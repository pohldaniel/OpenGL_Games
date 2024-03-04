#include <GL/glew.h>
#include <engine/input/Mouse.h>
#include <engine/input/Keyboard.h>
#include <engine/BuiltInShader.h>
#include <Utils/SolidIO.h>
#include "CharacterSkinned.h"

#include "Globals.h"

CharacterSkinned::CharacterSkinned() : m_onGround(true), m_okToJump(true), m_jumpStarted(false), m_inAirTimer(0.0f), m_jumpTimer(0.0f), weaponActionState_(Weapon_Invalid),
comboAnimsIdx_(0), equipWeapon(false), lMouseB(false){
	m_model.loadModelMdl("res/models/Girlbot/Girlbot.mdl");
	m_model.m_meshes[0]->m_meshBones[0].initialPosition.translate(0.0f, 0.0f, 0.0f);
	m_model.m_meshes[0]->m_meshBones[0].initialRotation.rotate(0.0f, 0.0f, 0.0f);
	m_model.m_meshes[0]->createBones();
	m_model.m_meshes[0]->m_rootBone->SetAnimationEnabled(false);
	m_animationController = new AnimationController(&m_model);

	// combo anims
	weaponComboAnim_.push_back("girl_slash_1");
	weaponComboAnim_.push_back("girl_slash_2");
	weaponComboAnim_.push_back("girl_slash_3");

	weaponActionState_ = Weapon_Unequipped;
	m_armorLocatorNode = m_model.m_meshes[0]->m_rootBone;
	m_rightHandLocatorNode = m_model.m_meshes[0]->m_rootBone->FindChildOfType(StringHash("RighthandLocator"), true);
	Bone* m_backLocatorNode = m_model.m_meshes[0]->m_rootBone->FindChildOfType(StringHash("BackLocator"), true);
	m_swordLocatorNode = m_backLocatorNode->addChild<Bone>();
	m_swordLocatorNode->setPosition(-2.52264f, 1.71661e-05f, 22.4404f);

	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;
	std::vector<std::vector<Utils::GeometryDesc>> geomDescs;
	std::vector<std::array<unsigned int, 4>> boneIds;
	std::vector<std::array<float, 4>> weights;
	std::vector<std::string> boneList;
	std::vector<ModelBone> meshBones;
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
}

CharacterSkinned::~CharacterSkinned() {
	delete m_sword;
}

void CharacterSkinned::fixedUpdate(float fdt) {
	Keyboard &keyboard = Keyboard::instance();

	m_jumpTimer = std::max(0.0f, m_jumpTimer - fdt);

	if (!m_onGround)
		m_inAirTimer += fdt;
	else
		m_inAirTimer = 0.0f;

	bool softGrounded = m_inAirTimer < INAIR_THRESHOLD_TIME;

	const Quaternion& rot = m_model.getWorldOrientation();
	Vector3f moveDir = Vector3f::ZERO;

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
		moveDir.normalize();

	m_model.translateRelative(moveDir * MOVE_SPEED * fdt);

	if (softGrounded) {
		if (keyboard.keyDown(Keyboard::KEY_SPACE)) {
			if (m_okToJump) {
				m_jumpStarted = true;
				m_okToJump = false;	
				m_jumpTimer = JUMP_TIMER;
				m_animationController->StopLayer(0);
				m_animationController->PlayExclusive("girl_jump_start", 0, false, 0.2f);
				m_animationController->SetTime("girl_jump_start", 0);
			}
		}else {
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
		}
		else {
			const float maxDistance = 50.0f;
			const float segmentDistance = 10.01f;
		}
	}else {
		if ((softGrounded) && !moveDir.compare(Vector3f::ZERO, EPSILON)) {
			m_animationController->PlayExclusive("girl_run", 0, true, 0.2f);
		}else {
			m_animationController->PlayExclusive("girl_idle", 0, true, 0.2f);
		}
	}
	m_onGround = m_jumpTimer == 0.0f;
}

void CharacterSkinned::update(const float dt) {
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();

	equipWeapon = keyboard.keyPressed(Keyboard::KEY_Q) | equipWeapon;
	lMouseB = mouse.buttonPressed(Mouse::BUTTON_LEFT) | lMouseB;

	m_animationController->Update(dt);
	m_model.update(dt);
	m_model.updateSkinning();
}

void CharacterSkinned::processWeaponAction(bool equip, bool lMouseB) {
	queInput_.Update();
	
	// eval state
	switch (weaponActionState_){

		case Weapon_Unequipped:
		if (equip){
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
		if (lMouseB && queInput_.Empty()){
			
			queInput_.SetInput(Mouse::BUTTON_LEFT);
		}

		m_animationController->Play(weaponActionAnim_, WeaponLayer, false, 0.1f);

		if (m_animationController->IsAtEnd(weaponActionAnim_)){

			if (queInput_.Empty()){
				m_animationController->PlayExclusive("girl_equiped_idle", WeaponLayer, true, 0.1f);
			}
			weaponActionState_ = Weapon_Equipped;
		}
		break;
		case Weapon_Equipped:
			if (equip){
				
				weaponActionAnim_ = "girl_sheath";
				m_animationController->Play(weaponActionAnim_, WeaponLayer, false, 0.1f);
				m_animationController->SetTime(weaponActionAnim_, 0.0f);
				weaponActionState_ = Weapon_UnEquipping;
				break;
			}

			if (lMouseB || !queInput_.Empty()){
				if (m_onGround){
					// reset input queue
					queInput_.Reset();
					
					weaponActionAnim_ = weaponComboAnim_[comboAnimsIdx_];

					if (m_animationController->PlayExclusive(weaponActionAnim_, NormalLayer, false, 0.1f)){
						m_animationController->SetTime(weaponActionAnim_, 0.0f);
						m_animationController->StopLayer(WeaponLayer);

						weaponActionState_ = Weapon_AttackAnim;
					}
				}else{
					if (lMouseB && queInput_.Empty()){
						queInput_.SetInput(Mouse::BUTTON_LEFT);
					}
				}
			}
			break;
		case Weapon_UnEquipping:
			m_animationController->Play(weaponActionAnim_, WeaponLayer, false, 0.1f);
			if (m_animationController->IsAtEnd(weaponActionAnim_)){
				m_animationController->StopLayer(WeaponLayer, 0.2f);
				m_sword->eraseSelf();
				m_swordLocatorNode->addChild(m_sword, true);
				m_sword->OnTransformChanged();
				weaponActionState_ = Weapon_Unequipped;
			}
			break;
		case Weapon_AttackAnim:
			if (lMouseB && queInput_.Empty()){
				queInput_.SetInput(Mouse::BUTTON_LEFT);
			}

			m_animationController->PlayExclusive(weaponActionAnim_, NormalLayer, false, 0.1f);
			AnimationState* state = m_animationController->GetAnimationState(StringHash(weaponActionAnim_));

			if (m_animationController->IsAtEnd(weaponActionAnim_)){

				if (queInput_.Empty()){					
					comboAnimsIdx_ = 0;
					m_animationController->PlayExclusive("girl_equiped_idle", WeaponLayer, true, 0.1f);
				}else{
					comboAnimsIdx_ = ++comboAnimsIdx_ % weaponComboAnim_.size();
				}

				weaponActionState_ = Weapon_Equipped;
			}

			break;
	}
}

void CharacterSkinned::draw(const Camera& camera) {

	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * m_model.m_meshes[0]->m_numBones, m_model.m_meshes[0]->m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	Globals::textureManager.get("null").bind();
	auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadVector("u_light", Vector3f(1.0f, 1.0f, 1.0f));
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadVector("u_color", Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	m_model.drawRaw();

	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_model", m_sword->getWorldTransformation());
	Globals::textureManager.get("sword").bind(1);
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