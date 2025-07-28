#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <engine/DebugRenderer.h>
#include <engine/Fontrenderer.h>
#include <engine/TileSet.h>
#include <Physics/ShapeDrawer.h>
#include <Physics/MousePicker.h>
#include <States/AdrianMenu.h>
#include <Utils/BinaryIO.h>

#include "Adrian.h"
#include "Application.h"
#include "Globals.h"
#include "Renderer.h"

Adrian::Adrian(StateMachine& machine, const std::string& background) : State(machine, States::ADRIAN),
m_camera(Application::Width, Application::Height),
m_cameraController(m_camera, Application::Width, Application::Height),
m_addedTiles(0, [](const std::array<int, 2>& p) {  return std::hash<int>()(p[0]) ^ std::hash<int>()(p[1]) << 1; }, [](const std::array<int, 2>& p1, const std::array<int, 2>& p2) { return p1[0] == p2[0] && p1[1] == p2[1]; }),
m_streamingDistance(6),
m_globalUserIndex(-1),
m_fade(m_fadeValue),
m_fadeCircle(m_fadeCircleValue),
m_separaionWeight(3.0f),
m_currentPanelTex(-1),
m_invisible(false),
m_miniMap(m_camera, m_scene, m_entities),
m_billboard(m_camera){

	//Application::SetCursorIcon(IDC_ARROW);
	Application::SetCursorIcon(arrow);

	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	//m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -static_cast<float>(Application::Width) / m_zoom, static_cast<float>(Application::Width) / m_zoom);
	m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -5000.0f, 5000.0f);
	m_camera.setSpeed(500.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	WorkQueue::Init(1);
	Renderer::Get().init(new Octree(m_camera, m_frustum, m_dt), new SceneNodeLC());
	m_octree = Renderer::Get().getOctree();
	m_octree->setUseOcclusionCulling(false);
	m_octree->setUseCulling(m_useCulling);
	m_root = Renderer::Get().getScene();

	DebugRenderer::Get().setEnable(true);
	
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

	if (!loadPolygonCache(m_navigationMesh) || !m_drawPolygon) {
		m_editPolygons.push_back(EditPolygon());
		m_currentPolygon = &m_editPolygons.back();
	}
	Utils::NavIO navIO;
	//m_navigationMesh->build();
	//saveNavigationData();
	//navIO.writeNavigationMap("res/data_edit.nav", m_navigationMesh->getNumTilesX(), m_navigationMesh->getNumTilesZ(), m_navigationMesh->getBoundingBox(), m_navigationMesh->getTileData());

	navIO.readNavigationMap("res/data_fin.nav", m_navigationMesh->numTilesX(), m_navigationMesh->numTilesZ(), m_navigationMesh->boundingBox(), m_navigationMesh->tileData());
	m_navigationMesh->allocate();
	m_navigationMesh->addTiles();

	m_depthBuffer.create(Application::Width, Application::Height);
	m_depthBuffer.attachTexture2D(AttachmentTex::DEPTH24);
	

	m_fade.setTransitionSpeed(3.0f);
	m_fadeCircle.setTransitionSpeed(3.0f);

	m_crowdManager = new CrowdManager();
	m_crowdManager->setNavigationMesh(m_navigationMesh);

	ShapeDrawer::Get().init(32768);
	ShapeDrawer::Get().setCamera(m_camera);

	m_ground = Physics::AddStaticObject(Physics::BtTransform(btVector3(0.0f, 0.0f, 0.0f)), new  btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), -0.1f), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER, nullptr);

	spawnHero(Vector3f(-780.0f, 0.0f, 780.0f));	
	loadBots("data/maps/default/main.map");

	m_miniMap.init();

	createCollisionFilter();

	TextureAtlasCreator::Get().init(64u, 64u);
	TileSetManager::Get().getTileSet("overlay").loadTileSetCpu(std::vector<std::string>({	
		"data/textures/panel/panelhero.tga",
		"data/textures/panel/CorpsePanel.tga",
		"data/textures/panel/MutantCheeta.tga",
		"data/textures/panel/MutantLizard.tga",
		"data/textures/panel/MutantManPanel.tga",
		"data/textures/panel/Ripper.tga",
		"data/textures/panel/SkelPanel.tga",
		"data/textures/panel/panel.tga",
	}), true);


	TileSetManager::Get().getTileSet("overlay").loadTileSetGpu();
	m_tileSet = TileSetManager::Get().getTileSet("overlay").getTextureRects();
	m_atlas = TileSetManager::Get().getTileSet("overlay").getAtlas();

	Spritesheet::SetWrapMode(m_atlas, GL_REPEAT);
	Spritesheet::Bind(m_atlas, 1u);
	Sprite::GetShader()->use();
	Sprite::GetShader()->loadInt("u_texture", 1);
	Sprite::GetShader()->unuse();

	activateHero();
	m_scene.loadBackground(background);
}

