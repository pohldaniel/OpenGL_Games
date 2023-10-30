#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"
#include "Utils/Rasterizer.h"
#include "Math.hpp"

#define MAX_ESTRING_LENGTH 128

Camera Game::_Camera;
float Game::ZoomFactor;
float Game::FocusPointY;
float Game::FocusPointX;

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) , m_map(_Camera) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	_Camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	_Camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	_Camera.lookAt(Vector3f(-800.0f, -1500.0f, 0.0f), Vector3f(-800.0f, -1500.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	_Camera.setRotationSpeed(0.1f);
	_Camera.setMovingSpeed(400.0f);


	ZoomFactor = 1.0f;

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);

	m_mainRT.create(Application::Width, Application::Height);
	m_mainRT.attachTexture2D(AttachmentTex::RGBA);
	m_mainRT.attachRenderbuffer(AttachmentRB::DEPTH24);

	
	TileSetManager::Get().getTileSet("map").loadTileSet("res/tilesetFrames2.bimg");
	m_atlas = TileSetManager::Get().getTileSet("map").getAtlas();
	//Spritesheet::Safe("map", m_atlas);

	FocusPointX = static_cast<float>(Application::Width / 2);
	FocusPointY = static_cast<float>(Application::Height / 2);

	TileSetManager::Get().getTileSet("sHero_anm").loadTileSet("Graphics/Animations/sHero/Controller_defs/sHero.bimg", 1280u, 1280u, true);
	//Spritesheet::Safe("sHero", TileSetManager::Get().getTileSet("sHero_anm").getAtlas());

	TileSetManager::Get().getTileSet("sArcher_anm").loadTileSet("Graphics/Animations/sArcher/Controller_defs/sArcher.bimg", 1280u, 1024u, true);
	//Spritesheet::Safe("sArcher", TileSetManager::Get().getTileSet("sArcher_anm").getAtlas());

	AnimationManager::Get().loadAnimation16("Graphics/Animations/sHero/Controller_defs/sHero.banim", TileSetManager::Get().getTileSet("sHero_anm").getTextureRects());
	AnimationManager::Get().loadAnimation8("Graphics/Animations/sArcher/Controller_defs/sArcher.banim", TileSetManager::Get().getTileSet("sArcher_anm").getTextureRects());

	PrefabManager::Get().addPrefab("sHero", TileSetManager::Get().getTileSet("sHero_anm"));
	PrefabManager::Get().getPrefab("sHero").setAnimationController(new eAnimationController("Graphics/Animations/sHero/Controller_defs/sHero.ectrl"));
	PrefabManager::Get().getPrefab("sHero").setBounds(Vector4f(-8.0f, -8.0f, 8.0f, 8.0f));
	PrefabManager::Get().getPrefab("sHero").setOffset(Vector2f(10.0f, -18.0f));
	PrefabManager::Get().getPrefab("sHero").setBoundingBox({ 16.0f, 0.0f, 40.0f, 64.0f });

	PrefabManager::Get().addPrefab("sArcher", TileSetManager::Get().getTileSet("sArcher_anm"));
	PrefabManager::Get().getPrefab("sArcher").setAnimationController(new eAnimationController("Graphics/Animations/sArcher/Controller_defs/sArcher.ectrl"));
	PrefabManager::Get().getPrefab("sArcher").setBounds(Vector4f(-8.0f, -8.0f, 8.0f, 8.0f));
	PrefabManager::Get().getPrefab("sArcher").setOffset(Vector2f(10.0f, -38.0f));
	PrefabManager::Get().getPrefab("sArcher").setBoundingBox({ 32.0f, 0.0f, 32.0f, 100.0f});

	m_entities.push_back(new Entity(PrefabManager::Get().getPrefab("sHero"), _Camera, ZoomFactor, FocusPointX, FocusPointY));
	m_entities[0]->setPosition({ -200.0f, -1110.0f });

	m_entities.push_back(new Entity(PrefabManager::Get().getPrefab("sArcher"), _Camera, ZoomFactor, FocusPointX, FocusPointY));
	m_entities[1]->setPosition({0.0f, -1120.0f});

	m_entities.push_back(new Entity(PrefabManager::Get().getPrefab("sHero"), _Camera, ZoomFactor, FocusPointX, FocusPointY));
	m_entities[2]->setPosition({ 200.0f, -1120.0f });

	m_map.loadCollision("Graphics/Maps/Tileset_defs/evilMaster2.etls");
	m_map.loadMap("res/EvilTown2.emap");
	m_map.resize(0, 0);
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {

}

void Game::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f directrion = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3f(0.0f, 1.0f * (1.0f / ZoomFactor), 0.0f);		
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, -1.0f * (1.0f / ZoomFactor), 0.0f);		
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3f(-1.0f * (1.0f / ZoomFactor), 0.0f, 0.0f);
		m_background.addOffset(-0.001f);
		m_background.setSpeed(-0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f * (1.0f / ZoomFactor), 0.0f, 0.0f);
		m_background.addOffset(0.001f);
		m_background.setSpeed(0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		directrion += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		directrion += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonPressed(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			//_Camera.rotate(dx, dy);
		}

		if (move) {
			_Camera.move(directrion * m_dt);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	m_background.update(m_dt);

	for (auto entity = m_entities.begin(); entity != m_entities.end(); entity++) {
		(*entity)->processInput();
		(*entity)->update(m_dt);
		
	}
}

void Game::render() {
	
	if (m_redrawMap) {
		m_redrawMap = !m_autoRedraw;
		m_mainRT.bindWrite();


		glClearBufferfv(GL_COLOR, 0, std::vector<float>{0.494f, 0.686f, 0.796f, 1.0f}.data());
		glClearBufferfv(GL_DEPTH, 0, &std::vector<float>{1.0f}[0]);
		//m_background.draw();
		glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
		glEnable(GL_BLEND);
		auto shader = Globals::shaderManager.getAssetPointer("batch");
		shader->use();
		shader->loadMatrix("u_transform", _Camera.getOrthographicMatrix() * _Camera.getViewMatrix());

		Spritesheet::Bind(m_atlas);
		m_map.drawRaw();
		Spritesheet::Unbind();

		for (auto entity = m_entities.begin(); entity != m_entities.end(); entity++) {
			const TextureRect& rect = (*entity)->m_animationController->currentFrame->rect;
			Spritesheet::Bind((*entity)->prefab.tileSet.getAtlas());
			Batchrenderer::Get().addQuadAA(Vector4f((*entity)->m_position[0] * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), (*entity)->m_position[1] * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), rect.width * ZoomFactor, rect.height * ZoomFactor), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), (*entity)->m_isMarked ? Vector4f(0.8f, 0.2f, 0.2f, 1.0f) : (*entity)->m_isSelected ? Vector4f(0.5f, 0.5f, 0.5f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
			Batchrenderer::Get().drawBufferRaw();
			Spritesheet::Unbind();
		}

		shader->unuse();
		glDisable(GL_BLEND);

		for (auto cell : m_map.getCollisionCells()) {
			drawIsometricRect((cell.posX + 32.0f) * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), (cell.posY + 32.0f) * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), cell.collisionRect * ZoomFactor, Vector4f(1.0f, 0.0f, 1.0f, 1.0f));
		}

		for (auto entity = m_entities.begin(); entity != m_entities.end(); entity++) {
			drawIsometricRect(((*entity)->m_position[0]) * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), ((*entity)->m_position[1]) * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), (*entity)->prefab.bounds * ZoomFactor, (*entity)->prefab.offset * ZoomFactor, Vector4f(1.0f, 0.0f, 1.0f, 1.0f));
			(*entity)->updateGridBounds();

			for (int y = (*entity)->m_minY; y <= (*entity)->m_maxY; y++) {
				for (int x = (*entity)->m_minX; x <= (*entity)->m_maxX; x++) {
					drawIsometricRect(x, y, Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
				}
			}

			drawClickBox((*entity)->m_position[0] + (*entity)->prefab.boundingBox.posX, (*entity)->m_position[1] + (*entity)->prefab.boundingBox.posY, +(*entity)->prefab.boundingBox.width, (*entity)->prefab.boundingBox.height);

			(*entity)->m_movementPlaner->DebugDraw();
			(*entity)->debugDraw();
		}
		

		if (m_drawCullingRect)
			m_map.drawCullingRect();

		if (m_mouseDown) {
			m_map.drawMouseRect(m_mouseX, m_curMouseX, m_mouseY, m_curMouseY);
		}

		m_mainRT.unbindWrite();
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadBool("u_flip", false);
	m_mainRT.bindColorTexture();
	m_zoomableQuad.drawRaw();
	m_mainRT.unbindColorTexture();
	shader->unuse();

	if (m_drawUi)
		renderUi();
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {

	if (m_selectionMode == SelectionMode::ENTITY) {
		for (auto entity = m_entities.begin(); entity != m_entities.end(); entity++) {
			(*entity)->processInput(event.x, event.y, event.button, true);
		}
	}

	m_map.OnMouseButtonDown(event);

	if (event.button == 1u) {

		float mouseViewX = static_cast<float>(event.x);
		float mouseViewY = static_cast<float>(Application::Height - event.y);

		m_mouseDown = true;
		m_mouseX = mouseViewX;
		m_mouseY = mouseViewY;
		m_curMouseX = m_mouseX;
		m_curMouseY = m_mouseY;	
	}

	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
	if (m_mouseDown) {

		float mouseViewX = static_cast<float>(event.x);
		float mouseViewY = static_cast<float>(Application::Height - event.y);

		m_curMouseX = mouseViewX;
		m_curMouseY = mouseViewY;
		m_mouseMove = true;

		if (m_selectionMode == SelectionMode::ENTITY) {

			float left = std::min(m_mouseX, m_curMouseX);
			float bottom = std::min(m_mouseY, m_curMouseY);
			float right = std::max(m_mouseX, m_curMouseX);
			float top = std::max(m_mouseY, m_curMouseY);

			for (auto entity = m_entities.begin(); entity != m_entities.end(); entity++) {
				(*entity)->mark(left, bottom, right, top);
			}
		}
	}

	m_map.OnMouseMotion(event);
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	
	if (m_selectionMode == SelectionMode::ENTITY) {
		for (auto entity = m_entities.begin(); entity != m_entities.end(); entity++) {
			if (m_mouseMove && event.button == Event::MouseButtonEvent::MouseButton::BUTTON_LEFT)
				(*entity)->select();
			else
				(*entity)->processInput(event.x, event.y, event.button, false);
		}
	}

	m_map.OnMouseButtonUp(event);

	if (event.button == 1u) {
		m_mouseDown = false;
		m_mouseMove = false;
		m_mouseX = static_cast<float>(event.x);
		m_mouseY = static_cast<float>(Application::Height - event.y);
		m_curMouseX = m_mouseX;
		m_curMouseY = m_mouseY;
	}

	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {
	if (event.direction == 1u) {
		ZoomFactor = ZoomFactor - 0.05f;
		ZoomFactor = Math::Clamp(ZoomFactor, 0.2f, 2.5f);
	}

	if (event.direction == 0u) {
		ZoomFactor = ZoomFactor + 0.05f;
		ZoomFactor = Math::Clamp(ZoomFactor, 0.2f, 2.5f);
	}
}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {

}

void Game::resize(int deltaW, int deltaH) {
	_Camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	_Camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_mainRT.resize(Application::Width, Application::Height);

	FocusPointX = static_cast<float>(Application::Width / 2);
	FocusPointY = static_cast<float>(Application::Height / 2);

	m_map.resize(deltaW, deltaH);
}

void Game::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void Game::renderUi() {
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
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::Checkbox("Use Culling", &m_map.useCulling());
	ImGui::Checkbox("Draw Culling Rect", &m_drawCullingRect);
	if (ImGui::Checkbox("Auto Redraw", &m_autoRedraw)) {
		m_redrawMap = !m_autoRedraw;
	}
	ImGui::SliderFloat("Screen Border", &m_map.screenBorder(), 0.0f, 450.0f);
	ImGui::SliderFloat("Focus Point X", &FocusPointX, -1600.0f, 1600.0f);
	ImGui::SliderFloat("Focus Point Y", &FocusPointY, -1000.0f, 1000.0f);
	ImGui::SliderFloat("Enlarge Culling Rect", &m_map.enlargeBorder(), 0.0f, 600.0f);

	int currentSelectionMode = m_selectionMode;
	if (ImGui::Combo("Selection Mode", &currentSelectionMode, "Box Selection\0Iso Selection\0Marker\0Rasterizer\0Entity\0\0")) {
		m_selectionMode = static_cast<SelectionMode>(currentSelectionMode);

		switch (m_selectionMode) {
			case SelectionMode::BOXSELECTION: {
				m_map.setSelectionMode(Enums::SelectionMode::BOXSELECTION);
				return;
			}case SelectionMode::ISOSELECTION: {
				m_map.setSelectionMode(Enums::SelectionMode::ISOSELECTION);
				return;
			}case SelectionMode::MARKER: {
				m_map.setSelectionMode(Enums::SelectionMode::MARKER);
				return;
			}case SelectionMode::RASTERIZER: {
				m_map.setSelectionMode(Enums::SelectionMode::RASTERIZER);
				return;
			} case SelectionMode::ENTITY: default :{
				m_map.setSelectionMode(Enums::SelectionMode::NONE);
				return;
			}
		}
	}

	if (m_selectionMode == SelectionMode::ISOSELECTION) {
		ImGui::Checkbox("Discrete Selection", &m_map.discreteSelection());
	}

	if (m_autoRedraw) {
		if (ImGui::Button("Redraw Map")) {
			m_redrawMap = true;
		}
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool Game::isValid(const int row, const int column) const {
	return m_map.isValid(row, column);
}

void Game::drawIsometricRect(float posX, float posY, Vector4f sizeOffset, Vector4f color) {
	std::array<Vector2f, 4> fPoints;
	fPoints[0] = Vector2f(sizeOffset[2], sizeOffset[3]);
	fPoints[1] = Vector2f(sizeOffset[2], sizeOffset[3] + sizeOffset[1]);
	fPoints[2] = Vector2f(sizeOffset[2] + sizeOffset[0], sizeOffset[3] + sizeOffset[1]);
	fPoints[3] = Vector2f(sizeOffset[2] + sizeOffset[0], sizeOffset[3]);

	Math::cartesianToIsometric(fPoints[0][0], fPoints[0][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[1][0], fPoints[1][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[2][0], fPoints[2][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[3][0], fPoints[3][1], 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_Camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&_Camera.getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	glVertex3f(posX + fPoints[0][0], posY - fPoints[0][1], 0.0f);
	glVertex3f(posX + fPoints[1][0], posY - fPoints[1][1], 0.0f);
	glVertex3f(posX + fPoints[2][0], posY - fPoints[2][1], 0.0f);
	glVertex3f(posX + fPoints[3][0], posY - fPoints[3][1], 0.0f);

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Game::drawIsometricRect(float posX, float posY, Vector4f bounds, Vector2f offset, Vector4f color) {
	std::array<Vector2f, 4> fPoints;

	fPoints[0] = Vector2f(bounds[0] + offset[0], bounds[1] + offset[1]);
	fPoints[1] = Vector2f(bounds[0] + offset[0], bounds[3] + offset[1]);
	fPoints[2] = Vector2f(bounds[2] + offset[0], bounds[3] + offset[1]);
	fPoints[3] = Vector2f(bounds[2] + offset[0], bounds[1] + offset[1]);

	Math::cartesianToIsometric(fPoints[0][0], fPoints[0][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[1][0], fPoints[1][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[2][0], fPoints[2][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[3][0], fPoints[3][1], 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_Camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&_Camera.getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	glVertex3f(posX + fPoints[0][0], posY - fPoints[0][1], 0.0f);
	glVertex3f(posX + fPoints[1][0], posY - fPoints[1][1], 0.0f);
	glVertex3f(posX + fPoints[2][0], posY - fPoints[2][1], 0.0f);
	glVertex3f(posX + fPoints[3][0], posY - fPoints[3][1], 0.0f);

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Game::drawIsometricRect(int posX, int posY, Vector4f color) {

	//if (!isValid(posX, posY)) return;

	float rowMin = posX - 0.5f;
	float rowMax = posX + 0.5f;
	float colMin = posY - 1.5f;
	float colMax = posY - 0.5f;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	float cartX = rowMax;
	float cartY = colMax;
	Math::cartesianToIsometric(cartX, cartY, m_map.getCellWidth(), m_map.getCellHeight());
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	cartX = rowMin;
	cartY = colMax;
	Math::cartesianToIsometric(cartX, cartY, m_map.getCellWidth(), m_map.getCellHeight());
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	cartX = rowMin;
	cartY = colMin;
	Math::cartesianToIsometric(cartX, cartY, m_map.getCellWidth(), m_map.getCellHeight());
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	cartX = rowMax;
	cartY = colMin;
	Math::cartesianToIsometric(cartX, cartY, m_map.getCellWidth(), m_map.getCellHeight());
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Game::drawClickBox(float posX, float posY, float width, float height) {

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_Camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&_Camera.getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);

	posX = posX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor);
	posY = posY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor);

	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(posX, posY, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(posX, (posY + height * ZoomFactor), 0.0f);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(posX + width * ZoomFactor, (posY + height * ZoomFactor), 0.0f);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(posX + width * ZoomFactor, posY, 0.0f);
	glEnd();

}

void Game::DrawIsometricRect(float posX, float posY, const Vector4f& sizeOffset, const Vector4f& color) {
	std::array<Vector2f, 4> fPoints;
	fPoints[0] = Vector2f(sizeOffset[2], sizeOffset[3]);
	fPoints[1] = Vector2f(sizeOffset[2], sizeOffset[3] + sizeOffset[1]);
	fPoints[2] = Vector2f(sizeOffset[2] + sizeOffset[0], sizeOffset[3] + sizeOffset[1]);
	fPoints[3] = Vector2f(sizeOffset[2] + sizeOffset[0], sizeOffset[3]);

	Math::cartesianToIsometric(fPoints[0][0], fPoints[0][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[1][0], fPoints[1][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[2][0], fPoints[2][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[3][0], fPoints[3][1], 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_Camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&_Camera.getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	posX = posX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor);
	posY = posY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor);

	glVertex3f(posX + fPoints[0][0] * ZoomFactor, posY - fPoints[0][1] * ZoomFactor, 0.0f);
	glVertex3f(posX + fPoints[1][0] * ZoomFactor, posY - fPoints[1][1] * ZoomFactor, 0.0f);
	glVertex3f(posX + fPoints[2][0] * ZoomFactor, posY - fPoints[2][1] * ZoomFactor, 0.0f);
	glVertex3f(posX + fPoints[3][0] * ZoomFactor, posY - fPoints[3][1] * ZoomFactor, 0.0f);

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Game::DrawIsometricRect(float posX, float posY, const Vector4f& bounds, const Vector2f& offset, const Vector4f& color) {
	std::array<Vector2f, 4> fPoints;

	fPoints[0] = Vector2f(bounds[0] + offset[0], bounds[1] + offset[1]);
	fPoints[1] = Vector2f(bounds[0] + offset[0], bounds[3] + offset[1]);
	fPoints[2] = Vector2f(bounds[2] + offset[0], bounds[3] + offset[1]);
	fPoints[3] = Vector2f(bounds[2] + offset[0], bounds[1] + offset[1]);

	Math::cartesianToIsometric(fPoints[0][0], fPoints[0][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[1][0], fPoints[1][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[2][0], fPoints[2][1], 1.0f, 1.0f);
	Math::cartesianToIsometric(fPoints[3][0], fPoints[3][1], 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_Camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&_Camera.getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	glVertex3f(posX + fPoints[0][0], posY - fPoints[0][1], 0.0f);
	glVertex3f(posX + fPoints[1][0], posY - fPoints[1][1], 0.0f);
	glVertex3f(posX + fPoints[2][0], posY - fPoints[2][1], 0.0f);
	glVertex3f(posX + fPoints[3][0], posY - fPoints[3][1], 0.0f);

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Game::DrawIsometricRect(int posX, int posY, const Vector4f& color) {

	//if (!isValid(posX, posY)) return;

	float rowMin = posX - 0.5f;
	float rowMax = posX + 0.5f;
	float colMin = posY - 1.5f;
	float colMax = posY - 0.5f;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_Camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&_Camera.getViewMatrix()[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], color[3]);

	float cartX = rowMax;
	float cartY = colMax;
	Math::cartesianToIsometric(cartX, cartY, CELL_WIDTH, CELL_HEIGHT);
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	cartX = rowMin;
	cartY = colMax;
	Math::cartesianToIsometric(cartX, cartY, CELL_WIDTH, CELL_HEIGHT);
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	cartX = rowMin;
	cartY = colMin;
	Math::cartesianToIsometric(cartX, cartY, CELL_WIDTH, CELL_HEIGHT);
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	cartX = rowMax;
	cartY = colMin;
	Math::cartesianToIsometric(cartX, cartY, CELL_WIDTH, CELL_HEIGHT);
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Game::DrawIsometricLine(const Vector2f& start, const Vector2f& end, const Vector2f& offset, const Vector4f& color) {
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&_Camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&_Camera.getViewMatrix()[0][0]);

	glBegin(GL_LINES);
	glColor4f(color[0], color[1], color[2], color[3]);

	float cartX = start[0] + (offset[0] - offset[1]);
	float cartY = -start[1] + 0.5f * (offset[0] + offset[1]) ;
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);

	
	cartX = end[0] + (offset[0] - offset[1]);
	cartY = -end[1] + 0.5f * (offset[0] + offset[1]);
	glVertex3f(cartX * ZoomFactor + (_Camera.getPositionX() + FocusPointX) * (1.0f - ZoomFactor), -cartY * ZoomFactor + (_Camera.getPositionY() + FocusPointY) * (1.0f - ZoomFactor), 0.0f);
	glEnd();

}