#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <engine/Batchrenderer.h>
#include <engine/DebugRenderer.h>

#include <States/MainMenu.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "SceneManager.h"
#include "HUD.h"

Game::Game(StateMachine& machine) : State(machine, States::GAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow());

	HUD::Get().init();

	Scene scene = SceneManager::Get().getScene("scene");
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	auto shader = Globals::shaderManager.getAssetPointer("antware");
	shader->use();
	shader->loadUnsignedInt("u_num_lights", Light::GetLights().size());
	shader->unuse();

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

	m_entitiesAfterClear = scene.getEntitiesAfterClear();
	m_player = scene.getPlayer();
	m_camera = &m_player->camera;
	m_camera->setOffsetDistance(m_offsetDistance);
	m_entities = scene.getEntities();
	m_skybox = scene.getSkybox();

	HUD::Get().setHP(m_player->hp * 10);
	HUD::Get().setInHandAmmo(m_player->inHandAmmo);
	HUD::Get().setTotalAmmo(m_player->totalAmmo);

	std::for_each(m_entitiesAfterClear.begin(), m_entitiesAfterClear.end(), std::mem_fn(&Entity::start));
	std::for_each(scene.getEntities().begin(), scene.getEntities().end(), std::mem_fn(&Entity::start));
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	SceneManager::Get().getScene("scene").unloadScene();
	HUD::Get().setWin(false);
	HUD::Get().setLoose(false);
}

void Game::fixedUpdate() {
	if(m_gameEnd)
		return;

	m_player->fixedUpdate(m_fdt);

	for (auto&& entity : m_entitiesAfterClear)
		entity->fixedUpdate(m_fdt);

	for (auto&& entity : m_entities)
		entity->fixedUpdate(m_fdt);
}

void Game::update() {

	if(m_gameEnd){
		m_gameStateTimer += 1.15f * m_dt;

		if (m_gameStateTimer >= 5.0f) {
			Mouse::instance().detach();
			m_isRunning = false;
			m_machine.addStateAtBottom(new MainMenu(m_machine));
		}
		return;
	}

	m_player->update(m_dt);

	for (auto&& entity : m_entitiesAfterClear)
		entity->update(m_dt);

	for (auto&& entity : m_entities)
		entity->update(m_dt);

	for (auto&& light : Light::GetLights())
			light->update(m_dt);
		
	bool isWin = Ant::GetCount() == 0;

	if (Globals::clock.getElapsedTimeSec() - m_player->timeSinceDamage > 0.25f) {
		HUD::Get().setIsHurting(false);
	}

	if (isWin) {
		m_gameEnd = true;
		HUD::Get().setWin(m_gameEnd);
		m_player->stopFootstepsSound();
	}else if (m_player->isDead() || (m_player->inHandAmmo <= 0 && m_player->totalAmmo <= 0)) {
		m_gameEnd = true;
		HUD::Get().setLoose(m_gameEnd);
		m_player->stopFootstepsSound();
	}

	deleteEntities();
	Light::UpdateLightUbo(BuiltInShader::lightUbo);	

	if (Keyboard::instance().keyPressed(Keyboard::KEY_T)) {
		Mouse::instance().detach();
	}
}

void Game::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DebugRenderer::Get().SetView(m_camera);

	auto shader = Globals::shaderManager.getAssetPointer("antware");
	shader->use();
	shader->loadMatrix("u_projection", m_camera->getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera->getViewMatrix());
	shader->loadVector("u_campos", m_camera->getPosition());

	for (auto&& entity : m_entities) {
		shader->loadMatrix("u_model", entity->getWorldTransformation());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(entity->GetTransformation()));
		entity->draw();

		if (m_drawOBB)
			entity->OnRenderOBB();
		if (m_drawAABB)
			entity->OnRenderAABB();
	}

	for (auto&& bullet : m_player->getBullets()) {
		shader->loadMatrix("u_model", bullet.getTransformationSOP());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(bullet.GetTransformation()));
		bullet.draw();
	}
	
	if(m_skybox)
		m_skybox->draw(*m_camera);

	glClear(GL_DEPTH_BUFFER_BIT);
	shader->use();

	if(m_drawOBB)
		m_player->OnRenderOBB({1.0f, 1.0f, 0.0f, 1.0f});
	if(m_drawAABB)
		m_player->OnRenderAABB({1.0f, 1.0f, 0.0f, 1.0f});

	for (auto&& entity : m_entitiesAfterClear) {
		shader->loadMatrix("u_model", entity->getWorldTransformation());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(entity->GetTransformation()));
		entity->draw();

		if (m_drawOBB)
			entity->OnRenderOBB({0.0f, 0.0f, 1.0f, 1.0f});

		if (m_drawAABB)
			entity->OnRenderAABB({0.0f, 0.0f, 1.0f, 1.0f});
	}

	shader->unuse();
	DebugRenderer::Get().drawBuffer();

	HUD::Get().draw(*m_camera);

	if (m_drawUi)
		renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u && m_drawUi) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u && m_drawUi) {
		Mouse::instance().detach();
	}
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {
	
	if (m_drawUi) {
		if (event.direction == 1u) {
			m_offsetDistance += 2.0f;
			m_offsetDistance = std::max(0.0f, std::min(m_offsetDistance, 150.0f));
			m_camera->setOffsetDistance(m_offsetDistance);
		}

		if (event.direction == 0u) {
			m_offsetDistance -= 2.0f;
			m_offsetDistance = std::max(0.0f, std::min(m_offsetDistance, 150.0f));
			m_camera->setOffsetDistance(m_offsetDistance);
		}
	}
}
void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU && !m_keyDown) {
		m_drawUi = !m_drawUi;
		Globals::soundManager.get("easter").replayChannel(0u);

		if (!m_drawUi) {
			DebugRenderer::Get().setEnable(false);
			Mouse::instance().attach(Application::GetWindow());
			m_offsetDistance = 0.0f;
			m_camera->setOffsetDistance(m_offsetDistance);
		}else {
			DebugRenderer::Get().setEnable(m_drawOBB | m_drawAABB);
			Mouse::instance().detach();
		}
		m_keyDown = true;
	}

	if (event.keyCode == VK_ESCAPE) {
		DebugRenderer::Get().setEnable(false);
		Mouse::instance().detach();
		m_isRunning = false;
		m_machine.addStateAtBottom(new MainMenu(m_machine));
	}
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU && m_keyDown) {
		m_keyDown = false;
	}
}

void Game::resize(int deltaW, int deltaH) {
	m_camera->setAspect(static_cast<float>(Application::Width) / static_cast<float>(Application::Height));
	HUD::Get().resize(deltaW, deltaH);
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
	if (ImGui::Checkbox("Draw OOB", &m_drawOBB)) {
		DebugRenderer::Get().setEnable(m_drawOBB | m_drawAABB);
	}

	if (ImGui::Checkbox("Draw AABB", &m_drawAABB)) {
		DebugRenderer::Get().setEnable(m_drawOBB | m_drawAABB);
	}

	ImGui::Text("Scroll Mouse");
	if (ImGui::SliderFloat("Camera Offset", &m_offsetDistance, 0.0f, 150.0f)) {
		m_camera->setOffsetDistance(m_offsetDistance);
	}
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
			// eraseSelf -> calls destructor -> delete (*it) not neccessary
			(*it)->eraseSelf();
			//delete (*it);
			it = m_entities.erase(it);
		}
		else ++it;
	}
}