Adrian::~Adrian() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	
	Renderer::Get().shutdown();
	ShapeDrawer::Get().shutdown();

	delete m_navigationMesh;
	m_navigationMesh = nullptr;

	delete m_crowdManager;
	m_crowdManager = nullptr;

	Physics::DeleteAllCollisionObjects();
	Fontrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("font_ttf"));

	WorkQueue::Shutdown();
}

void Adrian::fixedUpdate() {
	int index = 0;
	for (auto&& entity : m_entities) {
		entity->fixedUpdate(m_fdt);
		if (index != 0) {
			if (!m_invisible)
				m_hero->handleCollision(static_cast<Bot*>(entity)->getSegmentBody());

			static_cast<Bot*>(entity)->handleCollision(m_hero->getSegmentBody());
		}
		index++;
	}
	Globals::physics->stepSimulation(m_fdt);	
}

void Adrian::update() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		if (Keyboard::instance().keyDown(Keyboard::KEY_LSHIFT)) {
			m_height += m_dt * 30.0f;
			m_camera.setHeight(m_height);
		}else {
			m_angle += m_dt;
			m_camera.rotate(m_angle);
		}

	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		if (Keyboard::instance().keyDown(Keyboard::KEY_LSHIFT)) {
			m_height -= m_dt * 30.0f;
			m_camera.setHeight(m_height);
		}else {
			m_angle -= m_dt;
			m_camera.rotate(m_angle);
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_useStreaming = !m_useStreaming;
		toggleStreaming(m_useStreaming);
	}

	if (keyboard.keyPressed(Keyboard::KEY_F)) {
		m_fade.toggleFade(false);
	}

	if (keyboard.keyPressed(Keyboard::KEY_I)) {
		m_invisible = !m_invisible;
	}

	if (keyboard.keyPressed(Keyboard::KEY_U)) {
		m_noWalls = !m_noWalls;

		Utils::NavIO navIO;
		if (m_noWalls) {
			m_navigationMesh->clearTileData();
			navIO.readNavigationMap("res/data_no.nav", m_navigationMesh->numTilesX(), m_navigationMesh->numTilesZ(), m_navigationMesh->boundingBox(), m_navigationMesh->tileData());			
			m_navigationMesh->allocate();						
			m_crowdManager->resetNavMesh(m_navigationMesh->getDetourNavMesh());
			m_crowdManager->initNavquery(m_navigationMesh->getDetourNavMesh());
			m_navigationMesh->addTiles();
			m_crowdManager->reCreateCrowd();
		}else {
			m_navigationMesh->clearTileData();
			navIO.readNavigationMap("res/data_fin.nav", m_navigationMesh->numTilesX(), m_navigationMesh->numTilesZ(), m_navigationMesh->boundingBox(), m_navigationMesh->tileData());
			m_navigationMesh->allocate();						
			m_crowdManager->resetNavMesh(m_navigationMesh->getDetourNavMesh());
			m_crowdManager->initNavquery(m_navigationMesh->getDetourNavMesh());
			m_navigationMesh->addTiles();
			m_crowdManager->reCreateCrowd(Vector3f(1000.0f, 0.0f, 1000.0f));
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_B)) {
		const Vector3f& pos = m_hero->getWorldPosition();
		if (pos[0] <= -580 && pos[2] <= -640) {
			m_roatecamera = true;
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_M)) {
		m_showPanel = !m_showPanel;
	}

	if (keyboard.keyPressed(Keyboard::KEY_F1)) {
		m_showHelp = !m_showHelp;
	}

	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {
		centerHero();
	}

	if (keyboard.keyPressed(Keyboard::KEY_H)) {
		activateHero();
	}

	m_cameraController.update(m_dt);
	
	m_octree->updateFrameNumber();
	m_crowdManager->update(m_dt);

	if (m_hero->isDeath() || m_roatecamera) {
		m_hero->update(m_dt);
	}else {		
		for (auto&& entity : m_entities)
			entity->update(m_dt);
	}

	if (m_roatecamera) {
		m_angle -= m_dt;
		m_camera.rotate(m_angle);
	}

	m_fade.update(m_dt);
	m_fadeCircle.update(m_dt);

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

	if (m_showPanel) {

		glClear(GL_DEPTH_BUFFER_BIT);

		m_panel.setPosition(0.0f, 0.0f);
		m_panel.setScale(static_cast<float>(Application::Width), (50.0f / 480.0f) * static_cast<float>(Application::Height));
		m_panel.draw(m_tileSet[7], Vector4f::ONE, false, 10.0f, 1.0f);

		m_miniMap.draw();

		Fontrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("font"));
		if (m_currentPanelTex >= 0) {
			m_panel.setPosition(0.0f, (10.0f / 480.0f) * static_cast<float>(Application::Height));
			m_panel.setScale((100.0f / 640.0f) * static_cast<float>(Application::Width), (100.0f / 480.0f) * static_cast<float>(Application::Height));
			m_panel.draw(m_tileSet[m_currentPanelTex], Vector4f::ONE, false, 1.0f, 1.0f);


			Fontrenderer::Get().addText(m_scene.characterSet, (120.0f / 640.0f) * static_cast<float>(Application::Width), 20.0f, std::get<0>(m_scene.labels[m_currentPanelTex]), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (120.0f / 640.0f) * static_cast<float>(Application::Width), 0.0f, std::get<1>(m_scene.labels[m_currentPanelTex]), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			//Fontrenderer::Get().drawBuffer();	
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_scene.characterSet.spriteSheet);

		Fontrenderer::Get().addText(m_scene.characterSet, (500.0f / 640.0f) * static_cast<float>(Application::Width), (450.0f / 480.0f) * static_cast<float>(Application::Height), "F1:HELP", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
		if (m_showHelp) {
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (450.0f / 480.0f) * static_cast<float>(Application::Height), "F2 : QUIT GAME", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (430.0f / 480.0f) * static_cast<float>(Application::Height), "P  : PAUSE GAME", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (410.0f / 480.0f) * static_cast<float>(Application::Height), "Q  : ROTATE CAMERA ANTICLOCKWISE", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (390.0f / 480.0f) * static_cast<float>(Application::Height), "E  : ROTATE CAMERA CLOCKWISE", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);

			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (370.0f / 480.0f) * static_cast<float>(Application::Height), "M  : TOGGLE PANEL", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (350.0f / 480.0f) * static_cast<float>(Application::Height), "B  : BOMB", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (330.0f / 480.0f) * static_cast<float>(Application::Height), "H  : SELECT HERO", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (310.0f / 480.0f) * static_cast<float>(Application::Height), "SPACE  : BRINGS HERO TO SCREEEN CENTER", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);

			//Fontrenderer::Get().addText(set, (20.0f / 640.0f) * static_cast<float>(Application::Width), (290.0f / 480.0f) * static_cast<float>(Application::Height), "Q  : QUIT", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (270.0f / 480.0f) * static_cast<float>(Application::Height), " ", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (250.0f / 480.0f) * static_cast<float>(Application::Height), "LEFT CLICK THE TORSO TO SELECT", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (230.0f / 480.0f) * static_cast<float>(Application::Height), "RIGHT CLICK TO MOVE", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (210.0f / 480.0f) * static_cast<float>(Application::Height), "RIGHT CLICK ON THE ENEMY TO KILL", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);

			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (190.0f / 480.0f) * static_cast<float>(Application::Height), " ", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (170.0f / 480.0f) * static_cast<float>(Application::Height), "OBJECTIVE: ", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (150.0f / 480.0f) * static_cast<float>(Application::Height), "DESTROY THE EVIL SCIENTISTS CONTROL ROOM", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (130.0f / 480.0f) * static_cast<float>(Application::Height), "AT THE TOP RIGHT CORNER OF THE MAP BY", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
			Fontrenderer::Get().addText(m_scene.characterSet, (20.0f / 640.0f) * static_cast<float>(Application::Width), (110.0f / 480.0f) * static_cast<float>(Application::Height), "PLACING A BOMB UNDER HIS HOUSE", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true);
		}

		Fontrenderer::Get().drawBuffer();
	}

	if ((m_hero->isDeath() || m_roatecamera) && m_showPanel) {
		Sprite::SwitchShader();
		m_scene.texture1.bind();
		m_panel.setPosition(static_cast<float>(Application::Width) * 0.5f - (strlen("GAME OVER") + 1) * SCR2RESX(40) * 0.5f, static_cast<float>(Application::Height) * 0.5f - SCR2RESX(50) * 0.5f);
		m_panel.setScale((strlen("GAME OVER") + 1) * SCR2RESX(40), SCR2RESX(50));
		m_panel.draw(Vector4f::ONE, false, 1.0f, 1.0f);

		m_scene.texture2.bind();
		m_panel.setPosition(static_cast<float>(Application::Width) * 0.5f - (strlen("Press F2 to exit to Main Menu") + 1) * SCR2RESX(20) * 0.5f, static_cast<float>(Application::Height) * 0.5f - SCR2RESX(30) * 0.5 + 100.0f);
		m_panel.setScale((strlen("Press F2 to exit to Main Menu") + 1) * SCR2RESX(20), SCR2RESX(30));
		m_panel.draw(Vector4f::ONE, false, 1.0f, 1.0f);
		Sprite::SwitchShader();
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

	m_scene.background.bind();
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

	shader = MousePicker::GetShader().get();
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadFloat("u_radius", 0.7f * m_fadeCircleValue);

	shader = Globals::shaderManager.getAssetPointer("shape");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());

	for (const Batch& batch : m_octree->getOpaqueBatches().m_batches) {
		OctreeNode* drawable = batch.octreeNode;
		shader->loadMatrix("u_model", drawable->getWorldTransformation());
						
		if (drawable->getSortKey() == 4) 
			glDepthMask(GL_FALSE);

		if (drawable->getSortKey() == 5) 		
			glDepthMask(GL_TRUE);
		
		drawable->drawRaw();
	}

	m_billboard.draw();
	
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

	Matrix4f model = Matrix4f::Translate(m_hero->getWorldPosition() - Vector3f(0.0f, 30.0f, 0.0f)) * Matrix4f::Scale(m_fadeValue * 100.0f, m_fadeValue * 100.0f, m_fadeValue * 100.0f);

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
	m_scene.sphere.drawRaw();
}

