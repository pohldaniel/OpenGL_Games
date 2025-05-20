#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <engine/DebugRenderer.h>
#include <Physics/ShapeDrawer.h>
#include <States/Menu.h>
#include <Utils/BinaryIO.h>
#include "Adrian.h"
#include "Application.h"
#include "Globals.h"
#include "Renderer.h"

Adrian::Adrian(StateMachine& machine) : State(machine, States::ADRIAN),
	m_camera(Application::Width, Application::Height),
	m_addedTiles(0, [](const std::array<int, 2>& p) {  return std::hash<int>()(p[0]) ^ std::hash<int>()(p[1]) << 1; }, [](const std::array<int, 2>& p1, const std::array<int, 2>& p2) { return p1[0] == p2[0] && p1[1] == p2[1]; }),
	m_streamingDistance(6),
	m_globalUserIndex(-1),
	m_fade(m_fadeValue),
	m_separaionWeight(3.0f){

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	//m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -static_cast<float>(Application::Width) / m_zoom, static_cast<float>(Application::Width) / m_zoom);
	m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -5000.0f, 5000.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);
	Utils::NavIO navIO;
	Material::AddTexture("data/models/dynamic/hero/hero.tga");
	Material::AddTexture("res/textures/los.tga");
	Material::AddTexture();
	Material::AddTexture("res/textures/crate.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/barrel.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/aircraftgun.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/generalsbuilding.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/wall.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::GetTextures().back().setWrapMode(GL_REPEAT);
	Material::AddTexture("res/textures/200x200building.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/bunker.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/metal.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/ground.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();

	m_hero.load("data/models/dynamic/hero/hero.md2");

	WorkQueue::Init(0);
	Renderer::Get().init(new Octree(m_camera, m_frustum, m_dt), new SceneNodeLC());
	m_octree = Renderer::Get().getOctree();
	m_octree->setUseOcclusionCulling(false);
	m_octree->setUseCulling(m_useCulling);
	m_root = Renderer::Get().getScene();

	DebugRenderer::Get().setEnable(true);
	m_heroEnity = m_root->addChild<Md2Entity, Md2Model>(m_hero);
	m_heroEnity->setPosition(-780.0f, MAP_MODEL_HEIGHT_Y, 780.0f);
	m_heroEnity->setOrientation(0.0f, 0.0f, 0.0f);
	m_heroEnity->setTextureIndex(0);
	m_heroEnity->OnOctreeSet(m_octree);
	m_heroEnity->setSortKey(1);
	m_heroEnity->Md2Node::setShader(Globals::shaderManager.getAssetPointer("shape_color"));

	ShapeDrawer::Get().init(32768);
	ShapeDrawer::Get().setCamera(m_camera);
	m_heroEnity->setRigidBody(Physics::AddKinematicRigidBody(Physics::BtTransform(Physics::VectorFrom(m_heroEnity->getWorldPosition())), new btBoxShape(btVector3(0.5f, 0.5f, 0.5f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER, nullptr, false));
	//m_heroEnity->setRigidBody(Physics::AddKinematicRigidBody(Physics::BtTransform(Physics::VectorFrom(m_heroEnity->getWorldPosition())), new btBoxShape(btVector3(12.5f, 12.5f, 12.5f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER, nullptr, false));

	m_segment.buildSegmentXZ(150.0f, -30.0f, 30.0f, Vector3f(0.0f, 0.0f, 0.0f), 20, 20, true, false, false);
	m_segment.createBoundingBox();
	m_segment.markForDelete();
	m_segmentNode = m_heroEnity->addChild<ShapeNode, Shape>(m_segment);
	m_segmentNode->setPosition(0.0f, -MAP_MODEL_HEIGHT_Y + 0.01f, 0.0f);
	m_segmentNode->setTextureIndex(1);
	m_segmentNode->OnOctreeSet(m_octree);

	m_disk.buildDiskXZ(20.0f, Vector3f(0.0f, 0.0f, 0.0f), 20, 20, true, false, false);
	m_disk.createBoundingBox();
	m_disk.markForDelete();

	m_ground = Physics::AddStaticObject(Physics::BtTransform(btVector3(0.0f, 0.0f, 0.0f)), new  btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), -0.1f), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER, nullptr);

	loadBuilding("res/building_0.bld");
	loadBuilding("res/building_1.bld");
	loadBuilding("res/building_2.bld");
	loadBuilding("res/building_3.bld");
	loadBuilding("res/building_5.bld");
	loadBuilding("res/building_6.bld");
	loadBuilding("res/building_8.bld");
	loadBuilding("res/building_9.bld");
	loadBuilding("res/building_7.bld");	
	loadBuilding("res/building_4.bld", true);

	loadBuilding("res/building_10.bld", true);
	loadBuilding("res/building_11.bld", true);
	loadBuilding("res/building_12.bld");
	loadBuilding("res/building_13.bld", true);
	loadBuilding("res/building_14.bld");
	loadBuilding("res/building_15.bld", true);

	loadBuilding("res/building_16.bld", true);
	loadBuilding("res/building_17.bld");
	loadBuilding("res/building_18.bld");
	loadBuilding("res/building_19.bld");
	loadBuilding("res/building_20.bld");
	
	loadBuilding("res/building_21.bld", true);
	loadBuilding("res/building_22.bld");
	loadBuilding("res/building_23.bld", true);
	loadBuilding("res/building_24.bld", true);
	loadBuilding("res/building_25.bld");

	m_navigationMesh = new NavigationMesh();
	createScene();

	m_navigationMesh->setPadding(Vector3f(0.0f, 10.0f, 0.0f));
	m_navigationMesh->setTileSize(128);

	m_navigationMesh->setCellSize(0.3);
	m_navigationMesh->setCellHeight(0.2f);

	m_navigationMesh->setAgentMaxSlope(45.0f);
	m_navigationMesh->setAgentMaxClimb(0.9f);
	m_navigationMesh->setAgentHeight(2.0f);
	m_navigationMesh->setAgentRadius(0.6f);
	
	//m_navigationMesh->build();
	//saveNavigationData();
	//navIO.writeNavigationMap("res/data_edit.nav", m_navigationMesh->getNumTilesX(), m_navigationMesh->getNumTilesZ(), m_navigationMesh->getBoundingBox(), m_navigationMesh->getTileData());

	navIO.readNavigationMap("res/data_edit.nav", m_navigationMesh->numTilesX(), m_navigationMesh->numTilesZ(), m_navigationMesh->boundingBox(), m_navigationMesh->tileData());
	m_navigationMesh->allocate();
	m_navigationMesh->addTiles();

	m_editPolygons.push_back(EditPolygon());
	m_currentPolygon = &m_editPolygons.back();

	m_depthBuffer.create(Application::Width, Application::Height);
	m_depthBuffer.attachTexture2D(AttachmentTex::DEPTH24);
	m_sphere.fromObj("res/models/sphere.obj");

	m_fade.setTransitionEnd(true);
	m_fade.setTransitionSpeed(3.0f);

	m_crowdManager = new CrowdManager();
	m_crowdManager->setNavigationMesh(m_navigationMesh);

	spawnAgent(m_heroEnity->getWorldPosition() - Vector3f(0.0f, 26.0f, 0.0f));
}

Adrian::~Adrian() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	Material::CleanupTextures();
	Renderer::Get().shutdown();
	Renderer::Get().shutdown();
	ShapeDrawer::Get().shutdown();

	delete m_navigationMesh;
	m_navigationMesh = nullptr;

	WorkQueue::Shutdown();
	Physics::DeleteAllCollisionObjects();
}

