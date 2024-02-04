#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <engine/Batchrenderer.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "DebugRenderer.h"
#include "SceneManager.h"

Game::Game(StateMachine& machine) : State(machine, States::GAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	//Mouse::instance().attach(Application::GetWindow());

	SceneManager::Get().loadSettings("res/default_settings.json");
	SceneManager::Get().getScene("scene").loadScene(SceneManager::Get().getCurrentSceneFile());

	Scene scene = SceneManager::Get().getScene("scene");
	m_camera = Camera(scene.getCamera());


	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	glGenBuffers(1, &BuiltInShader::lightUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::lightUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBuffer) * 20, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, BuiltInShader::lightUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	
	glGenBuffers(1, &BuiltInShader::materialUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::materialUbo);
	glBufferData(GL_UNIFORM_BUFFER, 56, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, BuiltInShader::materialUbo, 0, 56);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	Material::SetTextures(scene.getTextures());
	Material::SetMaterials(scene.getMaterials());
	Light::SetLights(scene.getLights());
	m_meshes = scene.getMeshes();

	const BoundingBox& box = m_meshes[0]->getAABB();
	m_meshes[0]->getAABB().inset(Vector3f(-(2.0f * box.min[0] + 2.5f), 0.6f, 1.3f), Vector3f(2.0f * box.max[0] - 2.5f, 0.1f, 0.5f));

	m_objSequence = scene.getObjSequence();
	m_entitiesAfterClear = scene.getEntitiesAfterClear();
	m_player = scene.getPlayer();
	m_player->setPosition(0.0f, 0.0f, 0.0f);
	m_player->start();
	m_entities = scene.getEntities();

	
	HUD.setHP(m_player->hp * 10);
	HUD.setInHandAmmo(m_player->inHandAmmo);
	HUD.setTotalAmmo(m_player->totalAmmo);
	HUD.setStatus(aw::ONGOING);
	HUD.setShaderProgram(Globals::shaderManager.getAssetPointer("hud")->m_program);

	gameStatus = aw::ONGOING;

	Bullet::Init(m_meshes[5]);
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {
	m_player->fixedUpdate(m_fdt);

	for (auto entity : m_entitiesAfterClear)
		entity->fixedUpdate(m_fdt);

	for (auto entity : m_entities)
		entity->fixedUpdate(m_fdt);
}

void Game::update() {
	m_player->update(m_dt);

	for (auto entity : m_entitiesAfterClear)
		entity->update(m_dt);

	for (auto entity : m_entities)
		entity->update(m_fdt);

	//Vector3f lightPos = Light::GetLights()[0].getWorldPosition();
	//std::cout << "Position: " << lightPos[0] << "  " << lightPos[1] << "  " << lightPos[2] << std::endl;

	bool isWin = Ant::GetCount() == 0;

	if (Globals::clock.getElapsedTimeSec() - m_player->timeSinceDamage > 0.25f){
		HUD.setIsHurting(false);
	}

	if (isWin){
		gameStatus = aw::WIN;
		HUD.setStatus(aw::WIN);
		m_player->killSound();
	}else if (m_player->isDead() || (m_player->inHandAmmo <= 0 && m_player->totalAmmo <= 0)){
		gameStatus = aw::LOSE;
		HUD.setStatus(aw::LOSE);
		m_player->killSound();
	}

	deleteEntities();
}

void Game::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DebugRenderer::Get().SetView(&m_player->camera);

	auto shader = Globals::shaderManager.getAssetPointer("antware");
	shader->use();
	shader->loadMatrix("u_projection", m_player->camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_player->camera.getViewMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_player->camera.getViewMatrix() * Matrix4f::IDENTITY));

	for (auto entity : m_entities) {
		shader->loadMatrix("u_model", entity->getWorldTransformation());
		entity->draw();
		entity->OnRenderOBB();
	}

	for (auto& bullet : m_player->getBullets()) {
		shader->loadMatrix("u_model", bullet.getTransformationSOP());
		bullet.draw();
	}

	glClear(GL_DEPTH_BUFFER_BIT);

	m_player->OnRenderOBB({ 1.0f, 1.0f, 0.0f, 1.0f });
	for (auto entity : m_entitiesAfterClear) {
		shader->loadMatrix("u_model", entity->getWorldTransformation());
		entity->draw();
		entity->OnRenderOBB({0.0f, 0.0f, 1.0f, 1.0f});
	}

	shader->unuse();
	DebugRenderer::Get().drawBuffer();

	HUD.draw();

	if (m_drawUi)
		renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {

}

void Game::resize(int deltaW, int deltaH) {
	//m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	//m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
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
	ImGui::Checkbox("Draw OBB", &DebugRenderer::Enabled());
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::deleteEntities() {
	/*m_entities.erase(
		std::remove_if(m_entities.begin(), m_entities.end(),
			[](Entity *entity) -> bool{
				if (entity->isMarkForDelete()) {
					delete entity;
					return true;
				}
				return false; 
	
			}), 
		m_entities.end()
	);*/

	std::vector<Entity*>::iterator it = m_entities.begin();
	while (it != m_entities.end()) {

		if ((*it)->isMarkForDelete()) {
			delete (*it);
			it = m_entities.erase(it);;
		}
		else ++it;
	}
}