void Adrian::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_cameraController.OnMouseMotion(event);
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

void Adrian::setTarget(const Vector3f& _pos) {
	Vector3f pos = Physics::VectorFrom(_pos);
	Vector3f pathPos = m_navigationMesh->findNearestPoint(pos, Vector3f(1.0f, 1.0f, 1.0f));
	if (m_currentPanelTex == 0 && m_crowdManager->setCrowdTarget(pathPos)) {
		if (!m_cursorNode) {
			m_cursorNode = m_root->addChild<ShapeNode, Shape>(Globals::shapeManager.get("quad_xz"));
			m_cursorNode->setSortKey(1);
			m_cursorNode->setScale(50.0f, 0.0f, 50.0f);
			m_cursorNode->setShader(MousePicker::GetShader().get());
			m_cursorNode->OnOctreeSet(m_octree);
		}
		m_cursorNode->setPosition(pathPos);
	}
}

void Adrian::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
		if (!m_drawPolygon) {
			if (m_mousePicker.clickOrthographicAll(event.x, event.y, m_camera, m_ground, {}, 1)) {
				const MousePickCallbackAll& cb = m_mousePicker.getCallbackAll();
				if (cb.m_userPointer2) {
					m_currentBot = static_cast<Bot*>(cb.m_userPointer2);
					bool inRange = m_currentBot->isInRange();
					if (inRange) {
						m_hero->setAnimationType(AnimationType::ATTACK);
						m_hero->setOnAnimationEnd([&m_hero = m_hero, &m_currentBot = m_currentBot]{
							m_hero->setAnimationType(AnimationType::STAND);
							m_hero->setOnAnimationEnd(nullptr);								
						});						
						m_currentBot->death();
					}
				}else {
					setTarget(Physics::VectorFrom(cb.m_hitPointWorld[cb.index]));
				}				
				m_fadeCircle.setTransitionEnd(true);
				m_fadeCircle.fadeOut(false);
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
			float nx, ny;
			if (isMouseOver(event.x, event.y, nx, ny)) {
				m_camera.scrollOver(nx * m_scene.xconvfactor, ny * m_scene.yconvfactor);
				return;
			}

			if (m_mousePicker.clickOrthographic(event.x, event.y, m_camera, nullptr, m_ground)) {
				const MousePickCallback& cb = m_mousePicker.getCallback();
				if (cb.m_userPoiner == m_hero && !m_hero->isDeath()) {					
					if (m_currentPanelTex != 0) {
						activateHero();
					}else {
						ShapeNode* disk = m_hero->findChild<ShapeNode>("disk");
						disk->OnOctreeSet(nullptr);
						disk->eraseSelf();
						m_hero->setColor(Vector4f::ONE);
						m_currentPanelTex = -1;						
					}					
				}else {					
					if (m_currentPanelTex == 0) {
						ShapeNode* disk = m_hero->findChild<ShapeNode>("disk");		
						disk->OnOctreeSet(nullptr);
						disk->eraseSelf();		
						m_hero->setColor(Vector4f::ONE);
					}
					Bot* bot = static_cast<Bot*>(cb.m_userPoiner);					
					m_currentPanelTex = m_currentPanelTex == bot->getEnemyType() && m_currentPanelTex != 0 ? -1 : bot->getEnemyType();	
				}
			}else {
				if (m_currentPanelTex == 0) {
					ShapeNode* disk = m_hero->findChild<ShapeNode>("disk");
					disk->OnOctreeSet(nullptr);
					disk->eraseSelf();
					m_hero->setColor(Vector4f::ONE);
				}
				m_currentPanelTex = -1;
			}
		}else {
			Mouse::instance().attach(Application::GetWindow(), false, false, false);
			if (m_mousePicker.clickOrthographicAll(event.x, event.y, m_camera, nullptr, m_colliosionFilter, -1)) {
				const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();
				if (callbackAll.m_userIndex1 >= 0) {
					m_globalUserIndex = m_globalUserIndex >= 0 ? -1 : callbackAll.m_userIndex1;
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

	if (event.button == 3u) {
		m_zoom = 1.0f;
		m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -5000.0f, 5000.0f);
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
	m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -5000.0f, 5000.0f);
}

void Adrian::OnKeyDown(Event::KeyboardEvent& event) {
	m_cameraController.OnKeyDown(event);
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new AdrianMenu(m_machine));
	}

	/*if (event.keyCode == 81u) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}*/

	if (event.keyCode == VK_F2) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new AdrianMenu(m_machine));
	}
}