void Adrian::fixedUpdate() {
	m_heroEnity->fixedUpdate(m_fdt);
	Globals::physics->stepSimulation(m_fdt);
}

void Adrian::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
		m_camera.moveDown();
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
		m_camera.moveUp();
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		direction += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
		m_camera.moveRight();
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(1.0f, 0.0f, 0.0f);
		move |= true;
		m_camera.moveLeft();
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		if (Keyboard::instance().keyDown(Keyboard::KEY_LSHIFT)) {
			m_height += 0.05f;
			m_camera.setHeight(m_height);
		}else {
			m_angle += 0.005f;
			m_camera.rotate(m_angle);
		}
		
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		if (Keyboard::instance().keyDown(Keyboard::KEY_LSHIFT)) {
			m_height -= 0.05f;
			m_camera.setHeight(m_height);
		}else {
			m_angle -= 0.005f;
			m_camera.rotate(m_angle);
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_useStreaming = !m_useStreaming;
		toggleStreaming(m_useStreaming);
	}

	if (keyboard.keyPressed(Keyboard::KEY_F)) {
		m_fade.toggleFade();
	}

	Vector3f moveDir = Vector3f::ZERO;
	if (keyboard.keyDown(Keyboard::KEY_UP))
		moveDir += Vector3f::RIGHT;
	if (keyboard.keyDown(Keyboard::KEY_DOWN))
		moveDir += Vector3f::LEFT;
	if (keyboard.keyDown(Keyboard::KEY_RIGHT))
		moveDir += Vector3f::FORWARD;
	if (keyboard.keyDown(Keyboard::KEY_LEFT))
		moveDir += Vector3f::BACKWARD;

	if (moveDir.lengthSq() > 0.0f)
		Vector3f::Normalize(moveDir);

	m_heroEnity->translateRelative(moveDir);

	m_octree->updateFrameNumber();

	m_crowdManager->update(m_dt);
	m_heroEnity->update(m_dt);
	m_fade.update(m_dt);

	m_frustum.updatePlane(m_camera.getOrthographicMatrix(), m_camera.getViewMatrix());
	m_frustum.updateVertices(m_camera.getOrthographicMatrix(), m_camera.getViewMatrix());
	m_frustum.m_frustumSATData.calculate(m_frustum);

	if (m_useStreaming)
		updateStreaming();

	m_octree->updateOctree();
}

