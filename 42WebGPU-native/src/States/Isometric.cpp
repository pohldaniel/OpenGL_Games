#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include <Nuklear/NkContext.h>
#include <Nuklear/NkStyle.h>

#include <engine/scene/CollisionNode.h>
#include <engine/sound/SoundDevice.h>
#include <engine/sound/AudioEffect.h>

#include <Entities/CollisionEntity.h>
#include <Entities/Enemy.h>
#include <Entities/Player.h>

#include "Isometric.h"
#include "Application.h"
#include "Globals.h"

//For getting this matrices load the model with 
// importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, true); 
// and call printAiHierarchy(pScene->mRootNode);.
// The used ones are Gun_$AssimpFbx$_RotationOffset, Gun_$AssimpFbx$_RotationPivot and Gun_$AssimpFbx$_RotationPivotInverse
Matrix4f offset = Matrix4f(1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	-156.85f, -32.2427f, 144.702f, 1.0f);

Matrix4f pivot = Matrix4f(1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	130.762f, 70.4033f, -3.52485f, 1.0f);

Matrix4f invPivot = Matrix4f(1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	-130.762f, -70.4033f, 3.52485f, 1.0f);

ThreadPool threadPool(4);
const int spreadAmount = 10;

Isometric::Isometric(StateMachine& machine) : State(machine, States::ISOMETRIC), m_bulletStore(&threadPool), m_enemySpawner(120.0f * 0.0044f, m_player) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);
	wgpSetSurfaceDepthFormat(WGPUTextureFormat::WGPUTextureFormat_Depth24Plus, Application::OnSurfaceChange);
	Physics::DebugDrawer.init();
	nkInit(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
	nkInitFont("res/fonts/upheavtt.ttf");

	AnimationManager::Get().getAnimation("full").loadAnimationAssimp("res/models/Player.fbx", "Player", "full", 0u, 245u);
	AnimationManager::Get().getAnimation("idle").loadAnimationAssimp("res/models/Player.fbx", "Player", "idle", 5u, 81u);
	AnimationManager::Get().getAnimation("forward").loadAnimationAssimp("res/models/Player.fbx", "Player", "forward", 85u, 105u);
	AnimationManager::Get().getAnimation("backward").loadAnimationAssimp("res/models/Player.fbx", "Player", "backward", 110u, 130u);
	AnimationManager::Get().getAnimation("backward").shift(10u);
	AnimationManager::Get().getAnimation("right").loadAnimationAssimp("res/models/Player.fbx", "Player", "right", 135u, 155u);
	AnimationManager::Get().getAnimation("right").shift(10u);
	AnimationManager::Get().getAnimation("left").loadAnimationAssimp("res/models/Player.fbx", "Player", "left", 160u, 180u);
	AnimationManager::Get().getAnimation("death").loadAnimationAssimp("res/models/Player.fbx", "Player", "death", 185u, 244u);

	m_player.loadModelAssimp("res/models/Player.fbx", 1u);
	m_player.scale(0.0044f, 0.0044f, 0.0044f);

	m_rotationButtonResult.degrees = aimTheta * _180_ON_PI;

	m_enemy.loadModel("res/models/EelDog/EelDog.fbx");
	m_enemy.rotate(90.0f, 0.0f, 0.0f);
	m_enemy.rotate(0.0f, 0.0f, 0.0f);
	m_enemy.scale(0.01f);

	Material::CleanupMaterials();
	static_cast<const AssimpMesh*>(m_enemy.getMesh())->setMaterialIndex(-1);

	//Add additional nodes to the first Mesh, they are presented inside the Animation channels but not at the bone hierarchy from the model.
	AnimatedMesh* mesh = static_cast<AnimatedMesh*>(m_player.mesh());

	mesh->boneDescriptions().emplace_back();
	mesh->boneDescriptions().back().name = "Gun_$AssimpFbx$_Rotation";
	mesh->boneDescriptions().back().parentIndex = -1;
	mesh->boneDescriptions().back().offsetMatrix = invPivot;

	mesh->boneDescriptions().emplace_back();
	mesh->boneDescriptions().back().name = "Gun_$AssimpFbx$_Translation";
	mesh->boneDescriptions().back().parentIndex = 0;
	mesh->boneDescriptions().back().offsetMatrix = offset * pivot;

	mesh->createBones();

	mesh = static_cast<AnimatedMesh*>(m_player.mesh(1u));
	for (size_t index = 0u; index < mesh->getVertexBuffer().size() / mesh->getStride(); index++) {
		mesh->weights().push_back({ 1.0f, 0.0f, 0.0f, 0.0f });
		mesh->joints().push_back({ 42u, 0u, 0u, 0u });
	}

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f,  -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 4.3f, 4.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(50.0f);
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);

	m_floor.buildQuadXZ({-50.0f, 0.0f, -50.0f}, {100.0f, 100.0f});
	m_floor.rotate(0.0f, 45.0f, 0.0f);
	m_bullet.buildQuadXZ({ -0.3f * 0.243f, 0.0f, -0.3f * 0.243f }, { 0.3f * 0.5f, 0.3f * 0.5f }, 1u, 1u, true, false);

	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_storageBuffer.createBuffer(1000u * sizeof(Matrix4f), WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst);
	m_wigglyBuffer.createBuffer(sizeof(Vector4f), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_skinBuffer.createBuffer(sizeof(Matrix4f) * 96u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage);

	m_rotationBuffer.createBuffer(sizeof(Vector4f) * 4000u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_offsetBuffer.createBuffer(sizeof(Vector4f) * 4000u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	m_wgpBulletTexture.setFlipHorizontal(true);
	m_wgpBulletTexture.loadFromFile("res/textures/BulletTexture.png");

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	m_uniforms.lightPosition = Vector3f(50.0f, 100.0f, -100.0f);

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));
	//wgpuQueueWriteBuffer(wgpContext.queue, m_instanceBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));

	wgpContext.addSahderModule("ANIMATION", "res/shader/animation_fbx.wgsl");
	wgpContext.createRenderPipeline("ANIMATION", "RP_ANIMATION", VL_PTNWJ, std::bind(&Isometric::OnBindGroupLayouts, this));

	wgpContext.addSahderModule("FLOOR", "res/shader/floor.wgsl");
	wgpContext.createRenderPipeline("FLOOR", "RP_FLOOR", VL_PTN, std::bind(&Isometric::OnBindGroupLayoutsFloor, this));

	wgpContext.addSahderModule("WIGGLY", "res/shader/wiggly.wgsl");
	wgpContext.createRenderPipeline("WIGGLY", "RP_WIGGLY", VL_PTN, std::bind(&Isometric::OnBindGroupLayoutsWiggly, this));

	wgpContext.addSahderModule("BULLET", "res/shader/bullet.wgsl");
	wgpContext.createRenderPipeline("BULLET", "RP_BULLET", VL_PT, std::bind(&Isometric::OnBindGroupLayoutsBullet, this),
		1u, WGPUPrimitiveTopology_TriangleList, WGPUTextureFormat_Undefined, WGPUTextureFormat_Undefined, WGPUCompareFunction_Always,
		{ DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined, WGPUCullMode_None, StencilMode::DEFAULT, {} });

	m_wgpPlayer.create(m_player);
	m_wgpPlayer.setBindGroups("BG", std::bind(&Isometric::OnBindGroups, this));

	wgpContext.setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	wgpContext.OnDraw = std::bind(&Isometric::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
	nkContext.OnFillBuffer = std::bind(&Isometric::OnFillBuffer, this, std::placeholders::_1);
	
	m_wgpFloorD.loadFromFile("res/textures/floor/Floor_D.psd");
	m_wgpEnemyD.loadFromFile("res/models/EelDog/Eeldog_Albedo.tif");

	m_wgpFloor.create(m_floor);
	m_wgpFloor.setBindGroups("BG", std::bind(&Isometric::OnBindGroupsFloor, this));

	m_wgpEnemy.create(m_enemy);
	m_wgpEnemy.addBindGroup("BG", CreateBindGroup(m_uniformBuffer, m_wigglyBuffer, m_wgpEnemyD, m_storageBuffer));

	m_wgpBullet.create(m_bullet);
	m_wgpBullet.setBindGroups("BG", std::bind(&Isometric::OnBindGroupsBullet, this));

	m_player.addAnimationState(AnimationManager::Get().getAnimation("forward"));
	m_player.getAnimationState(0u)->setLooped(true);

	m_player.addAnimationState(AnimationManager::Get().getAnimation("left"));
	m_player.getAnimationState(1u)->setLooped(true);

	m_player.addAnimationState(AnimationManager::Get().getAnimation("backward"));
	m_player.getAnimationState(2u)->setLooped(true);

	m_player.addAnimationState(AnimationManager::Get().getAnimation("right"));
	m_player.getAnimationState(3u)->setLooped(true);

	m_player.addAnimationState(AnimationManager::Get().getAnimation("idle"));
	m_player.getAnimationState(4u)->setLooped(true);

	m_player.addAnimationState(AnimationManager::Get().getAnimation("death"));
	m_player.getAnimationState(5u)->setLooped(false);

	m_player.update(0.01f);

	m_fire.init<RtAudioEffect>();

	m_ding.init<RtAudioEffect>();
	m_ding.get<RtAudioEffect>()->getMixer().setVolume(0.5f);

	m_scene = new SceneNode();
	m_scene->setOnChildAdded([this](Node* newNode) {
		if (auto* enemy = dynamic_cast<Enemy*>(newNode)) {
			m_enemies.push_back(enemy);
		}
	});

	m_scene->setOnChildRemoved([this](Node* removedNode) {	
		auto it = std::find(m_enemies.begin(), m_enemies.end(), removedNode);
		if (it != m_enemies.end()) {
			std::iter_swap(it, m_enemies.end() - 1);
			m_enemies.pop_back();
		}
	});

	m_enemySpawner.scene = m_scene;
	m_targetPoolSize = 100;

	btCollisionObject* body = Physics::AddKinematicObject(Physics::BtTransform(Vector3f(0.0f, 0.4f, 0.0f)), new btCylinderShape(btVector3(0.35f * 0.5f, 0.4f, 0.35f * 0.5f)), Physics::collisiontypes::CHARACTER, Physics::collisiontypes::ENEMY);
	m_playerEnitity = m_scene->addChild<Player>(body, m_player);
}

Isometric::~Isometric() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	nkShutDown();
	Physics::DebugDrawer.shutDown();
	m_uniformBuffer.markForDelete();
	m_storageBuffer.markForDelete();
	m_wigglyBuffer.markForDelete();
	m_skinBuffer.markForDelete();
	m_rotationBuffer.markForDelete();
	m_offsetBuffer.markForDelete();
}