void Adrian::OnKeyUp(Event::KeyboardEvent& event) {
	m_cameraController.OnKeyUp(event);
}

void Adrian::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	//m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -static_cast<float>(Application::Width) / m_zoom, static_cast<float>(Application::Width) / m_zoom);
	m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -5000.0f, 5000.0f);
	m_camera.resize(Application::Width, Application::Height);
	m_depthBuffer.resize(Application::Width, Application::Height);
}

void Adrian::renderUi() {

	//Application::SetCursorIcon(arrow);

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
	ImGui::SliderFloat("Rim Scale", &m_rimScale, 0.0f, 2.0f);
	if (ImGui::Checkbox("Use Culling", &m_useCulling)) {
		m_octree->setUseCulling(m_useCulling);
	}
	ImGui::Checkbox("Debug Tree", &m_debugTree);
	ImGui::Checkbox("Debug Physic", &m_debugPhysic);
	ImGui::Checkbox("Debug Navmesh", &m_debugNavmesh);
	ImGui::Checkbox("Draw Polygon", &m_drawPolygon);
	ImGui::SliderFloat("Marker Size", &m_markerSize, 0.0f, 20.0f);
	if (ImGui::Button("New Polygon")) {
		m_editPolygons.push_back(EditPolygon());
		m_currentPolygon = &m_editPolygons.back();
		m_currentPolygon->userPointerOffset = m_edgePoints.size();
	}
	
	if (ImGui::Button("Save Polygon")) {
		std::ofstream fileOut;
		fileOut.open("res/polygon_cache.txt");
		fileOut << std::setprecision(6) << std::fixed;
		fileOut << "# Polygon Cache\n";
		for (EditPolygon& editPolygon : m_editPolygons) {
			std::vector<Vector3f> edgePoints = { m_edgePoints.begin() + editPolygon.userPointerOffset, m_edgePoints.begin() + editPolygon.userPointerOffset + editPolygon.size };	
			for (int i = 0; i < edgePoints.size(); i++) {
				fileOut << "v " << edgePoints[i][0] << " " << edgePoints[i][1] << " " << edgePoints[i][2] << std::endl;
			}
			if (&editPolygon != &m_editPolygons.back())
				fileOut << "#" << std::endl;
			else
				fileOut << "#";
		}
		fileOut.close();
	}
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
		navIO.writeNavigationMap("res/data_tmp.nav", m_navigationMesh->getNumTilesX(), m_navigationMesh->getNumTilesZ(), m_navigationMesh->getBoundingBox(), m_navigationMesh->getTileData());
		
		
	}

	if (ImGui::Button("Clear Polygons")) {
		m_edgePoints.clear();
		m_edgePoints.shrink_to_fit();
		for (btCollisionObject* obj : m_collisionObjects) {
			Physics::DeleteCollisionObject(obj);
		}
		m_collisionObjects.clear();
		m_collisionObjects.shrink_to_fit();

		m_editPolygons.clear();
		m_editPolygons.shrink_to_fit();
	}

	ImGui::Image((ImTextureID)(intptr_t)m_depthBuffer.getDepthTexture(), ImVec2(200.0f, 200.0f), { 0.0f, 1.0f }, { 1.0f, 0.0f });
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}