void Adrian::render() {

	m_depthBuffer.bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	renderSceneDepth();
	m_depthBuffer.unbind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderScene();

	glDepthMask(GL_FALSE);
	glCullFace(GL_FRONT);
	renderBubble();

	glCullFace(GL_BACK);
	renderBubble();

	glDepthMask(GL_TRUE);

	auto shader = Globals::shaderManager.getAssetPointer("shape_color");
	shader->use();
	Globals::textureManager.get("null").bind(0);
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	
	if (m_drawPolygon) {
		for (const EditPolygon& editPolygon : m_editPolygons) {
			
			for (int i = editPolygon.userPointerOffset, j = 0; i < editPolygon.userPointerOffset + editPolygon.size; i++, j++) {			
				shader->loadMatrix("u_model", Matrix4f::Translate(m_edgePoints[i]) * Matrix4f::Scale(m_markerSize, m_markerSize, m_markerSize));
				shader->loadVector("u_color", i == m_globalUserIndex ? Vector4f(1.0f, 0.0f, 0.0f, 1.0f) : Vector4f::ONE);
				Globals::shapeManager.get("sphere").drawRaw();
				if (editPolygon.size > 1 && i < editPolygon.userPointerOffset + editPolygon.size - 1) {
					DebugRenderer::Get().AddLine(m_edgePoints[i] + Vector3f(0.0f, 1.0f, 0.0f), m_edgePoints[i + 1] + Vector3f(0.0f, 1.0f, 0.0f), Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
				}

				if (i == editPolygon.userPointerOffset + editPolygon.size - 1) {
					DebugRenderer::Get().AddLine(m_edgePoints[i] + Vector3f(0.0f, 1.0f, 0.0f), m_edgePoints[editPolygon.userPointerOffset] + Vector3f(0.0f, 1.0f, 0.0f), Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
				}
			}
		}
	}
	shader->unuse();

	if (m_debugTree) {
		for (size_t i = 0; i < m_octree->getRootLevelOctants().size(); ++i) {
			const Octree::ThreadOctantResult& result = m_octree->getOctantResults()[i];
			for (auto oIt = result.octants.begin(); oIt != result.octants.end(); ++oIt) {
				Octant* octant = oIt->first;
				if (m_debugTree)
					octant->OnRenderAABB(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

				const std::vector<OctreeNode*>& drawables = octant->getOctreeNodes();
				for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt) {
					(*dIt)->OnRenderAABB(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
				}
			}
		}
	}

	if (m_debugPhysic) {
		ShapeDrawer::Get().setProjectionView(m_camera.getOrthographicMatrix(), m_camera.getViewMatrix());

		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		ShapeDrawer::Get().drawDynmicsWorld(Physics::GetDynamicsWorld());
		glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
		//glEnable(GL_CULL_FACE);
	}

	if (m_debugNavmesh) {
		m_navigationMesh->OnRenderDebug();
		m_crowdManager->OnRenderDebug();
	}

	if (m_debugTree || m_debugNavmesh || m_drawPolygon) {
		DebugRenderer::Get().SetProjectionView(m_camera.getOrthographicMatrix(), m_camera.getViewMatrix());
		DebugRenderer::Get().drawBuffer();
	}

	if (m_drawUi)
		renderUi();
}

void Adrian::renderScene() {

	int tilex, tilez;

	const int TILE_SIZE = 3000;
	const float TILE_LOWFACTOR = 4.0;
	const float TILE_HIGHFACTOR = 4.0;
	int tileSzFactor = TILE_HIGHFACTOR + TILE_LOWFACTOR;

	tilex = ((int)m_camera.m_initx / TILE_SIZE) * TILE_SIZE;
	tilez = ((int)m_camera.m_initz / TILE_SIZE) * TILE_SIZE;

	Globals::textureManager.get("ground").bind(0);
	auto shader = Globals::shaderManager.getAssetPointer("map");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::Translate(tilex, 0.0f, tilez) * Matrix4f::Scale(TILE_SIZE * TILE_LOWFACTOR, 0.0f, TILE_SIZE * TILE_HIGHFACTOR));
	shader->loadFloat("u_tileFactor", m_tileFactor);
	shader->loadVector("u_blendColor", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Globals::shapeManager.get("quad_xz").drawRaw();
	shader->unuse();


	shader = Globals::shaderManager.getAssetPointer("shape_color");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadVector("u_color", m_heroEnity->getColor());

	shader = Globals::shaderManager.getAssetPointer("shape");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());

	for (const Batch& batch : m_octree->getOpaqueBatches().m_batches) {
		OctreeNode* drawable = batch.octreeNode;
		shader->loadMatrix("u_model", drawable->getWorldTransformation());
		drawable->drawRaw();
	}
}

void Adrian::renderSceneDepth() {

	int tilex, tilez;

	const int TILE_SIZE = 3000;
	const float TILE_LOWFACTOR = 4.0;
	const float TILE_HIGHFACTOR = 4.0;
	int tileSzFactor = TILE_HIGHFACTOR + TILE_LOWFACTOR;

	tilex = ((int)m_camera.m_initx / TILE_SIZE) * TILE_SIZE;
	tilez = ((int)m_camera.m_initz / TILE_SIZE) * TILE_SIZE;

	auto shader = Globals::shaderManager.getAssetPointer("depth_ortho");

	shader->use();
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::Translate(tilex, 0.0f, tilez) * Matrix4f::Scale(TILE_SIZE * TILE_LOWFACTOR, 0.0f, TILE_SIZE * TILE_HIGHFACTOR));

	Globals::shapeManager.get("quad_xz").drawRaw();

	for (const Batch& batch : m_octree->getOpaqueBatches().m_batches) {
		OctreeNode* drawable = batch.octreeNode;
		shader->loadMatrix("u_model", drawable->getWorldTransformation());
		drawable->drawRaw();
	}
}

void Adrian::renderBubble() {

	Matrix4f model = Matrix4f::Translate(m_heroEnity->getWorldPosition() - Vector3f(0.0f, 40.0f, 0.0f)) * Matrix4f::Scale(m_fadeValue * 100.0f, m_fadeValue * 100.0f, m_fadeValue * 100.0f);

	auto shader = Globals::shaderManager.getAssetPointer("bubble_new");

	shader->use();	
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", model);
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * model));
	shader->loadVector("color", Vector3f(0.0f, 0.9f, 1.0f));
	shader->loadFloat("alpha", 0.1f);
	shader->loadVector("screensize", Vector2f(static_cast<float>(Application::Width), static_cast<float>(Application::Height)));
	shader->loadFloat("scale", m_rimScale);

	m_depthBuffer.bindDepthTexture(0u);
	m_sphere.drawRaw();
}