void Isometric::fixedUpdate() {
	size_t activeBulletCount = m_bulletStore.m_offsets.size();

	while (m_entities.size() < activeBulletCount) {
		createNewBulletToPool();
	}

	while (m_entities.size() > activeBulletCount) {
		CollisionEntity* lastEntity = m_entities.back();
		m_scene->eraseChildSilent(lastEntity);
		m_entities.pop_back();
	}

	for (auto entity : m_entities) {
		entity->setActive(false);
	}

	for (size_t i = 0; i < activeBulletCount; ++i) {
		glm::vec3 pos = m_bulletStore.m_offsets[i];
		glm::quat rot = m_bulletStore.m_rots[i];

		m_entities[i]->setActive(true);
		m_entities[i]->setPosition(pos[0], pos[1], pos[2]);
		m_entities[i]->setOrientation(rot.x, rot.y, rot.z, rot.w);
	}

	for (size_t i = activeBulletCount; i < m_entities.size(); ++i) {
		if (m_entities[i]->isActive()) {
			m_entities[i]->setActive(false);
		}
	}

	for (auto entity : m_entities) {
		entity->fixedUpdate(m_fdt);
	}

	for (auto enemy : m_enemies) {
		enemy->fixedUpdate(m_fdt);
	}

	m_playerEnitity->fixedUpdate(m_fdt);

	Globals::physics->stepSimulation(PHYSICS_STEP);

	BulletCollisionPlayerCallback callback;
	Physics::GetDynamicsWorld()->contactTest(m_playerEnitity->getCollisionObject(), callback);
	if (callback.m_hasCollided && callback.m_hitTarget) {
		m_playerEnitity->setActive(false);
		m_isDeath = true;
	}

	for (auto entity : m_entities) {
		if (!entity->isActive()) continue;
		
		BulletCollisionCallback callback;
		Physics::GetDynamicsWorld()->contactTest(entity->getCollisionObject(), callback);

		if (callback.m_hasCollided && callback.m_hitTarget) {			
			void* userPtr = callback.m_hitTarget->getUserPointer();
			if (userPtr) {
				Enemy* hitEnemy = static_cast<Enemy*>(userPtr);
				hitEnemy->setActive(false);
				m_scene->eraseChild(hitEnemy);

				m_ding.play("res/sounds/bullet_hit_metal_enemy_4.wav");
			}
		}
	}
}