void Adrian::loadBots(const char* filename) {
	FILE *f;
	char buf[256];
	char filepath[256];
	strncpy(filepath, filename, 256);

	if ((f = fopen(filepath, "r")) == NULL) {
		fprintf(stderr, "Cannot open building file: %s\n", filepath);
		exit(-1);
	}
	float width, height;
	fscanf(f, "%f %f", &width, &height);

	int numTextures;
	unsigned int texId;
	fscanf(f, "%d", &numTextures);
	for (int i = 0; i < numTextures; i++) {
		fscanf(f, "%s %d", buf, &texId);
	}

	int noOfBuildings, btype;	
	float bx1, by1, bx2, by2;
	char fn[256];
	fscanf(f, "%d", &noOfBuildings);
	for (int i = 0; i < noOfBuildings; i++) {
		fscanf(f, "%f %f %f %f %d %d %s", &bx1, &by1, &bx2, &by2, &btype, &texId, fn);
		//float _bx1, _by1, _bx2, _by2;
		//block_convert(_bx1, _by1, bx1, by1);
		//block_convert(_bx2, _by2, bx2, by2);
		//m_buildings_.push_back({ _bx1 + (_bx2 - _bx1) * 0.5f, -(_by1 + (_by2 - _by1) * 0.5f),  (_bx2 - _bx1) * 0.5f, (_by2 - _by1) * 0.5f });
	}

	int numGuards, gtype;
	float gx1, gy1, gx2, gy2, gspeed, gangle;
	char paneltexfn[256];
	fscanf(f, "%d", &numGuards);
	for (int i = 0; i < numGuards; i++) {
		fscanf(f, "%s %d %f %f %f %f %f %f %s", buf, &gtype, &gx1, &gy1, &gx2, &gy2, &gspeed, &gangle, paneltexfn);
		Bot* bot;
		if (gtype == 66) {
			bot = m_root->addChild<Bot, Md2Model>(m_scene.mutantman);
			bot->setTextureIndex(13);
			bot->setEnemyType(EnemyType::MUTANT_MAN);
         }else if (gtype == 68) {
			bot = m_root->addChild<Bot, Md2Model>(m_scene.hueteotl);
			bot->setTextureIndex(12);
			bot->setEnemyType(EnemyType::SKEL);
		}else if (gtype == 67) {
			bot = m_root->addChild<Bot, Md2Model>(m_scene.corpse);
			bot->setTextureIndex(14);
			bot->setEnemyType(EnemyType::CORPSE);
		}else if (gtype == 70) {
			bot = m_root->addChild<Bot, Md2Model>(m_scene.mutantlizard);
			bot->setTextureIndex(15);
			bot->setEnemyType(EnemyType::MUTANT_LIZARD);
		}else if (gtype == 69) {
			bot = m_root->addChild<Bot, Md2Model>(m_scene.mutantcheetah);
			bot->setTextureIndex(16);
			bot->setEnemyType(EnemyType::MUTANT_CHEETA);
		}else if (gtype == 71) {
			bot = m_root->addChild<Bot, Md2Model>(m_scene.ripper);
			bot->setTextureIndex(17);
			bot->setEnemyType(EnemyType::RIPPER);
		}else {
			continue;
		}
		

		if (gx1 == gx2 && gy1 == gy2) {
			bot->setPosition(gx1, 0.0f, gy1);
			bot->setOrientation(0.0f, gangle - 90.0f, 0.0f);			
			bot->setStart(gx1, 0.0f, gy1);
			bot->setEnd(gx2, 0.0f, gy2);
			bot->setSpeed(1.0f);
			bot->setMoveSpeed(0.0f);
			if(gtype == 69)
				bot->setAnimationType(AnimationType::NONE, AnimationType::STAND);
			else
				bot->setAnimationType(AnimationType::STAND);
			bot->OnOctreeSet(m_octree);
			bot->setSortKey(5);
			bot->Md2Node::setShader(Globals::shaderManager.getAssetPointer("shape_color"));
			bot->init(m_scene.segment);
		}else {
			bot->setPosition(gx1, 0.0f, gy1);
			bot->setOrientation(0.0f, gangle - 90.0f, 0.0f);
			bot->setStart(gx1, 0.0f, gy1);
			bot->setEnd(gx2, 0.0f, gy2);
			bot->setSpeed(gtype == 69 ?  0.4f : gtype == 71 ? 0.6f : gtype == 66 ? 0.5f : 0.3f);
			bot->setMoveSpeed(gtype == 69 ? 125.6f : gtype == 71 ? 175.6f : gtype == 66  ? 105.0f : 35.0f);
			bot->setAnimationType(AnimationType::RUN);
			bot->OnOctreeSet(m_octree);
			bot->setSortKey(5);
			bot->Md2Node::setShader(Globals::shaderManager.getAssetPointer("shape_color"));
			bot->init(m_scene.segment);
		}

		m_entities.push_back(bot);
		m_segmentNode = bot->addChild<ShapeNode, Shape>(m_scene.segment);
		m_segmentNode->setPosition(0.0f, 0.5f, 0.0f);
		m_segmentNode->setOrientation(0.0f, 0.0f, 0.0f);
		m_segmentNode->setScale(2.5f, 0.0f, 2.5f);
		m_segmentNode->setTextureIndex(0);
		m_segmentNode->OnOctreeSet(m_octree);
		m_segmentNode->setSortKey(4);
		m_segmentNode->setShader(Globals::shaderManager.getAssetPointer("shape_color"));
		m_segmentNode->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
		m_segmentNode->setName("segment");

		m_diskNode = bot->addChild<ShapeNode, Shape>(m_scene.disk);
		m_diskNode->setPosition(0.0f, 1.0f, 0.0f);
		m_diskNode->setTextureIndex(1);
		m_diskNode->setName("disk");
		m_diskNode->OnOctreeSet(m_octree);
		m_diskNode->setSortKey(2);
		m_diskNode->setShader(Globals::shaderManager.getAssetPointer("shape_color"));
		m_diskNode->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	}

	int numSprites;
	float sx, sz;
	fscanf(f, "%d", &numSprites);
	for (int i = 0; i < numSprites; i++) {
		fscanf(f, "%d %f %f", &texId, &sx, &sz);
	}

	fclose(f);
}

