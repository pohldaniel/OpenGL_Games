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
const int spreadAmount = 50;

Isometric::Isometric(StateMachine& machine) : State(machine, States::ISOMETRIC), m_bulletStore(&threadPool), m_enemySpawner(120.0f * 0.0044f, m_player) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	wgpSetMSAASampleCount(1u, Application::OnSurfaceChange);
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
	m_infoBufferBillboard.createBuffer(sizeof(FrameInfo), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_infoBufferMuzzle.createBuffer(sizeof(FrameInfo), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	
	m_storageBuffer.createBuffer(1000u * sizeof(Matrix4f), WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst);
	m_wigglyBuffer.createBuffer(sizeof(Vector4f), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_skinBuffer.createBuffer(sizeof(Matrix4f) * 96u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage);

	m_rotationBuffer.createBuffer(sizeof(Vector4f) * 400000u, WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst);
	m_offsetBuffer.createBuffer(sizeof(Vector4f) * 400000u, WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst);

	m_spriteBuffer.createBuffer(20u * sizeof(SpriteInstance), WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst);
	m_muzzleBuffer.createBuffer(100u * sizeof(SpriteInstance), WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst);

	m_directionalLightBuffer.createBuffer(sizeof(DirectionalLight), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_pointLightBuffer.createBuffer(sizeof(PointLight), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	m_wgpBulletTexture.setFlipHorizontal(true);
	m_wgpBulletTexture.loadFromFile("res/textures/BulletTexture.png");
	m_sprite.loadFromFile("res/textures/impact_spritesheet_with_00.png");
	m_muzzle.loadFromFile("res/textures/muzzle_spritesheet.png");

	m_wgpPlayerD.loadFromFile("res/models/Player_D.tga", true);
	m_wgpPlayerN.loadFromFile("res/models/Player_N.tga", true);
	m_wgpPlayerS.loadFromFile("res/models/Player_S.tga", true);
	m_wgpPlayerE.loadFromFile("res/models/Player_E.tga", true);

	m_wgpGunD.loadFromFile("res/models/Gun_D.tga", true);
	m_wgpGunN.loadFromFile("res/models/Gun_N.tga", true);
	m_wgpGunS.loadFromFile("res/models/Gun_S.tga", true);
	m_wgpGunE.loadFromFile("res/models/Gun_E.tga", true);

	m_wgpFloorD.loadFromFile("res/textures/floor/Floor_D.psd");
	m_wgpFloorN.loadFromFile("res/textures/floor/Floor_N.psd");
	m_wgpFloorS.loadFromFile("res/textures/floor/Floor_S.psd");

	m_wgpEnemyD.loadFromFile("res/models/EelDog/Eeldog_Albedo.tif");
	m_wgpEnemyN.loadFromFile("res/models/EelDog/Eeldog_Normal.tif");
	m_wgpEnemyS.loadFromFile("res/models/EelDog/Eeldog_MetallicSmooth.tif");

	m_wgpTextureShadow.createEmpty(6u * 1024u, 6u * 1024u, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_Depth32Float);
	
	m_wgpEmissionTarget.createEmpty(Application::Width, Application::Height, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_BGRA8Unorm, 1u, 1u);
	m_wgpEmissionDepth.createEmpty(Application::Width, Application::Height, 1u, WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_Depth16Unorm, 1u, 1u);
	
	m_wgpSceneTarget.createEmpty(Application::Width, Application::Height, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_BGRA8Unorm, 1u, 4u);
	m_wgpSceneDepth.createEmpty(Application::Width, Application::Height, 1u, WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_Depth24Plus, 1u, 4u);

	m_wgpBlurTempTarget.createEmpty(Application::Width / 2, Application::Height / 2, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_BGRA8Unorm, 1u, 1u);
	m_wgpBlurFinalTarget.createEmpty(Application::Width / 2, Application::Height / 2, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_BGRA8Unorm, 1u, 1u);

	m_lightDir = Vector3f(-1.0f, -1.0f, 1.0f);
	m_lightProjection = Matrix4f::Orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.0f);	
	m_lightView = Matrix4f::LookAt(Vector3f(0.0f, 0.0f, 0.0f) - 20.0f * m_lightDir, Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = m_lightProjection * m_lightView;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	m_uniforms.lightPosition = Vector3f(0.0f, 0.0f, 0.0f) - 20.0f * m_lightDir;

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));
	//wgpuQueueWriteBuffer(wgpContext.queue, m_instanceBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));

	FrameInfo sprite;
	sprite.frameSize[0] = 1.0f / 11.0f;
	sprite.frameSize[1] = 1.0f;
	sprite.colRow[0] = 11u;
	sprite.colRow[1] = 1u;
	wgpuQueueWriteBuffer(wgpContext.queue, m_infoBufferBillboard.getBuffer(), 0u, &sprite, sizeof(FrameInfo));

	FrameInfo muzzle;
	muzzle.frameSize[0] = 1.0f / 6.0f;
	muzzle.frameSize[1] = 1.0f;
	muzzle.colRow[0] = 6u;
	muzzle.colRow[1] = 1u;
	wgpuQueueWriteBuffer(wgpContext.queue, m_infoBufferMuzzle.getBuffer(), 0u, &muzzle, sizeof(FrameInfo));

	DirectionalLight directional;
	directional.color[0] = 0.35f * 0.7f * 0.406f; directional.color[1] = 0.35f * 0.7f * 0.723f; directional.color[2] = 0.35f * 0.7f; directional.color[3] = 1.0f;
	directional.padding = 0.0f;

	Vector3f _dir = Vector3f(-0.8f, 0.0f, -1.0f);
	_dir.normalize();

	directional.direction[0] = _dir[0];
	directional.direction[1] = _dir[1];
	directional.direction[2] = _dir[2];
	wgpuQueueWriteBuffer(wgpContext.queue, m_directionalLightBuffer.getBuffer(), 0u, &directional, sizeof(DirectionalLight));

	wgpContext.addSampler(wgpCreateSampler(WGPUFilterMode_Linear, WGPUAddressMode_ClampToEdge, 1u, WGPUMipmapFilterMode_Nearest, WGPUCompareFunction_Greater), SS_0);

	wgpContext.addSahderModule("COMPOSITE", "res/shader/composite.wgsl");
	wgpContext.createRenderPipeline("COMPOSITE", "RP_COMPOSITE", VL_NONE, std::bind(&Isometric::OnBindGroupLayoutsComposite, this), 1u);

	wgpContext.addSahderModule("ANIMATION", "res/shader/player.wgsl");
	wgpContext.createRenderPipeline("ANIMATION", "RP_ANIMATION", VL_PTNWJ, std::bind(&Isometric::OnBindGroupLayouts, this), 4u);

	wgpContext.addSahderModule("EMISSION", "res/shader/player_emission.wgsl");
	wgpContext.createRenderPipeline("EMISSION", "RP_PLAYER_EMISSION", VL_PTNWJ, std::bind(&Isometric::OnBindGroupLayoutsEmission, this),
		1u, WGPUPrimitiveTopology_TriangleList, WGPUTextureFormat_Undefined, WGPUTextureFormat_Depth16Unorm);

	wgpContext.addSahderModule("FLOOR", "res/shader/floor.wgsl");
	wgpContext.createRenderPipeline("FLOOR", "RP_FLOOR", VL_PTN, std::bind(&Isometric::OnBindGroupLayoutsFloor, this), 4u);

	wgpContext.addSahderModule("FLOOR_MASK", "res/shader/floor_emission.wgsl");
	wgpContext.createRenderPipeline("FLOOR_MASK", "RP_FLOOR_EMISSION", VL_PTN, std::bind(&Isometric::OnBindGroupLayoutsFloorEmission, this),
		1u, WGPUPrimitiveTopology_TriangleList, WGPUTextureFormat_Undefined, WGPUTextureFormat_Depth16Unorm, WGPUCompareFunction_Always,
		{ DEPTH_STENCIL_STATE | FRAGMENT_STATE, ColorMode::WRITE_NONE });

	wgpContext.addSahderModule("WIGGLY", "res/shader/wiggly.wgsl");
	wgpContext.createRenderPipeline("WIGGLY", "RP_WIGGLY", VL_PTN, std::bind(&Isometric::OnBindGroupLayoutsWiggly, this), 4u);

	wgpContext.addSahderModule("BULLET", "res/shader/bullet.wgsl");
	wgpContext.createRenderPipeline("BULLET", "RP_BULLET", VL_PT, std::bind(&Isometric::OnBindGroupLayoutsBullet, this),
		4u, WGPUPrimitiveTopology_TriangleList, WGPUTextureFormat_Undefined, WGPUTextureFormat_Undefined, WGPUCompareFunction_Always,
		{ DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE, ColorMode::WRITE_RGBA, DepthMode::WRITE, StencilMode::DEFAULT, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined, WGPUCullMode_None });

	wgpContext.createRenderPipeline("BULLET", "RP_BULLET_EMISSION", VL_PT, std::bind(&Isometric::OnBindGroupLayoutsBullet, this),
		1u, WGPUPrimitiveTopology_TriangleList, WGPUTextureFormat_Undefined, WGPUTextureFormat_Depth16Unorm, WGPUCompareFunction_LessEqual,
		{ DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE, ColorMode::WRITE_RGBA, DepthMode::WRITE, StencilMode::DEFAULT, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined, WGPUCullMode_None });

	wgpContext.addSahderModule("BILLBOARD", "res/shader/billboard.wgsl");
	wgpContext.createRenderPipeline("BILLBOARD", "RP_BILLBOARD", VL_NONE, std::bind(&Isometric::OnBindGroupLayoutsBillboard, this),
		4u, WGPUPrimitiveTopology_TriangleStrip, WGPUTextureFormat_Undefined, WGPUTextureFormat_Undefined, WGPUCompareFunction_Always,
		{ DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE });

	wgpContext.addSahderModule("MUZZLE", "res/shader/muzzle.wgsl");
	wgpContext.createRenderPipeline("MUZZLE", "RP_MUZZLE", VL_NONE, std::bind(&Isometric::OnBindGroupLayoutsBillboard, this),
		4u, WGPUPrimitiveTopology_TriangleStrip, WGPUTextureFormat_Undefined, WGPUTextureFormat_Undefined, WGPUCompareFunction_Always,
		{ DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE, ColorMode::WRITE_RGBA, DepthMode::WRITE, StencilMode::DEFAULT, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined, WGPUCullMode_None });

	wgpContext.addSahderModule("PLAYER_SHADOW", "res/shader/player_shadow.wgsl");
	wgpContext.createRenderPipeline("PLAYER_SHADOW", "RP_PLAYER_SHADOW", VL_PTNWJ, std::bind(&Isometric::OnBindGroupLayoutsShadow, this),
		1u, WGPUPrimitiveTopology_TriangleList, WGPUTextureFormat_Undefined, WGPUTextureFormat_Depth32Float, WGPUCompareFunction_Less,
		{ DEPTH_STENCIL_STATE }
	);

	wgpContext.addSahderModule("WIGGLY_SHADOW", "res/shader/wiggly_shadow.wgsl");
	wgpContext.createRenderPipeline("WIGGLY_SHADOW", "RP_WIGGLY_SHADOW", VL_PTN, std::bind(&Isometric::OnBindGroupLayoutsWigglyShadow, this),
		1u, WGPUPrimitiveTopology_TriangleList, WGPUTextureFormat_Undefined, WGPUTextureFormat_Depth32Float, WGPUCompareFunction_Less,
		{ DEPTH_STENCIL_STATE}
	);

	wgpContext.addSahderModule("BLUR_HORIZONTAL", "res/shader/blur_horizontal.wgsl");
	wgpContext.createRenderPipeline("BLUR_HORIZONTAL", "RP_BLUR_HORIZONTAL", VL_NONE, std::bind(&Isometric::OnBindGroupLayoutsBlur, this),
		1u, WGPUPrimitiveTopology_TriangleList, WGPUTextureFormat_Undefined, WGPUTextureFormat_Undefined, WGPUCompareFunction_Always,
		{ FRAGMENT_STATE}
	);

	wgpContext.addSahderModule("BLUR_VERTICAL", "res/shader/blur_vertical.wgsl");
	wgpContext.createRenderPipeline("BLUR_VERTICAL", "RP_BLUR_VERTICAL", VL_NONE, std::bind(&Isometric::OnBindGroupLayoutsBlur, this),
		1u, WGPUPrimitiveTopology_TriangleList, WGPUTextureFormat_Undefined, WGPUTextureFormat_Undefined, WGPUCompareFunction_Always,
		{ FRAGMENT_STATE }
	);

	m_wgpPlayer.create(m_player);
	m_wgpPlayer.setBindGroups("SHADOW", std::bind(&Isometric::OnBindGroupsShadow, this));
	m_wgpPlayer.getMesh(0u).setBindGroups("EMISSION", std::bind(&Isometric::OnBindGroupsPlayerEmission, this));
	m_wgpPlayer.getMesh(0u).setBindGroups("BG", std::bind(&Isometric::OnBindGroupsPlayer, this));
	m_wgpPlayer.getMesh(1u).setBindGroups("EMISSION", std::bind(&Isometric::OnBindGroupsGunEmission, this));
	m_wgpPlayer.getMesh(1u).setBindGroups("BG", std::bind(&Isometric::OnBindGroupsGun, this));

	wgpContext.setClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	wgpContext.OnDraw = std::bind(&Isometric::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
	wgpContext.OnPostDraw = std::bind(&Isometric::OnPostDraw, this);
	nkContext.OnFillBuffer = std::bind(&Isometric::OnFillBuffer, this, std::placeholders::_1);
	
	m_wgpFloor.create(m_floor);
	m_wgpFloor.addBindGroups("EMISSION", std::bind(&Isometric::OnBindGroupsFloorEmission, this));
	m_wgpFloor.setBindGroups("BG", std::bind(&Isometric::OnBindGroupsFloor, this));

	m_wgpEnemy.create(m_enemy);
	m_wgpEnemy.addBindGroup("SHADOW", CreateBindGroupShadow(m_uniformBuffer, m_wigglyBuffer, m_storageBuffer));
	m_wgpEnemy.addBindGroup("BG", createBindGroupWiggly());

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
			m_enemySpawner.count()++;
		}
	});

	m_scene->setOnChildRemoved([this](Node* removedNode) {	
		auto it = std::find(m_enemies.begin(), m_enemies.end(), removedNode);
		if (it != m_enemies.end()) {
			std::iter_swap(it, m_enemies.end() - 1);
			m_enemies.pop_back();
			m_enemySpawner.count()--;
		}
	});

	m_enemySpawner.scene = m_scene;
	m_targetPoolSize = 100;

	btCollisionObject* body = Physics::AddKinematicObject(Physics::BtTransform(Vector3f(0.0f, 0.4f, 0.0f)), new btCylinderShape(btVector3(0.35f * 0.5f, 0.4f, 0.35f * 0.5f)), Physics::collisiontypes::CHARACTER, Physics::collisiontypes::ENEMY);
	m_playerEntity = m_scene->addChild<Player>(body, m_player);

	m_bindGroupBillboard = createBindGroupBillboard();
	m_bindGroupMuzzle = createBindGroupMuzzle();
	m_bindGroupComposite = createBindGroupComposite();
	m_bindGroupBlurH = createBindGroupBlurH();
	m_bindGroupBlurV = createBindGroupBlurV();
	m_muzzleInstance.currentFrame = 6u;
}