void Adrian::OnMouseMotion(Event::MouseMoveEvent& event) {
	if (m_drawPolygon && m_mousePicker.updatePositionOrthographicAll(event.x, event.y, m_camera, m_ground)) {
		const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();
		if (m_globalUserIndex >= 0) {
			m_edgePoints[m_globalUserIndex] = Physics::VectorFrom(callbackAll.m_hitPointWorld[callbackAll.index]);

			btTransform& transform = m_collisionObjects[m_globalUserIndex]->getWorldTransform();
			transform.getOrigin() = callbackAll.m_hitPointWorld[callbackAll.index];
			m_collisionObjects[m_globalUserIndex]->setWorldTransform(transform);
		}
	}	
}

void Adrian::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
		if (!m_drawPolygon) {
			if (m_mousePicker.clickOrthographicAll(event.x, event.y, m_camera, m_ground)) {
				const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();
				Vector3f pos = Physics::VectorFrom(callbackAll.m_hitPointWorld[callbackAll.index]);
				Renderer::Get().addMarker(pos, 20.0f, 2);
				m_heroEnity->move(pos[0], pos[2]);

				//btVector3 pos = callbackAll.m_hitPointWorld[callbackAll.index];
				Vector3f pathPos = m_navigationMesh->findNearestPoint(pos, Vector3f(1.0f, 1.0f, 1.0f));
				m_crowdManager->setCrowdTarget(pathPos);
			}
		}else {
			if (Keyboard::instance().keyDown(Keyboard::KEY_LCTRL)) {
				m_edgePoints.pop_back();
				Physics::DeleteCollisionObject(m_collisionObjects.back());
				m_collisionObjects.pop_back();
				if (m_currentPolygon->size > 0)
					m_currentPolygon->size--;
			}else {
				m_edgePoints.resize(m_edgePoints.size() - m_currentPolygon->size);
				m_edgePoints.shrink_to_fit();				
				for (int i = m_currentPolygon->userPointerOffset; i < m_currentPolygon->userPointerOffset + m_currentPolygon->size; i++) {
					Physics::DeleteCollisionObject(m_collisionObjects[i]);
				}
				m_collisionObjects.resize(m_collisionObjects.size() - m_currentPolygon->size);
				m_collisionObjects.shrink_to_fit();
				m_currentPolygon->size = 0;
			}
		}
	}

	if (event.button == 1u) {
		if (!m_drawPolygon) {
			Mouse::instance().attach(Application::GetWindow(), false, false, false);
			if (m_mousePicker.clickOrthographicAll(event.x, event.y, m_camera, m_heroEnity->getRigidBody())) {
				if (!m_heroEnity->isActive()) {
					m_diskNode = m_heroEnity->addChild<ShapeNode, Shape>(m_disk);
					m_diskNode->setPosition(0.0f, -MAP_MODEL_HEIGHT_Y + 0.01f, 0.0f);
					m_diskNode->setTextureIndex(2);
					m_diskNode->setName("disk");
					m_diskNode->OnOctreeSet(m_octree);
				}else {
					ShapeNode* marker = m_heroEnity->findChild<ShapeNode>("disk");
					marker->OnOctreeSet(nullptr);
					marker->eraseSelf();
				}
				m_heroEnity->setIsActive(!m_heroEnity->isActive());
			}else {
				if (m_heroEnity->isActive()) {
					ShapeNode* marker = m_heroEnity->findChild<ShapeNode>("disk");
					marker->OnOctreeSet(nullptr);
					marker->eraseSelf();
				}
				m_heroEnity->setIsActive(false);
			}
		}else {
			Mouse::instance().attach(Application::GetWindow(), false, false, false);
			if (m_mousePicker.clickOrthographicAll(event.x, event.y, m_camera, nullptr)) {
				const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();								
				if (callbackAll.m_userIndex >= 0) {
					m_globalUserIndex = m_globalUserIndex >= 0 ? -1 : callbackAll.m_userIndex;
				}else {
					Vector3f pos = callbackAll.m_hitPointWorld[callbackAll.index];
					m_edgePoints.push_back(pos);
					btCollisionObject* collisionObject = Physics::AddKinematicObject(Physics::BtTransform(pos), new btSphereShape(m_markerSize * 0.5f), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
					collisionObject->setUserIndex(m_edgePoints.size() - 1);
					m_collisionObjects.push_back(collisionObject);
					m_currentPolygon->size = m_edgePoints.size() - m_currentPolygon->userPointerOffset;
				}
			}
		}
	}
}