void Adrian::createScene(bool recreate) {

	for (int i = 0; i < 4; i++) {
		m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[i]);
		m_buildingNode->setTextureIndex(6);
		m_buildingNode->OnOctreeSet(m_octree);
		m_navigationMesh->addNavArea(NavArea(m_scene.buildings[i].getAABB().maximize(30.0f)));
	}

	for (int i = 4; i < 8; i++) {
		m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[i]);
		m_buildingNode->setTextureIndex(i < 6 ? 2 : 3);
		m_buildingNode->OnOctreeSet(m_octree);
	}

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[8]);
	m_buildingNode->setTextureIndex(4);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[9]);
	m_buildingNode->setTextureIndex(5);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[10]);
	m_buildingNode->setTextureIndex(2);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_scene.buildings[10].getAABB().maximize(30.0f)));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[11]);
	m_buildingNode->setTextureIndex(2);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_scene.buildings[11].getAABB().maximize(30.0f)));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[12]);
	m_buildingNode->setTextureIndex(7);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_scene.buildings[12].getAABB().maximize(30.0f)));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[13]);
	m_buildingNode->setTextureIndex(2);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_scene.buildings[13].getAABB().maximize(30.0f)));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[14]);
	m_buildingNode->setTextureIndex(6);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_scene.buildings[14].getAABB().maximize(30.0f)));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[15]);
	m_buildingNode->setTextureIndex(3);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[16]);
	m_buildingNode->setTextureIndex(3);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[17]);
	m_buildingNode->setTextureIndex(7);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_scene.buildings[17].getAABB().maximize(30.0f)));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[18]);
	m_buildingNode->setTextureIndex(6);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_scene.buildings[18].getAABB().maximize(30.0f)));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[19]);
	m_buildingNode->setTextureIndex(7);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigationMesh->addNavArea(NavArea(m_scene.buildings[19].getAABB().maximize(30.0f)));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[20]);
	m_buildingNode->setTextureIndex(8);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[21]);
	m_buildingNode->setTextureIndex(9);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[22]);
	m_buildingNode->setTextureIndex(9);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[23]);
	m_buildingNode->setTextureIndex(4);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[24]);
	m_buildingNode->setTextureIndex(3);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[25]);
	m_buildingNode->setTextureIndex(8);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[26]);
	m_buildingNode->setTextureIndex(8);
	m_buildingNode->OnOctreeSet(m_octree);

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_scene.buildings[27]);
	m_buildingNode->setTextureIndex(9);
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
	Vector3f averageAgentPosition = m_hero->getWorldPosition();
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
	m_agent = m_crowdManager->addAgent(pos);

	m_agent->setHeight(60.0f);
	m_agent->setMaxSpeed(200.0f, true);
	m_agent->setMaxAccel(FLT_MAX, true);
	m_agent->setActiveThreshold(35000.0f);
	m_agent->setArrivedScale(0.75f);
	m_agent->setCorrection(7.5f);

	m_agent->setRadius(30.0f);
	m_agent->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	m_agent->setSeparationWeight(m_separaionWeight);
	m_agent->initCallbacks();
	
	m_hero = m_root->addChild<Hero, Md2Model>(m_scene.heroModel, *m_agent);
	m_hero->Md2Node::setPosition(pos);
	m_hero->Md2Node::setOrientation(0.0f, 180.0f, 0.0f);
	m_hero->Md2Node::setTextureIndex(11);
	m_hero->OnOctreeSet(m_octree);
	m_hero->setSortKey(5);
	m_hero->Md2Node::setShader(Globals::shaderManager.getAssetPointer("shape_color"));
	m_hero->init(m_scene.segment);
	m_hero->setOnDeath([this] {
		m_currentPanelTex = -1;
		if (m_cursorNode) {
			m_cursorNode->OnOctreeSet(nullptr);
			m_cursorNode->eraseSelf();
			m_cursorNode = nullptr;
		}
	});

	m_entities.push_back(m_hero);

	m_segmentNode = m_hero->addChild<ShapeNode, Shape>(m_scene.segment);
	m_segmentNode->setPosition(0.0f, 0.5f, 0.0f);
	m_segmentNode->setOrientation(0.0f, 0.0f, 0.0f);
	m_segmentNode->setScale(1.5f, 0.0f, 1.5f);
	m_segmentNode->setTextureIndex(0);
	m_segmentNode->OnOctreeSet(m_octree);
	m_segmentNode->setSortKey(3);
	m_segmentNode->setShader(Globals::shaderManager.getAssetPointer("shape_color"));
	m_segmentNode->setColor(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	m_segmentNode->setName("segment");
}

