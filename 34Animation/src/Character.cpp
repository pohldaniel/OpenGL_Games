#include <GL/glew.h>
#include <engine/BuiltInShader.h>
#include "Character.h"

#include "Globals.h"

Character::Character() : m_onGround(true), m_okToJump(true), m_jumpStarted(false), m_isJumping(false), m_inAirTimer(0.0f) {
	//m_model.loadModelMdl("res/models/BetaLowpoly/Beta.mdl");
	//m_model.m_meshes[0]->m_meshBones[0].initialPosition.translate(0.0f, 0.0f, 0.0f);
	//m_model.m_meshes[0]->m_meshBones[0].initialRotation.rotate(0.0f, 0.0f, 0.0f);
	//m_model.m_meshes[0]->createBones();
	//m_model.m_meshes[0]->m_rootBone->SetAnimationEnabled(false);
	//Globals::textureManager.get("null").bind();

	m_model.loadModelAssimp("res/models/woman/Woman.gltf", 2, false);
	m_model.m_meshes[0]->m_meshBones[1].initialScale.scale(0.007f, 0.007f, 0.007f);
	m_model.m_meshes[0]->m_meshBones[1].initialPosition.translate(0.0f, 0.0f, 0.0f);
	m_model.m_meshes[0]->m_meshBones[1].initialRotation.rotate(Vector3f(0.0f, 1.0f, 0.0f), 180.0f);
	m_model.m_meshes[0]->createBones();
	m_model.m_meshes[0]->m_rootBone->SetAnimationEnabled(false);	
	Globals::textureManager.get("woman").bind();

	m_animationController = new AnimationController(&m_model);
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
	const Quaternion& rot = m_model.getWorldOrientation();
	Vector3f moveDir = Vector3f::ZERO;
	//m_onGround = kinematicController_->OnGround();
	m_onGround = true;

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
		moveDir.normalize();

	m_model.translateRelative(moveDir * MOVE_SPEED);

	// rotate movedir
	//m_curMoveDir = Quaternion::Rotate(rot, moveDir * MOVE_SPEED);
	//m_model.translate(m_curMoveDir);

	if (softGrounded){
		m_isJumping = false;
		if (keyboard.keyDown(Keyboard::KEY_SPACE)){
			m_isJumping = true;
			if (m_okToJump){
				m_okToJump = false;
				m_jumpStarted = true;
				m_animationController->StopLayer(0);
				//m_animationController->PlayExclusive("beta_jump_start", 0, false, 0.2f);
				//m_animationController->SetTime("beta_jump_start", 0);
				m_animationController->PlayExclusive("woman_jump_2", 0, false, 0.2f);
				m_animationController->SetTime("woman_jump_2", 0);
			}
		}else{
			m_okToJump = true;
		}
	}

	if (!m_onGround || m_jumpStarted){
		if (m_jumpStarted){
			/*if (m_animationController->IsAtEnd("beta_jump_start")){
				m_animationController->PlayExclusive("beta_jump_loop", 0, true, 0.3f);
				m_animationController->SetTime("beta_jump_loop", 0);
				m_jumpStarted = false;
			}*/

			if (m_animationController->IsAtEnd("woman_jump_2")) {
				m_animationController->PlayExclusive("woman_jump_2", 0, true, 0.3f);
				m_animationController->SetTime("woman_jump_2", 0);
				m_jumpStarted = false;
			}

		}else{
			const float maxDistance = 50.0f;
			const float segmentDistance = 10.01f;
		}
	}else{
		if ((softGrounded) && !moveDir.compare(Vector3f::ZERO, EPSILON)){
			//m_animationController->PlayExclusive("beta_run", 0, true, 0.2f);
			m_animationController->PlayExclusive("woman_run", 0, true, 0.2f);
		}else{
			//m_animationController->PlayExclusive("beta_idle", 0, true, 0.2f);
			m_animationController->PlayExclusive("woman_idle", 0, true, 0.2f);
		}
	}
}

void Character::update(const float dt) {	
	m_animationController->Update(dt);
	m_model.update(dt);
	m_model.updateSkinning();
}

void Character::draw(const Camera& camera) {
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * m_model.m_meshes[0]->m_numBones, m_model.m_meshes[0]->m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadVector("u_light", Vector3f(1.0f, 1.0f, 1.0f));
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	m_model.drawRaw();

	shader->unuse();
}