void Isometric::update() {
	Keyboard& keyboard = Keyboard::instance();
	Mouse& mouse = Mouse::instance();

	nkUpdateInput(mouse.xPos(), mouse.yPos(), mouse.buttonDown(Mouse::MouseButton::BUTTON_LEFT), mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT), Application::ScrollDelta);

	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;
	bool playerMove = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		direction += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(1.0f, 0.0f, 0.0f);
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

	if (keyboard.keyPressed(Keyboard::KEY_1)) {
		m_fire.get<RtAudioEffect>()->getMixer().setEnabled("chorus", true);
	}

	if (keyboard.keyPressed(Keyboard::KEY_2)) {
		m_fire.get<RtAudioEffect>()->getMixer().setEnabled("chorus", false);
	}

	if (keyboard.keyPressed(Keyboard::KEY_Z)) {
		Physics::DebugDrawer.toggleWireframe();
	}

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_debugCollision = !m_debugCollision;
	}

	if (!m_isDeath && (m_rotationButtonResult.buttonDown || (mouse.buttonDown(Mouse::MouseButton::BUTTON_LEFT) && !m_rotationButtonResult.isActive && !m_joystickResult.isActive)) && (lastFireTime + 0.1f) < Globals::clock.getElapsedTimeSec()) {
		const Quaternion orientation = m_player.getOrientation();

		glm::quat midOri;
		midOri.x = orientation[0];
		midOri.y = orientation[1];
		midOri.z = orientation[2];
		midOri.w = orientation[3];

		const Matrix4f trans = m_player.getWorldTransformation();
		const glm::mat4 playerModelTransform(trans[0][0], trans[0][1], trans[0][2], trans[0][3],
                                             trans[1][0], trans[1][1], trans[1][2], trans[1][3],
                                             trans[2][0], trans[2][1], trans[2][2], trans[2][3],
                                             trans[3][0], trans[3][1], trans[3][2], trans[3][3]);

		const glm::vec3 projectileSpawnPoint = playerModelTransform * glm::vec4(-20.0f, 120.0f, 100.0f, 1.0f);

		m_bulletStore.createBullets(projectileSpawnPoint, midOri, spreadAmount);
		lastFireTime = Globals::clock.getElapsedTimeSec();
		m_fire.play("res/sounds/shooting_one.wav");
	}

	if (mouse.buttonDownInvisible(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xDelta();
		dy = mouse.yDelta();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);
		}
		
		if (move) {
			m_camera.move(direction * m_dt);
		}
	}
	m_trackball.idle();

	const AnimatedMesh* mesh_ = static_cast<const AnimatedMesh*>(m_player.getMesh());
	const Vector3f posistion = mesh_->getBone(0u).getPosition();
	m_camera.lookAt(posistion + Vector3f(0.0f, 4.3f, 4.0f), posistion, Vector3f(0.0f, 1.0f, 0.0f));
	if ((mouse.xDelta() || mouse.yDelta()) && !m_isDeath) {
		Vector3f coords;	
		if (getWorldPosition(mouse.xPos(), mouse.yPos(), Vector3f(0.0f, 1.0f, 0.0f), coords)) {
			aimTheta = (!m_rotationButtonResult.isActive && !m_joystickResult.isActive) ? getLookAtYRotation(posistion, coords) : m_rotationButtonResult.degrees;
			m_rotationButtonResult.degrees = aimTheta ;

			if(m_rotationButtonResult.degrees)
				m_player.setOrientation(0.0f, m_rotationButtonResult.degrees, 0.0f);
		}
	}

	float moveX = 0.0f;
	float moveY = 0.0f;

	float magnitude = m_joystickResult.x * m_joystickResult.x + m_joystickResult.y * m_joystickResult.y;
	float deadzone = 0.25f;

	if (magnitude > deadzone * deadzone) {
		if (fabsf(m_joystickResult.x) > fabsf(m_joystickResult.y)) {
			moveX = (m_joystickResult.x > 0.0f) ? 1.0f : -1.0f;
			moveY = 0.0f;
		}else {
			moveX = 0.0f;
			moveY = (m_joystickResult.y > 0.0f) ? 1.0f : -1.0f;
		}
	}else {
		moveX = 0.0f;
		moveY = 0.0f;
	}

	Vector3f playerDirection = Vector3f();
	if (keyboard.keyDown(Keyboard::KEY_UP) || moveY > 0.0f) {
		playerDirection -= Vector3f(0.0f, 0.0f, 1.0f);
	}

	if (keyboard.keyDown(Keyboard::KEY_DOWN) || moveY < 0.0f) {
		playerDirection += Vector3f(0.0f, 0.0f, 1.0f);
	}

	if (keyboard.keyDown(Keyboard::KEY_LEFT) || moveX < 0.0f) {
		playerDirection -= Vector3f(1.0f, 0.0f, 0.0f);
	}

	if (keyboard.keyDown(Keyboard::KEY_RIGHT) || moveX > 0.0f) {
		playerDirection += Vector3f(1.0f, 0.0f, 0.0f);
	}

	playerMove = playerDirection.lengthSq() > 0.01f && !m_isDeath;

	if (playerMove) {
		m_playerEnitity->translate(playerDirection[0] * 2.0f * m_dt, playerDirection[1] * 2.0f * m_dt, playerDirection[2] * 2.0f * m_dt);
	}

	float movementTheta = std::atan2(playerDirection[0], playerDirection[2]);
	const float thetaDelta = movementTheta - aimTheta * PI_ON_180;
	const Vector2f movementAnim = !playerMove ? Vector2f() : Vector2f(sinf(thetaDelta), cosf(thetaDelta));

	prev_idleWeight = std::max(0.0f, prev_idleWeight - m_dt / animTransitionTime);
	prev_rightWeight = std::max(0.0f, prev_rightWeight - m_dt / animTransitionTime);
	prev_leftWeight = std::max(0.0f, prev_leftWeight - m_dt / animTransitionTime);
	prev_forwardWeight = std::max(0.0f, prev_forwardWeight - m_dt / animTransitionTime);
	prev_backWeight = std::max(0.0f, prev_backWeight - m_dt / animTransitionTime);

	float deathWeight = m_isDeath ? 1.0f : 0.0f;
	float idleWeight = prev_idleWeight + ((m_isDeath || playerMove) ? 0.0f : 1.0f);
	float forwardWeight = prev_forwardWeight + (playerMove ? std::max(0.0f, movementAnim[1]) : 0.0f);
	float leftWeight = prev_leftWeight + (playerMove ? std::max(0.0f, movementAnim[0]) : 0.0f);
	float backWeight = prev_backWeight + (playerMove ? std::max(0.0f, -movementAnim[1]) : 0.0f);
	float rightWeight = prev_rightWeight + (playerMove ? std::max(0.0f, -movementAnim[0]) : 0.0f);
	const float weightSum = deathWeight + idleWeight + rightWeight + forwardWeight + backWeight + leftWeight;

	deathWeight /= weightSum;
	idleWeight /= weightSum;
	rightWeight /= weightSum;
	forwardWeight /= weightSum;
	backWeight /= weightSum;
	leftWeight /= weightSum;
	prev_idleWeight = std::max(prev_idleWeight, idleWeight);
	prev_rightWeight = std::max(prev_rightWeight, rightWeight);
	prev_leftWeight = std::max(prev_leftWeight, leftWeight);
	prev_forwardWeight = std::max(prev_forwardWeight, forwardWeight);
	prev_backWeight = std::max(prev_backWeight, backWeight);
	 
	//leftWeight *= 0.75f;
	//backWeight *= 0.75f;
	//rightWeight *= 0.75f;
	idleWeight *= 0.25f;

	m_player.getAnimationState(0u)->setWeight(forwardWeight);
	m_player.getAnimationState(1u)->setWeight(leftWeight);
	m_player.getAnimationState(2u)->setWeight(backWeight);
	m_player.getAnimationState(3u)->setWeight(rightWeight);
	m_player.getAnimationState(4u)->setWeight(idleWeight);
	m_player.getAnimationState(5u)->setWeight(deathWeight);

	m_player.update(m_dt);
	m_player.updateSkinning();
	m_bulletStore.updateBullets(m_dt);
	m_enemySpawner.update(posistion, m_dt);

	for (auto enemy : m_enemies) {
		enemy->update(m_dt);
	}

	const AnimatedMesh* mesh = static_cast<const AnimatedMesh*>(m_player.getMesh());
	mesh->skinMatrices()[42] ^= mesh->getBone(43u).getWorldTransformation() * offset * pivot;
	wgpuQueueWriteBuffer(wgpContext.queue, m_skinBuffer.getBuffer(), 0u, mesh->getSkinMatrices(), mesh->getNumBones() * sizeof(Matrix4f));

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));

	m_cpuInstanceBuffer.clear();
	m_cpuInstanceBuffer.reserve(m_enemies.size());

	for (const auto& child : m_enemies) {	
		m_cpuInstanceBuffer.push_back(child->getTransformationSOP());
	}

	wgpuQueueWriteBuffer(wgpContext.queue, m_storageBuffer.getBuffer(), 0u, m_cpuInstanceBuffer.data(), m_cpuInstanceBuffer.size() * sizeof(Matrix4f));

	m_wiggly.nosePos[0] = 1.0f ;
	m_wiggly.nosePos[1] = 120.0f * 0.0044f ;
	m_wiggly.nosePos[2] = -2.0f ;
	m_wiggly.time = Globals::clock.getElapsedTimeSec();

	wgpuQueueWriteBuffer(wgpContext.queue, m_wigglyBuffer.getBuffer(), 0, &m_wiggly, sizeof(Wiggly));

	wgpuQueueWriteBuffer(wgpContext.queue, m_rotationBuffer.getBuffer(), 0u, m_bulletStore.m_rots.data(), m_bulletStore.m_rots.size() * sizeof(Vector4f));
	wgpuQueueWriteBuffer(wgpContext.queue, m_offsetBuffer.getBuffer(), 0u, m_bulletStore.m_offsets.data(), m_bulletStore.m_offsets.size() * sizeof(Vector4f));

	if (m_debugCollision) {
		Matrix4f viewProjection = m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix();
		viewProjection.copy(Physics::DebugDrawer.getViewProjection());
	}
}