void Adrian::spawnAgent(const Vector3f& pos) {
	m_agent = m_crowdManager->addAgent(pos);

	m_agent->setHeight(60.0f);
	m_agent->setMaxSpeed(200.0f, true);

	m_agent->setMaxAccel(FLT_MAX, true);
	m_agent->setActiveThreshold(37500.0f);
	m_agent->setArrivedScale(10.0f);
	m_agent->setCorrection(60.0f);

	m_agent->setRadius(30.0f);
	m_agent->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	m_agent->setSeparationWeight(m_separaionWeight);
	m_agent->initCallbacks();
}

bool Adrian::isMouseOver(int sx, int sy, float &nx, float &ny){

	float mx, my;
	mx = (sx - (565.0f / 640.0f) * 1024.0f) * cosf(m_camera.getAngle()) + (sy - (405.0f / 480.0f) * 768.0f) * sinf(m_camera.getAngle());
	my = (sy - (405.0f / 480.0f) * 768.0f) * cosf(m_camera.getAngle()) - (sx - (565.0f / 640.0f) * 1024.0f) * sinf(m_camera.getAngle());

	if (mx < (50.0f / 640.0f) * 1024.0f && mx > -(50.0f / 640.0f) * 1024.0f && my < (50.0f / 480.0f) * 768.0f  && my > -(50.0f / 480.0f) * 768.0f) {
		nx = mx;
		ny = my;
		return true;
	}

	return false;
}

