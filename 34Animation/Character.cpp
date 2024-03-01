#include <GL/glew.h>
#include <engine/BuiltInShader.h>
#include "Character.h"

#include "Globals.h"

Character::Character() : m_onGround(true), m_okToJump(true), m_inAirTimer(0.0f) {
	m_model.loadModelMdl("res/models/BetaLowpoly/Beta.mdl");
	m_model.m_meshes[0]->m_meshBones[0].initialPosition.translate(0.0f, 0.0f, 0.0f);
	m_model.m_meshes[0]->m_meshBones[0].initialRotation.rotate(0.0f, 0.0f, 0.0f);
	m_model.m_meshes[0]->createBones();

	m_animationController = new AnimationController(&m_model);
	//m_currentAnimation = { "woman_walk",  0.0f, m_animationController->GetAnimationState(StringHash("woman_walk")) };
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

	if (keyboard.keyDown(Keyboard::KEY_UP))
		moveDir += Vector3f::BACK;
	if (keyboard.keyDown(Keyboard::KEY_DOWN))
		moveDir += Vector3f::FORWARD;
	if (keyboard.keyDown(Keyboard::KEY_LEFT))
		moveDir += Vector3f::LEFT;
	if (keyboard.keyDown(Keyboard::KEY_RIGHT))
		moveDir += Vector3f::RIGHT;

	// Normalize move vector so that diagonal strafing is not faster
	if (moveDir.lengthSq() > 0.0f)
		moveDir.normalize();

	// rotate movedir
	m_curMoveDir = Quaternion::Rotate(rot, moveDir);

	if (softGrounded){
		m_isJumping = false;
		// Jump. Must release jump control between jumps
		if (keyboard.keyDown(Keyboard::KEY_SPACE)){
			m_isJumping = true;
			if (m_okToJump){
				m_okToJump = false;
				m_jumpStarted = true;
				//kinematicController_->Jump();

				m_animationController->StopLayer(0);
				m_animationController->PlayExclusive("beta_jump_start", 0, false, 0.2f);
				m_animationController->SetTime("beta_jump_start", 0);
			}
		}else{
			m_okToJump = true;
		}
	}

	if (!m_onGround || m_jumpStarted){
		if (m_jumpStarted){
			if (m_animationController->IsAtEnd("beta_jump_start")){
				m_animationController->PlayExclusive("beta_jump_loop", 0, true, 0.3f);
				m_animationController->SetTime("beta_jump_loop", 0);
				m_jumpStarted = false;
			}

		}else{
			const float maxDistance = 50.0f;
			const float segmentDistance = 10.01f;
			//PhysicsRaycastResult result;

			//Physics::RaycastSingleSegmented(result, Ray(model_->Position(), Vector3::DOWN), maxDistance, segmentDistance, Physics::collisiontypes::RAY, Physics::collisiontypes::FLOOR);

			//float slopeDot = result.normal_.DotProduct(Vector3(0.0f, 1.0f, 0.0f));

			//if (result.distance_ > 0.7f) {
				//m_animationController->PlayExclusive("Beta/Beta_JumpLoop1.ani", 0, true, 0.2f);
			//}			
		}
	}else{
		if ((softGrounded) && !moveDir.compare(Vector3f::ZERO, EPSILON)){
			m_animationController->PlayExclusive("beta_run", 0, true, 0.2f);
		}else{
			m_animationController->PlayExclusive("beta_idle", 0, true, 0.2f);
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

	Globals::textureManager.get("null").bind();
	m_model.drawRaw();

	shader->unuse();
}