void Isometric::render() {
	wgpDraw();
}

void Isometric::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {

	{
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);

		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_FLOOR"));
		m_wgpFloor.draw(renderPassEncoder);

		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WIGGLY"));
		m_wgpEnemy.draw(renderPassEncoder, m_cpuInstanceBuffer.size());

		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_ANIMATION"));
		m_wgpPlayer.draw(renderPassEncoder);

		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_BULLET"));
		m_wgpBullet.draw(renderPassEncoder, m_bulletStore.m_rots.size());

		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}

	if (m_debugCollision)
	{
		Physics::GetDynamicsWorld()->debugDrawWorld();
		Physics::DebugDrawer.OnDraw(commandEncoder, renderPassDescriptor);
	}

	{
		WGPURenderPassColorAttachment renderPassColorAttachment = renderPassDescriptor.colorAttachments[0];
		renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Load;

		WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
		rndrPssDscrptor.colorAttachments = &renderPassColorAttachment;

		nkDraw(commandEncoder, rndrPssDscrptor);
	}

	if (m_drawUi)
	{
		WGPURenderPassColorAttachment renderPassColorAttachment = renderPassDescriptor.colorAttachments[0];
		renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Load;

		WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
		rndrPssDscrptor.colorAttachments = &renderPassColorAttachment;

		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &rndrPssDscrptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
		renderUi(renderPassEncoder);
		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}
}