bool Adrian::loadPolygonCache(NavigationMesh* navigationMesh) {
	FILE * pFile = fopen("res/polygon_cache.txt", "r");
	if (pFile == NULL) {
		return false;
	}
	char buffer[250];
	int size = 0;
	while (fscanf(pFile, "%s", buffer) != EOF) {
		switch (buffer[0]) {

		case 'v': {

			switch (buffer[1]) {
				case '\0': {
					float x, y, z;
					fgets(buffer, sizeof(buffer), pFile);
					sscanf(buffer, "%f %f %f", &x, &y, &z);
					m_edgePoints.push_back({ x, y, z });	
					btCollisionObject* collisionObject = Physics::AddKinematicObject(Physics::BtTransform(m_edgePoints.back()), new btSphereShape(m_markerSize * 0.5f), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
					collisionObject->setUserIndex(m_edgePoints.size() - 1);
					m_collisionObjects.push_back(collisionObject);
					size++;
				}default: {
					break;
				}
			}
			break;
		}case '#': {
			if (!m_edgePoints.size())
				break;

			m_editPolygons.push_back(EditPolygon());
			m_currentPolygon = &m_editPolygons.back();
			m_currentPolygon->size = size;
			m_currentPolygon->userPointerOffset = m_edgePoints.size() - size;
			size = 0;
		}default: {
			break;
		}

		}//end switch
	}// end while


	m_editPolygons.push_back(EditPolygon());
	m_currentPolygon = &m_editPolygons.back();
	m_currentPolygon->userPointerOffset = m_edgePoints.size();

	return true;
}

void Adrian::createCollisionFilter() {
	m_colliosionFilter.push_back(m_hero->getRigidBody());
	std::transform(m_entities.begin() + 1, m_entities.end(), std::back_inserter(m_colliosionFilter), [this](Md2Entity* p)->btCollisionObject* {
		return static_cast<Bot*>(p)->getRigidBody();
	});
}

void Adrian::activateHero() {
	m_diskNode = m_hero->addChild<ShapeNode, Shape>(m_scene.disk);
	m_diskNode->setPosition(0.0f, 0.51f, 0.0f);
	m_diskNode->setSortKey(2);
	m_diskNode->setTextureIndex(1);
	m_diskNode->setName("disk");
	m_diskNode->OnOctreeSet(m_octree);
	m_diskNode->setShader(Globals::shaderManager.getAssetPointer("shape"));
	m_hero->setColor(Vector4f(0.7f, 0.7f, 1.0f, 1.0f));
	m_currentPanelTex = 0;
}

void Adrian::centerHero() {
	Vector3f pos = m_hero->getWorldPosition();
	m_camera.scrollOver(pos[0], pos[2]);
}