Isometric::~Isometric() {
	delete m_scene;
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	nkShutDown();
	Physics::DebugDrawer.shutDown();
	m_uniformBuffer.markForDelete();
	m_infoBufferBillboard.markForDelete();
	m_infoBufferMuzzle.markForDelete();
	m_storageBuffer.markForDelete();
	m_wigglyBuffer.markForDelete();
	m_skinBuffer.markForDelete();
	m_rotationBuffer.markForDelete();
	m_offsetBuffer.markForDelete();
	m_spriteBuffer.markForDelete();
	m_muzzleBuffer.markForDelete();

	m_pointLightBuffer.markForDelete();
	m_directionalLightBuffer.markForDelete();

	m_wgpFloorD.markForDelete();
	m_wgpFloorN.markForDelete();
	m_wgpFloorS.markForDelete();
	m_wgpEnemyD.markForDelete();
	m_wgpBulletTexture.markForDelete();
	m_sprite.markForDelete();
	m_muzzle.markForDelete();
	m_wgpTextureShadow.markForDelete();

	wgpuBindGroupRelease(m_bindGroupBillboard);
	wgpuBindGroupRelease(m_bindGroupMuzzle);
}

void Isometric::fixedUpdate() {
	for (auto enemy : m_enemies) {
		enemy->fixedUpdate(m_fdt);
	}

	m_playerEntity->fixedUpdate(m_fdt);

	Globals::physics->stepSimulation(PHYSICS_STEP);

	BulletCollisionCallback callback;
	Physics::GetDynamicsWorld()->contactTest(m_playerEntity->getCollisionObject(), callback);
	if (callback.m_hasCollided && callback.m_hitTarget) {
		m_playerEntity->setActive(false);
		m_isDeath = true;
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

	if (keyboard.keyPressed(Keyboard::KEY_R)) {
		//resetMuzzle();
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

		const glm::vec3 projectileSpawnPoint = playerModelTransform * glm::vec4(-20.0f, 120.0f, 140.0f, 1.0f);

		m_bulletStore.createBullets(projectileSpawnPoint, midOri, spreadAmount);
		lastFireTime = Globals::clock.getElapsedTimeSec();
		m_fire.play("res/sounds/shooting_one.wav");
		resetMuzzle();
		m_muzzleFlashSpritesAge.emplace_back(0.0f);
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
	if (keyboard.keyDown(Keyboard::KEY_W) || moveY > 0.0f) {
		playerDirection -= Vector3f(0.0f, 0.0f, 1.0f);
	}

	if (keyboard.keyDown(Keyboard::KEY_S) || moveY < 0.0f) {
		playerDirection += Vector3f(0.0f, 0.0f, 1.0f);
	}

	if (keyboard.keyDown(Keyboard::KEY_A) || moveX < 0.0f) {
		playerDirection -= Vector3f(1.0f, 0.0f, 0.0f);
	}

	if (keyboard.keyDown(Keyboard::KEY_D) || moveX > 0.0f) {
		playerDirection += Vector3f(1.0f, 0.0f, 0.0f);
	}

	playerMove = playerDirection.lengthSq() > 0.01f && !m_isDeath;

	if (playerMove) {
		m_playerEntity->translate(playerDirection[0] * 2.0f * m_dt, playerDirection[1] * 2.0f * m_dt, playerDirection[2] * 2.0f * m_dt);
		m_lightView = Matrix4f::LookAt(m_playerEntity->getPosition() - 20.0f * m_lightDir, m_playerEntity->getPosition(), Vector3f(0.0f, 1.0f, 0.0f));
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


	m_bulletStore.updateBullets(m_dt, m_enemies);
	m_enemySpawner.update(posistion, m_dt);

	for (auto enemy : m_enemies) {
		enemy->update(m_dt);
	}
	updateBillboards(m_dt);
	
	const AnimatedMesh* mesh = static_cast<const AnimatedMesh*>(m_player.getMesh());
	mesh->skinMatrices()[42] ^= mesh->getBone(43u).getWorldTransformation() * offset * pivot;
	wgpuQueueWriteBuffer(wgpContext.queue, m_skinBuffer.getBuffer(), 0u, mesh->getSkinMatrices(), mesh->getNumBones() * sizeof(Matrix4f));

	Matrix4f muzzleTransform = mesh->skinMatrices()[42] * Matrix4f::Translate(221.0f, 76.143f, -3.054f) ;
	updateMuzzle(m_dt, muzzleTransform[3][0], muzzleTransform[3][1], muzzleTransform[3][2]);

	float angle = aimTheta;
	while (angle < 0.0f) angle += 360.0f;
	while (angle >= 360.0f) angle -= 360.0f;
	float radians = angle * PI_ON_180;
	float finalCorrectionAngle = 90.0f * std::abs(std::cos(radians));
	float sign = (angle > 0.0f && angle < 180.0f) ? -1.0f : 1.0f;

	muzzleTransform = muzzleTransform * Matrix4f::Scale(100.0f, 100.0f, 100.0f) * Matrix4f::Rotate(sign * finalCorrectionAngle, 0.0f, 0.0f);
	
	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = muzzleTransform;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = m_lightProjection * m_lightView;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	m_uniforms.lightPosition = m_playerEntity->getPosition() - 20.0f * m_lightDir;

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));

	m_cpuInstanceBuffer.clear();
	m_cpuInstanceBuffer.reserve(m_enemies.size());

	for (const auto& enemy : m_enemies) {
		if (enemy->isDeath()) {
			enemy->setActive(false);
			m_scene->eraseChild(enemy);
			m_ding.play("res/sounds/bullet_hit_metal_enemy_4.wav");
			spawnBillboard(enemy->getPosition());
			continue;
		}
		m_cpuInstanceBuffer.push_back(enemy->getTransformationSOP());
	}

	wgpuQueueWriteBuffer(wgpContext.queue, m_storageBuffer.getBuffer(), 0u, m_cpuInstanceBuffer.data(), m_cpuInstanceBuffer.size() * sizeof(Matrix4f));

	m_wiggly.nosePos[0] = 1.0f ;
	m_wiggly.nosePos[1] = 120.0f * 0.0044f ;
	m_wiggly.nosePos[2] = -2.0f ;
	m_wiggly.time = Globals::clock.getElapsedTimeSec();

	wgpuQueueWriteBuffer(wgpContext.queue, m_wigglyBuffer.getBuffer(), 0u, &m_wiggly, sizeof(Wiggly));
	wgpuQueueWriteBuffer(wgpContext.queue, m_rotationBuffer.getBuffer(), 0u, m_bulletStore.m_rots.data(), m_bulletStore.m_rots.size() * sizeof(Vector4f));
	wgpuQueueWriteBuffer(wgpContext.queue, m_offsetBuffer.getBuffer(), 0u, m_bulletStore.m_offsets.data(), m_bulletStore.m_offsets.size() * sizeof(Vector4f));

	if (m_debugCollision) {
		Matrix4f viewProjection = m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix();
		viewProjection.copy(Physics::DebugDrawer.getViewProjection());
	}

	wgpuQueueWriteBuffer(wgpContext.queue, m_spriteBuffer.getBuffer(), 0u, m_activeBillboards.data(), m_activeBillboards.size() * sizeof(SpriteInstance));
	wgpuQueueWriteBuffer(wgpContext.queue, m_muzzleBuffer.getBuffer(), 0u, &m_muzzleInstance, sizeof(SpriteInstance));
}

void Isometric::render() {
	wgpDraw();
}

void Isometric::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {

	{
		WgpRenderer::Draw(m_wgpEmissionTarget, m_wgpEmissionDepth, std::bind(&Isometric::OnDrawEmission, this, std::placeholders::_1));
	}

	{
		WgpRenderer::DrawDepth(m_wgpTextureShadow, std::bind(&Isometric::OnDrawShadow, this, std::placeholders::_1));
	}

	{
		WgpRenderer::Draw(m_wgpSceneTarget, m_wgpSceneDepth, std::bind(&Isometric::OnDrawScene, this, std::placeholders::_1));
	}

	{
		WgpRenderer::DrawColor(m_wgpBlurTempTarget, [this](const WGPURenderPassEncoder& renderPassEncoder) {
			wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_BLUR_HORIZONTAL"));
			wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindGroupBlurH, 0u, NULL);
			wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, 1u, 0u, 0u);
		});
	}
	//wgpSubmitQueue();
	{
		WgpRenderer::DrawColor(m_wgpBlurFinalTarget, [this](const WGPURenderPassEncoder& renderPassEncoder) {
			wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_BLUR_VERTICAL"));
			wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindGroupBlurV, 0u, NULL);
			wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, 1u, 0u, 0u);
		});
	}

	{
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
		
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_COMPOSITE"));
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindGroupComposite, 0u, NULL);
		wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, 1u, 0u, 0u);

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