void Adrian::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void Adrian::OnMouseWheel(Event::MouseWheelEvent& event) {
	if (event.direction == 1u) {
		m_zoom = m_zoom - 0.05f;
		m_zoom = Math::Clamp(m_zoom, 0.2f, 5.0f);
	}

	if (event.direction == 0u) {
		m_zoom = m_zoom + 0.05f;
		m_zoom = Math::Clamp(m_zoom, 0.2f, 5.0f);
	}
	//m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -static_cast<float>(Application::Width) / m_zoom, static_cast<float>(Application::Width) / m_zoom);
	m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -5000.0f, 5000.0f);
}

void Adrian::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Adrian::OnKeyUp(Event::KeyboardEvent& event) {

}

void Adrian::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	//m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -static_cast<float>(Application::Width) / m_zoom, static_cast<float>(Application::Width) / m_zoom);
	m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -5000.0f, 5000.0f);
	m_camera.resize(Application::Width, Application::Height);
	m_depthBuffer.resize(Application::Width, Application::Height);
}

void Adrian::renderUi() {
	ImGui_ImplOpenGL3_NewFrame();
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
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.3f, nullptr, &dockSpaceId);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.3f, nullptr, &dockSpaceId);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.3f, nullptr, &dockSpaceId);
		ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.3f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::SliderFloat("Tile", &m_tileFactor, 1.0f, 100.0f);

	if (ImGui::SliderFloat("Rotate", &m_angle, -2 * PI, 2 * PI)) {
		m_camera.rotate(m_angle);
	}

	if (ImGui::SliderFloat("Height", &m_height, 1.0f, 150.0f)) {
		m_camera.setHeight(m_height);
	}

	if (ImGui::Checkbox("Use Culling", &m_useCulling)) {
		m_octree->setUseCulling(m_useCulling);
	}
	ImGui::Checkbox("Debug Tree", &m_debugTree);
	ImGui::Checkbox("Debug Physic", &m_debugPhysic);
	ImGui::Checkbox("Debug Navmesh", &m_debugNavmesh);
	if (ImGui::Button("Clear Marker"))
		Renderer::Get().clearMarker();

	ImGui::Checkbox("Draw Polygon", &m_drawPolygon);	
	ImGui::SliderFloat("Marker Size", &m_markerSize, 0.0f, 20.0f);
	if (ImGui::Button("New Polygon")) {
		m_editPolygons.push_back(EditPolygon());
		m_currentPolygon = &m_editPolygons.back();
		m_currentPolygon->userPointerOffset = m_edgePoints.size();
	}
	ImGui::SliderFloat("Rim Scale", &m_rimScale, 0.0f, 2.0f);
	if (ImGui::Button("Bake (to File)")) {

		for (EditPolygon& editPolygon : m_editPolygons) {
			NavPolygon poly = NavPolygon();
			poly.setNumVerts(editPolygon.size);
			editPolygon.edgePoints = { m_edgePoints.begin() + editPolygon.userPointerOffset, m_edgePoints.begin() + editPolygon.userPointerOffset + editPolygon.size };
			poly.setVerts(&editPolygon.edgePoints[0][0]);
			poly.setMinY(-2.0f);
			poly.setMaxY(2.0f);
			poly.createBoundingBox();
			m_navigationMesh->addNavPolygon(poly);
		}
	
		m_navigationMesh->build();
		saveNavigationData();

		Utils::NavIO navIO;
		navIO.writeNavigationMap("res/data_edit2.nav", m_navigationMesh->getNumTilesX(), m_navigationMesh->getNumTilesZ(), m_navigationMesh->getBoundingBox(), m_navigationMesh->getTileData());

		m_edgePoints.clear();
		m_edgePoints.shrink_to_fit();
		for (btCollisionObject* obj : m_collisionObjects) {
			Physics::DeleteCollisionObject(obj);
		}
		m_collisionObjects.clear();
		m_collisionObjects.shrink_to_fit();

		for (EditPolygon& editPolygon : m_editPolygons) {
			editPolygon.size = 0;
		}
	}
	ImGui::Image((ImTextureID)(intptr_t)m_depthBuffer.getDepthTexture(), ImVec2(200.0f, 200.0f), { 0.0f, 1.0f }, { 1.0f, 0.0f });
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Adrian::loadBuilding(const char* fn, bool changeWinding) {
	FILE *f;
	char buf[1024];
	char filepath[256];
	strncpy(filepath, fn, 256);

	if ((f = fopen(filepath, "r")) == NULL) {
		fprintf(stderr, "Cannot open building file: %s\n", filepath);
		exit(-1);
	}

	std::vector<Vector3f> positions;
	std::vector<Vector2f> texels;
	
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::vector<Vector4f> shapeColor;

	Vector4f currentColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	std::string currentTexture;
	unsigned int currentPolygon;

	while (fgets(buf, 1024, f) != NULL) {
		char *str = buf + 5;
		float fl[5];
		int ret;

		if (!strncmp(buf, "#", 1))
			continue;

		for (int i = 0; str[i]; i++)
			if (str[i] == '\n' || str[i] == '\r')
				str[i] = '\0';

		ret = sscanf(str, "%f,%f,%f,%f,%f", &fl[0], &fl[1], &fl[2], &fl[3], &fl[4]);
		if (!strncmp(buf, "txco:", 5)) {
			texels.push_back({ fl[0], fl[1] });
		}else if (!strncmp(buf, "colr:", 5)) {
			currentColor.set(fl[0], fl[1], fl[2], fl[3]);
		}else if (!strncmp(buf, "vrtx:", 5)) {
			positions.push_back({ fl[0], fl[1], fl[2] });
		}else if (!strncmp(buf, "begn:", 5)) {
			sscanf(str, "%d", &currentPolygon);
		}else if (!strncmp(buf, "txtr:", 5)) {
			if (currentTexture != str && !currentTexture.empty()) {
				m_buildings.push_back(Shape(vertices, indices, 8u));
				m_buildings.back().createBoundingBox();
				m_buildings.back().setVec4Attribute(shapeColor, 0u, 3u);

				vertices.shrink_to_fit();
				vertices.clear();
				indices.shrink_to_fit();
				indices.clear();
				shapeColor.shrink_to_fit();
				shapeColor.clear();
			}
			currentTexture = str;
		}else if (!strncmp(buf, "ends", 4)) {
		
			if (currentPolygon == 6u) {
				unsigned int baseIndex = vertices.size() / 8;
				for (int i = 0; i < positions.size(); i++) {
					vertices.push_back(positions[i][0]); vertices.push_back(positions[i][1]); vertices.push_back(positions[i][2]);
					if (texels.size() > i) {
						vertices.push_back(texels[i][0]); vertices.push_back(texels[i][1]);
					}
					else {
						vertices.push_back(0.0f); vertices.push_back(0.0f);
					}
					vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
					shapeColor.push_back(currentColor);
				}

				for (unsigned int i = 1u; i < positions.size() - 1; i++) {
					indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? i + 1u + baseIndex : i + baseIndex); indices.push_back(changeWinding ? i+ baseIndex : i + 1u + baseIndex);
				}

				positions.shrink_to_fit();
				positions.clear();
				texels.shrink_to_fit();
				texels.clear();
			}else if (currentPolygon == 7u) {
				unsigned int baseIndex = vertices.size() / 8;
				for (int i = 0; i < positions.size(); i++) {
					vertices.push_back(positions[i][0]); vertices.push_back(positions[i][1]); vertices.push_back(positions[i][2]);
					if (texels.size() > i) {						
						vertices.push_back(texels[i][0]); vertices.push_back(texels[i][1]);
					}else {
						vertices.push_back(0.0f); vertices.push_back(0.0f);
					}
					vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
					shapeColor.push_back(currentColor);
				}
			
				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 2u + baseIndex : 1u + baseIndex); indices.push_back(changeWinding ? 1u + baseIndex : 2u + baseIndex);
				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 3u + baseIndex : 2u + baseIndex); indices.push_back(changeWinding ? 2u + baseIndex : 3u + baseIndex);

				
				positions.shrink_to_fit();
				positions.clear();
				texels.shrink_to_fit();
				texels.clear();
			}else if (currentPolygon == 8u) {

				unsigned int baseIndex = vertices.size() / 8;
				for (int i = 0; i < positions.size(); i++) {
					vertices.push_back(positions[i][0]); vertices.push_back(positions[i][1]); vertices.push_back(positions[i][2]);
					if (texels.size() > i) {
						vertices.push_back(texels[i][0]); vertices.push_back(texels[i][1]);
					}else {
						vertices.push_back(0.0f); vertices.push_back(0.0f);
					}
					vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
					shapeColor.push_back(currentColor);
				}
				
				for (unsigned int i = 0; i < positions.size() - 2; i = i + 2) {
					indices.push_back(0u + i + baseIndex); indices.push_back(changeWinding ? 1u + i + baseIndex : 2u + i + baseIndex); indices.push_back(changeWinding ? 2u + i + baseIndex : 1u + i + baseIndex);
					indices.push_back(1u + i + baseIndex); indices.push_back(changeWinding ? 3u + i + baseIndex : 2u + i + baseIndex); indices.push_back(changeWinding ? 2u + i + baseIndex : 3u + i + baseIndex);
				}

				positions.shrink_to_fit();
				positions.clear();
				texels.shrink_to_fit();
				texels.clear();
			}else if (currentPolygon == 9u) {
				
				unsigned int baseIndex = vertices.size() / 8;
				for (int i = 0; i < 8; i++) {
					vertices.push_back(positions[i][0]); vertices.push_back(positions[i][1]); vertices.push_back(positions[i][2]);
					if (texels.size() > i) {
						vertices.push_back(texels[i][0]); vertices.push_back(texels[i][1]);
					}else {
						vertices.push_back(0.0f); vertices.push_back(0.0f);
					}
					vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
					shapeColor.push_back(currentColor);
				}
				

				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 2u + baseIndex : 1u + baseIndex); indices.push_back(changeWinding ? 1u + baseIndex : 2u + baseIndex);
				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 3u + baseIndex : 2u + baseIndex); indices.push_back(changeWinding ? 2u + baseIndex : 3u + baseIndex);

				indices.push_back(0u + baseIndex);  indices.push_back(changeWinding ? 7u + baseIndex : 6u + baseIndex); indices.push_back(changeWinding ? 6u + baseIndex : 7u + baseIndex);
				indices.push_back(0u + baseIndex);  indices.push_back(changeWinding ? 6u + baseIndex : 5u + baseIndex); indices.push_back(changeWinding ? 5u + baseIndex : 6u + baseIndex);

				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 5u + baseIndex : 4u + baseIndex); indices.push_back(changeWinding ? 4u + baseIndex : 5u + baseIndex);
				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 4u + baseIndex : 3u + baseIndex); indices.push_back(changeWinding ? 3u + baseIndex : 4u + baseIndex);

							
				positions.shrink_to_fit();
				positions.clear();
				texels.shrink_to_fit();
				texels.clear();
			}
			
		}		
	}
	m_buildings.push_back(Shape(vertices, indices, 8u));
	m_buildings.back().createBoundingBox();
	m_buildings.back().setVec4Attribute(shapeColor, 0u, 3u);
}