void Isometric::OnFillBuffer(nk_context& nkCntxt) {
	set_transparent_window_style();
	virtual_joystick(nk_rect(20.0f, static_cast<float>(Application::Height) - 200.0f, 180.0f, 180.0f), m_joystickResult);
	virtual_rotation_button(nk_rect(static_cast<float>(Application::Width) - 180.0f, static_cast<float>(Application::Height) - 180.0f, 140.0f, 140.0f), m_rotationButtonResult);
	reset_transparent_window_style();
}

void Isometric::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void Isometric::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);
}

void Isometric::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void Isometric::OnMouseWheel(const Event::MouseWheelEvent& event) {
	
}

void Isometric::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void Isometric::OnKeyUp(const Event::KeyboardEvent& event) {

}

void Isometric::resize(int deltaW, int deltaH) {
	nkResize(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);
	m_trackball.reshape(Application::Width, Application::Height);	
}

void Isometric::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
	ImGui_ImplWGPU_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceId = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();

	if (m_initUi) {
		m_initUi = false;
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Debug Collision", &m_debugCollision);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayouts() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(2);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[1].buffer.minBindingSize = 16 * sizeof(float);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayoutsFloor() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayoutsWiggly() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(5);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = sizeof(Vector4f);

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[3].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[3].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[4].binding = 4u;
	bindingLayoutEntries[4].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[4].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[4].buffer.minBindingSize = 1000u * sizeof(Matrix4f);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayoutsBullet() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(5);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = sizeof(Vector4f) * 4000u;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[2].buffer.minBindingSize = sizeof(Vector4f) * 4000u;

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[3].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[4].binding = 4u;
	bindingLayoutEntries[4].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[4].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[4].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> Isometric::OnBindGroups() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(2);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_skinBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(m_skinBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_ANIMATION"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

std::vector<WGPUBindGroup> Isometric::OnBindGroupsFloor() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_wgpFloorD.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_FLOOR"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

std::vector<WGPUBindGroup> Isometric::OnBindGroupsBullet() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(5);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_rotationBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(m_rotationBuffer.getBuffer());

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].buffer = m_offsetBuffer.getBuffer();
	bindGroupEntries[2].offset = 0u;
	bindGroupEntries[2].size = wgpuBufferGetSize(m_rotationBuffer.getBuffer());

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[4].binding = 4u;
	bindGroupEntries[4].textureView = m_wgpBulletTexture.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_BULLET"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

WGPUBindGroup Isometric::CreateBindGroup(const WgpBuffer& uniformBuffer, const WgpBuffer& wigglyBuffer, const WgpTexture& texture, const WgpBuffer& storageBuffer) {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(5);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(uniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = wigglyBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = sizeof(Vector4f);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].textureView = texture.getTextureView();

	bindGroupEntries[4].binding = 4u;
	bindGroupEntries[4].buffer = storageBuffer.getBuffer();
	bindGroupEntries[4].offset = 0u;
	bindGroupEntries[4].size = wgpuBufferGetSize(storageBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WIGGLY"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

bool Isometric::getWorldPosition(int xPos, int yPos, const Vector3f& planeNormal, Vector3f& outIntersection) {
	float mouseXndc = (2.0f * xPos) / static_cast<float>(Application::Width) - 1.0f;
	float mouseYndc = 1.0f - (2.0f * yPos) / static_cast<float>(Application::Height);

	float tanfov = m_camera.getTanFov();
	float aspect = (static_cast<float>(Application::Width) / static_cast<float>(Application::Height));

	Vector3f rayStartWorld = m_camera.getPosition() + (m_camera.getCamX() * mouseXndc * tanfov * aspect + m_camera.getCamY() * mouseYndc * tanfov + m_camera.getViewDirection()) * m_camera.getNear();
	Vector3f rayEndWorld = m_camera.getPosition() + (m_camera.getCamX() * mouseXndc * tanfov * aspect + m_camera.getCamY() * mouseYndc * tanfov + m_camera.getViewDirection()) * m_camera.getFar();
	Vector3f direction = Vector3f::Normalize(rayEndWorld - rayStartWorld);
	float denom = Vector3f::Dot(direction, planeNormal);

	if (std::abs(denom) > 1e-6f) {
		float t = Vector3f::Dot(-rayStartWorld, planeNormal) / denom;
		if (t >= 0.0f) { 
			outIntersection = rayStartWorld + direction * t;
			return true;
		}
	}
	return false;
}

float Isometric::getLookAtYRotation(const Vector3f& objectPos, const Vector3f& targetPos) {
	float dx = targetPos[0] - objectPos[0];
	float dz = targetPos[2] - objectPos[2];

	if (abs(dx) < 0.01f && abs(dz) < 0.01f)
		return 0.0f;

	return std::atan2(dx, dz) * _180_ON_PI;
}

CollisionEntity* Isometric::createNewBulletToPool() {
	btTransform startTransform;
	startTransform.setIdentity();

	btCollisionObject* body = Physics::AddKinematicObject(startTransform, new btCapsuleShapeX(0.03f, 0.3f), Physics::collisiontypes::SPHERE, Physics::collisiontypes::ENEMY);

	CollisionEntity* entity = m_scene->addChildSilent<CollisionEntity>(body);
	entity->setActive(false);
	m_entities.push_back(entity);

	return entity;
}