void Isometric::OnPostDraw() {
	if (m_wantResize) {

		m_wgpEmissionTarget.resize(Application::Width, Application::Height);
		m_wgpEmissionDepth.resize(Application::Width, Application::Height);

		m_wgpSceneTarget.resize(Application::Width, Application::Height);
		m_wgpSceneDepth.resize(Application::Width, Application::Height);

		m_wgpBlurTempTarget.resize(Application::Width / 2, Application::Height / 2);
		m_wgpBlurFinalTarget.resize(Application::Width / 2, Application::Height / 2);

		wgpuBindGroupRelease(m_bindGroupComposite);
		wgpuBindGroupRelease(m_bindGroupBlurH);
		wgpuBindGroupRelease(m_bindGroupBlurV);

		m_bindGroupComposite = createBindGroupComposite();
		m_bindGroupBlurH = createBindGroupBlurH();
		m_bindGroupBlurV = createBindGroupBlurV();

		m_wantResize = false;
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
	m_wantResize = true;
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

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(9);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[1].buffer.minBindingSize = 16 * sizeof(float);

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[3].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[3].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[4].binding = 4u;
	bindingLayoutEntries[4].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[4].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[4].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[5].binding = 5u;
	bindingLayoutEntries[5].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[5].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[5].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[6].binding = 6u;
	bindingLayoutEntries[6].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[6].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Comparison;

	bindingLayoutEntries[7].binding = 7u;
	bindingLayoutEntries[7].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[7].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Depth;
	bindingLayoutEntries[7].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[8].binding = 8u;
	bindingLayoutEntries[8].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[8].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[8].buffer.minBindingSize = sizeof(PointLight);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayoutsFloor() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(9);
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

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[3].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[3].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[4].binding = 4u;
	bindingLayoutEntries[4].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[4].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[4].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[5].binding = 5u;
	bindingLayoutEntries[5].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[5].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Comparison;

	bindingLayoutEntries[6].binding = 6u;
	bindingLayoutEntries[6].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[6].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Depth;
	bindingLayoutEntries[6].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[7].binding = 7u;
	bindingLayoutEntries[7].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[7].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[7].buffer.minBindingSize = sizeof(DirectionalLight);

	bindingLayoutEntries[8].binding = 8u;
	bindingLayoutEntries[8].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[8].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[8].buffer.minBindingSize = sizeof(PointLight);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayoutsWiggly() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(10);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = sizeof(Vector4f);

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[2].buffer.minBindingSize = 1000u * sizeof(Matrix4f);

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[3].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[4].binding = 4u;
	bindingLayoutEntries[4].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[4].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[4].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[5].binding = 5u;
	bindingLayoutEntries[5].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[5].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[5].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[6].binding = 6u;
	bindingLayoutEntries[6].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[6].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[6].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[7].binding = 7u;
	bindingLayoutEntries[7].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[7].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Comparison;

	bindingLayoutEntries[8].binding = 8u;
	bindingLayoutEntries[8].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[8].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Depth;
	bindingLayoutEntries[8].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[9].binding = 9u;
	bindingLayoutEntries[9].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[9].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[9].buffer.minBindingSize = sizeof(PointLight);

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
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[1].buffer.minBindingSize = sizeof(Vector4f) * 400000u;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[2].buffer.minBindingSize = sizeof(Vector4f) * 400000u;

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

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayoutsBillboard() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(5);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = sizeof(FrameInfo);

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[2].buffer.minBindingSize = 10u * sizeof(SpriteInstance);

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

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayoutsShadow() {
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

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayoutsWigglyShadow() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = sizeof(Vector4f);
	
	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[2].buffer.minBindingSize = 1000u * sizeof(Matrix4f);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> Isometric::OnBindGroupsPlayer() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(9);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_skinBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(m_skinBuffer.getBuffer());

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].sampler = wgpContext.getSampler(SS_NEAREST_CLAMP);

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].textureView = m_wgpPlayerD.getTextureView();

	bindGroupEntries[4].binding = 4u;
	bindGroupEntries[4].textureView = m_wgpPlayerN.getTextureView();

	bindGroupEntries[5].binding = 5u;
	bindGroupEntries[5].textureView = m_wgpPlayerS.getTextureView();

	bindGroupEntries[6].binding = 6u;
	bindGroupEntries[6].sampler = wgpContext.getSampler(SS_0);

	bindGroupEntries[7].binding = 7u;
	bindGroupEntries[7].textureView = m_wgpTextureShadow.getTextureView();

	bindGroupEntries[8].binding = 8u;
	bindGroupEntries[8].buffer = m_pointLightBuffer.getBuffer();
	bindGroupEntries[8].offset = 0u;
	bindGroupEntries[8].size = wgpuBufferGetSize(m_pointLightBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_ANIMATION"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

std::vector<WGPUBindGroup> Isometric::OnBindGroupsGun() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(9);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_skinBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(m_skinBuffer.getBuffer());

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].textureView = m_wgpGunD.getTextureView();

	bindGroupEntries[4].binding = 4u;
	bindGroupEntries[4].textureView = m_wgpGunN.getTextureView();

	bindGroupEntries[5].binding = 5u;
	bindGroupEntries[5].textureView = m_wgpGunS.getTextureView();

	bindGroupEntries[6].binding = 6u;
	bindGroupEntries[6].sampler = wgpContext.getSampler(SS_0);

	bindGroupEntries[7].binding = 7u;
	bindGroupEntries[7].textureView = m_wgpTextureShadow.getTextureView();

	bindGroupEntries[8].binding = 8u;
	bindGroupEntries[8].buffer = m_pointLightBuffer.getBuffer();
	bindGroupEntries[8].offset = 0u;
	bindGroupEntries[8].size = wgpuBufferGetSize(m_pointLightBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_ANIMATION"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

std::vector<WGPUBindGroup> Isometric::OnBindGroupsFloor() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(9);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_wgpFloorD.getTextureView();

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].textureView = m_wgpFloorN.getTextureView();

	bindGroupEntries[4].binding = 4u;
	bindGroupEntries[4].textureView = m_wgpFloorS.getTextureView();

	bindGroupEntries[5].binding = 5u;
	bindGroupEntries[5].sampler = wgpContext.getSampler(SS_0);

	bindGroupEntries[6].binding = 6u;
	bindGroupEntries[6].textureView = m_wgpTextureShadow.getTextureView();

	bindGroupEntries[7].binding = 7u;
	bindGroupEntries[7].buffer = m_directionalLightBuffer.getBuffer();
	bindGroupEntries[7].offset = 0u;
	bindGroupEntries[7].size = wgpuBufferGetSize(m_directionalLightBuffer.getBuffer());

	bindGroupEntries[8].binding = 8u;
	bindGroupEntries[8].buffer = m_pointLightBuffer.getBuffer();
	bindGroupEntries[8].offset = 0u;
	bindGroupEntries[8].size = wgpuBufferGetSize(m_pointLightBuffer.getBuffer());

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

std::vector<WGPUBindGroup> Isometric::OnBindGroupsShadow() {
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
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PLAYER_SHADOW"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

WGPUBindGroup Isometric::createBindGroupWiggly() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(10);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_wigglyBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = sizeof(Vector4f);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].buffer = m_storageBuffer.getBuffer();
	bindGroupEntries[2].offset = 0u;
	bindGroupEntries[2].size = wgpuBufferGetSize(m_storageBuffer.getBuffer());

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries[4].binding = 4u;
	bindGroupEntries[4].textureView = m_wgpEnemyD.getTextureView();

	bindGroupEntries[5].binding = 5u;
	bindGroupEntries[5].textureView = m_wgpEnemyN.getTextureView();

	bindGroupEntries[6].binding = 6u;
	bindGroupEntries[6].textureView = m_wgpEnemyS.getTextureView();

	bindGroupEntries[7].binding = 7u;
	bindGroupEntries[7].sampler = wgpContext.getSampler(SS_0);

	bindGroupEntries[8].binding = 8u;
	bindGroupEntries[8].textureView = m_wgpTextureShadow.getTextureView();

	bindGroupEntries[9].binding = 9u;
	bindGroupEntries[9].buffer = m_pointLightBuffer.getBuffer();
	bindGroupEntries[9].offset = 0u;
	bindGroupEntries[9].size = wgpuBufferGetSize(m_pointLightBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WIGGLY"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup Isometric::CreateBindGroupShadow(const WgpBuffer& uniformBuffer, const WgpBuffer& wigglyBuffer, const WgpBuffer& storageBuffer) {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(uniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = wigglyBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = sizeof(Vector4f);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].buffer = storageBuffer.getBuffer();
	bindGroupEntries[2].offset = 0u;
	bindGroupEntries[2].size = wgpuBufferGetSize(storageBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WIGGLY_SHADOW"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup Isometric::createBindGroupBillboard() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(5);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_infoBufferBillboard.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(m_infoBufferBillboard.getBuffer());

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].buffer = m_spriteBuffer.getBuffer();
	bindGroupEntries[2].offset = 0u;
	bindGroupEntries[2].size = wgpuBufferGetSize(m_spriteBuffer.getBuffer());

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[4].binding = 4u;
	bindGroupEntries[4].textureView = m_sprite.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_BILLBOARD"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = (WGPUBindGroupEntry*)bindGroupEntries.data();
	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup Isometric::createBindGroupMuzzle() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(5);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_infoBufferMuzzle.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(m_infoBufferMuzzle.getBuffer());

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].buffer = m_muzzleBuffer.getBuffer();
	bindGroupEntries[2].offset = 0u;
	bindGroupEntries[2].size = wgpuBufferGetSize(m_muzzleBuffer.getBuffer());

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[4].binding = 4u;
	bindGroupEntries[4].textureView = m_muzzle.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_MUZZLE"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = (WGPUBindGroupEntry*)bindGroupEntries.data();
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

void Isometric::spawnBillboard(const Vector3f& position) {
	SpriteInstance billboard;
	billboard.position[0] = position[0];
	billboard.position[1] = 120.0f * 0.0044f;
	billboard.position[2] = position[2];

	billboard.scale[0] = 0.25f;
	billboard.scale[1] = 0.25f;
	billboard.age = 0.0f;
	billboard.currentFrame = 0u;

	m_activeBillboards.push_back(billboard);
}

void Isometric::resetMuzzle() {
	if (m_muzzleInstance.currentFrame >= 6u) {
		m_muzzleInstance.position[0] = 0.0f;
		m_muzzleInstance.position[1] = 0.0f;
		m_muzzleInstance.position[2] = 0.0f;
		m_muzzleInstance.scale[0] = 1.0f;
		m_muzzleInstance.scale[1] = 1.0f;
		m_muzzleInstance.currentFrame = 0u;
		m_muzzleInstance.age = 0.0f;
	}
}

void Isometric::updateBillboards(float dt) {
	const uint32_t numCols = 11;
	const float timePerSprite = 0.05f;
	const float spritesheetDur = numCols * timePerSprite;

	for (auto& sprite : m_activeBillboards) {
		sprite.age += dt;
		sprite.currentFrame = sprite.age / timePerSprite;
	}

	m_activeBillboards.erase(
		std::remove_if(
			m_activeBillboards.begin(),
			m_activeBillboards.end(),
			[spritesheetDur](const SpriteInstance& s) {
				return s.age >= spritesheetDur;
			}
		),
		m_activeBillboards.end()
	);
}

void Isometric::updateMuzzle(float dt, float x, float y, float z) {
	const uint32_t numCols = 6;
	const float timePerSprite = 0.05f;
	
	if (m_muzzleFlashSpritesAge.size() > 0) {
		for (size_t i = 0; i < m_muzzleFlashSpritesAge.size(); ++i) {
			m_muzzleFlashSpritesAge[i] += dt;
		}

		const float maxAge = numCols * timePerSprite;
		m_muzzleFlashSpritesAge.erase(
			std::remove_if(m_muzzleFlashSpritesAge.begin(), m_muzzleFlashSpritesAge.end(),
				[maxAge](const float f) { return f >= maxAge; }),
			m_muzzleFlashSpritesAge.end()
		);
	}

	float minAge = 1000.0f;
	for (const float a : m_muzzleFlashSpritesAge) {
		minAge = std::min(a, minAge);
	}

	PointLight point;
	point.color[0] = 1.0f; point.color[1] = 0.2f; point.color[2] = 0.0f; point.color[3] = 1.0f;
	point.padding = 0.0f;
	point.position[0] = x;
	point.position[1] = y;
	point.position[2] = z;
	point.active = (minAge < 0.03f) ? 1u : 0u;

	wgpuQueueWriteBuffer(wgpContext.queue, m_pointLightBuffer.getBuffer(), 0u, &point, sizeof(PointLight));

	if (m_muzzleInstance.currentFrame < 6u) {		
		const float spritesheetDur = numCols * timePerSprite;
		m_muzzleInstance.age += dt;
		m_muzzleInstance.currentFrame = m_muzzleInstance.age / timePerSprite;	
	}
}

void Isometric::OnDrawShadow(const WGPURenderPassEncoder& renderPassEncoder) {
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_PLAYER_SHADOW"));
	m_wgpPlayer.setBindGroupsSlot("SHADOW");
	m_wgpPlayer.draw(renderPassEncoder);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WIGGLY_SHADOW"));
	m_wgpEnemy.setBindGroupsSlot("SHADOW");
	m_wgpEnemy.draw(renderPassEncoder, m_cpuInstanceBuffer.size());


	m_wgpEnemy.setBindGroupsSlot("BG");
	m_wgpPlayer.setBindGroupsSlot("BG");
}

void Isometric::OnDrawEmission(const WGPURenderPassEncoder& renderPassEncoder) {
	wgpContext.setClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_PLAYER_EMISSION"));
	m_wgpPlayer.setBindGroupsSlot("EMISSION");
	m_wgpPlayer.draw(renderPassEncoder);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_FLOOR_EMISSION"));
	m_wgpFloor.setBindGroupsSlot("EMISSION");
	m_wgpFloor.draw(renderPassEncoder);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_BULLET_EMISSION"));
	m_wgpBullet.draw(renderPassEncoder, m_bulletStore.m_rots.size());

	m_wgpFloor.setBindGroupsSlot("BG");
	wgpContext.setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
}

void Isometric::OnDrawScene(const WGPURenderPassEncoder& renderPassEncoder) {
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_FLOOR"));
	m_wgpFloor.draw(renderPassEncoder);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WIGGLY"));
	m_wgpEnemy.draw(renderPassEncoder, m_cpuInstanceBuffer.size());

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_ANIMATION"));
	m_wgpPlayer.draw(renderPassEncoder);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_BILLBOARD"));
	wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindGroupBillboard, 0u, NULL);
	wgpuRenderPassEncoderDraw(renderPassEncoder, 4u, m_activeBillboards.size(), 0u, 0u);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_MUZZLE"));
	wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindGroupMuzzle, 0u, NULL);
	wgpuRenderPassEncoderDraw(renderPassEncoder, 4u, 1u, 0u, 0u);
}

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayoutsEmission() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(4);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[1].buffer.minBindingSize = 16 * sizeof(float);

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[3].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[3].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> Isometric::OnBindGroupsPlayerEmission() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(4);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_skinBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(m_skinBuffer.getBuffer());

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].textureView = m_wgpPlayerE.getTextureView();


	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PLAYER_EMISSION"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

std::vector<WGPUBindGroup> Isometric::OnBindGroupsGunEmission() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(4);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_skinBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(m_skinBuffer.getBuffer());

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].textureView = m_wgpGunE.getTextureView();


	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PLAYER_EMISSION"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayoutsFloorEmission() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(1);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> Isometric::OnBindGroupsFloorEmission() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(1);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_FLOOR_EMISSION"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayoutsComposite() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(4);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_UnfilterableFloat;
	bindingLayoutEntries[1].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	bindingLayoutEntries[1].texture.multisampled = true;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[3].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[3].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

WGPUBindGroup Isometric::createBindGroupComposite() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(4);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].textureView = m_wgpSceneTarget.getTextureView();

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_wgpBlurFinalTarget.getTextureView();

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].textureView = m_wgpEmissionTarget.getTextureView();
	
	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_COMPOSITE"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayoutsBlur() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(2);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[1].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

WGPUBindGroup Isometric::createBindGroupBlurH() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(2);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].textureView = m_wgpEmissionTarget.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_BLUR_HORIZONTAL"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup Isometric::createBindGroupBlurV() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(2);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].textureView = m_wgpBlurTempTarget.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_BLUR_VERTICAL"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}