void Adrian::createScene(bool recreate) {

	for (int i = 0; i < 4; i++) {
		m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[i]);
		m_buildingNode->setTextureIndex(7);
		m_buildingNode->OnOctreeSet(m_octree);
		m_navigationMesh->addNavArea(NavArea(m_buildings[i].getAABB()));
	}

	for (int i = 4; i < 8; i++) {
		m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[i]);
		m_buildingNode->setTextureIndex(i < 6 ? 3 : 4);
		m_buildingNode->OnOctreeSet(m_octree);
	}

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[8]);
	m_buildingNode->setTextureIndex(5);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[9]);
	m_buildingNode->setTextureIndex(6);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[10]);
	m_buildingNode->setTextureIndex(3);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_buildings[10].getAABB()));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[11]);
	m_buildingNode->setTextureIndex(3);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_buildings[11].getAABB()));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[12]);
	m_buildingNode->setTextureIndex(8);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_buildings[12].getAABB()));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[13]);
	m_buildingNode->setTextureIndex(3);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_buildings[13].getAABB()));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[14]);
	m_buildingNode->setTextureIndex(7);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_buildings[14].getAABB()));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[15]);
	m_buildingNode->setTextureIndex(4);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[16]);
	m_buildingNode->setTextureIndex(4);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[17]);
	m_buildingNode->setTextureIndex(8);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_buildings[17].getAABB()));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[18]);
	m_buildingNode->setTextureIndex(7);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_buildings[18].getAABB()));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[19]);
	m_buildingNode->setTextureIndex(8);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_buildings[19].getAABB()));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[20]);
	m_buildingNode->setTextureIndex(9);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[21]);
	m_buildingNode->setTextureIndex(10);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[22]);
	m_buildingNode->setTextureIndex(10);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[23]);
	m_buildingNode->setTextureIndex(5);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[24]);
	m_buildingNode->setTextureIndex(4);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[25]);
	m_buildingNode->setTextureIndex(9);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[26]);
	m_buildingNode->setTextureIndex(9);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[27]);
	m_buildingNode->setTextureIndex(10);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(Globals::shapeManager.get("quad_xz"));
	m_buildingNode->translate(0.0f, 0.1f, 0.0f);
	m_buildingNode->setScale(1000.0f, 0.0f, 1000.0f);
	m_navigationMesh->addNavigable(Navigable(m_buildingNode));
}

void Adrian::toggleStreaming(bool enabled) {
	if (enabled) {
		int maxTiles = (2 * m_streamingDistance + 1) * (2 * m_streamingDistance + 1);
		BoundingBox boundingBox = m_navigationMesh->getBoundingBox();
		saveNavigationData();
		m_navigationMesh->allocate(boundingBox, maxTiles);
		updateStreaming();
	}else {
		rebuild();
	}
}

void Adrian::saveNavigationData() {
	m_addedTiles.clear();
	m_navigationMesh->saveToTileData();
}

void Adrian::updateStreaming() {
	Vector3f averageAgentPosition = m_heroEnity->getWorldPosition();
	const std::array<int, 2> heroTile = m_navigationMesh->getTileIndex(averageAgentPosition);
	const std::array<int, 2> numTiles = m_navigationMesh->getNumTiles();
	const std::array<int, 2> beginTile = { std::max(0, heroTile[0] - m_streamingDistance), std::max(0, heroTile[1] - m_streamingDistance) };
	const std::array<int, 2> endTile = { std::min(heroTile[0] + m_streamingDistance, numTiles[0] - 1), std::min(heroTile[1] + m_streamingDistance, numTiles[1] - 1) };

	// Remove tiles
	for (std::unordered_set<std::array<int, 2>>::iterator i = m_addedTiles.begin(); i != m_addedTiles.end();) {
		const std::array<int, 2> tileIdx = *i;
		if (beginTile[0] <= tileIdx[0] && tileIdx[0] <= endTile[0] && beginTile[1] <= tileIdx[1] && tileIdx[1] <= endTile[1])
			++i;
		else {
			m_navigationMesh->removeTile(tileIdx, 3u);
			i = m_addedTiles.erase(i);
		}
	}
	
	// Add tiles
	for (int z = beginTile[1]; z <= endTile[1]; ++z) {
		for (int x = beginTile[0]; x <= endTile[0]; ++x) {
			const std::array<int, 2> tileIdx = { x, z };
			bool tmp = m_navigationMesh->hasTile(tileIdx);
			if (!m_navigationMesh->hasTile(tileIdx) && m_navigationMesh->hasTileData(x, z)) {
				m_addedTiles.insert(tileIdx);
				m_navigationMesh->addTile(x, z);
			}
		}
	}
}

void Adrian::rebuild() {
	const  std::array<int, 2> numTiles = m_navigationMesh->getNumTiles();
	BoundingBox boundingBox = m_navigationMesh->getBoundingBox();
	m_navigationMesh->allocate(boundingBox, numTiles[0], numTiles[1]);
	m_navigationMesh->addTiles();
}

void Adrian::spawnHero(const Vector3f& pos) {

}

void Adrian::spawnAgent(const Vector3f& pos) {
	CrowdAgent* agent = m_crowdManager->addAgent(pos);

	agent->setHeight(60.0f);
	agent->setMaxSpeed(6.0f, true);
	agent->setMaxAccel(10.0f, true);
	agent->setRadius(30.0f);
	agent->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	agent->setSeparationWeight(m_separaionWeight);
	agent->initCallbacks();
}