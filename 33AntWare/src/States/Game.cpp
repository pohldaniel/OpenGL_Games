#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <engine/Batchrenderer.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"

Game::Game(StateMachine& machine) : State(machine, States::GAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	//Mouse::instance().attach(Application::GetWindow());

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 5000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.5f, 0.0f), Vector3f(0.0f, 0.5f, 0.0f) - Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_camera.setPosition(10.0f, 0.5f, 0.0f);
	m_camera.setRotationSpeed(1.0f);
	m_camera.setMovingSpeed(1.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	glGenBuffers(1, &Globals::lightUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, Globals::lightUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightStruct) * 20, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, Globals::lightBinding, Globals::lightUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	auto shader = Globals::shaderManager.getAssetPointer("antware");
	aw::Material::setUniformsLocation(shader->m_program);

	m_model.loadModel("res/models/Ant.glb", false, false, false);
	Material& material = Material::GetMaterials()[m_model.getMeshes()[0]->getMaterialIndex()];

	m_gun.loadModel("res/models/Gun.glb", false, false, false);
	Material::GetMaterials()[m_gun.getMeshes()[0]->getMaterialIndex()].textures[0].loadFromFile("res/textures/Gun.png", true);
	m_gun.initShader();

	m_objSequence.loadSequence("res/animations/ant_walkcycle");
	m_objSequence.addMesh(m_model.getMeshes()[0]->getVertexBuffer(), m_model.getMeshes()[0]->getIndexBuffer());
	m_objSequence.loadSequenceGpu();

	m_muzzleMesh = std::make_shared<aw::Mesh>("res/models/MuzzleQuad.glb", nullptr, false);
	m_bulletMesh = std::make_shared<aw::Mesh>("res/models/Bullet.glb", "res/textures/Bullet.png", false);
	m_gunMesh = std::make_shared<aw::Mesh>("res/models/Gun.glb", "res/textures/Gun.png", false);
	m_handsMesh = std::make_shared<aw::Mesh>("res/models/Hands.glb", "res/textures/Hands.png", false);
	m_glovesMesh = std::make_shared<aw::Mesh>("res/models/Gloves.glb", "res/textures/Gloves.png", false);
	m_cpuMesh = std::make_shared<aw::Mesh>("res/models/CPU.glb", "res/textures/CPU.jpg", false);
	m_antMesh = std::make_shared<aw::Mesh>("res/models/Ant.glb", "res/textures/Ant.png", false);
	m_platformMesh = std::make_shared<aw::Mesh>("res/models/Platform.glb", nullptr, false);
	m_cubeMesh = std::make_shared<aw::Mesh>("res/models/PlayerCube.glb", nullptr, false);

	m_meshes.push_back(m_muzzleMesh);
	m_meshes.push_back(m_bulletMesh);
	m_meshes.push_back(m_gunMesh);
	m_meshes.push_back(m_handsMesh);
	m_meshes.push_back(m_glovesMesh);
	m_meshes.push_back(m_cpuMesh);
	m_meshes.push_back(m_antMesh);
	m_meshes.push_back(m_platformMesh);
	m_meshes.push_back(m_cubeMesh);
	aw::Mesh::constructVAO(m_meshes);

	m_player = new Player(m_camera,
						  m_cubeMesh,
						  aw::Material(),
						  m_bulletMesh,
						  glm::vec2(-51.5f, -51.5f),
						  glm::vec2(51.5f, 51.5f),
						  nullptr);
	m_player->transform = aw::Transform(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	
	m_muzzleGO = new aw::StaticGO(m_muzzleMesh, aw::Material(), m_player);
	m_gunGO = new aw::StaticGO(m_gunMesh, aw::Material(), m_player);
	m_handsGO = new aw::StaticGO(m_handsMesh, aw::Material(), m_player);
	m_glovesGO = new aw::StaticGO(m_glovesMesh, aw::Material(), m_player);
	m_cpuGO = new aw::StaticGO(m_cpuMesh, aw::Material(), nullptr);
	m_platformGO = new aw::StaticGO(m_platformMesh, aw::Material(), nullptr);

	m_player->start();

	m_ant1 = new Ant(m_objSequence, m_antMesh, aw::Material(), nullptr, m_player);
	m_ant1->transform = aw::Transform(glm::vec3(-21.3863f, -0.978558f, -1.92476f), glm::vec3(0.0f, 262.062f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	m_ant1->isStatic = false;
	m_ant1->rigidbody = aw::Rigidbody();
	m_ant1->start();

	m_ant2 = new Ant(m_objSequence, m_antMesh, aw::Material(), nullptr, m_player);
	m_ant2->transform = aw::Transform(glm::vec3(-23.6894f, -0.978558f, 34.7609f), glm::vec3(0.0f, -11.0968f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	m_ant2->isStatic = false;
	m_ant2->rigidbody = aw::Rigidbody();
	m_ant2->start();

	m_ant3 = new Ant(m_objSequence, m_antMesh, aw::Material(), nullptr, m_player);
	m_ant3->transform = aw::Transform(glm::vec3(23.6894f, -0.978558f, 34.1029f), glm::vec3(0.0f, 18.5357f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	m_ant3->isStatic = false;
	m_ant3->rigidbody = aw::Rigidbody();
	m_ant3->start();

	m_ant4 = new Ant(m_objSequence, m_antMesh, aw::Material(), nullptr, m_player);
	m_ant4->transform = aw::Transform(glm::vec3(33.3955f, -0.978558f, 16.0068f), glm::vec3(0.0f, 86.8875f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	m_ant4->isStatic = false;
	m_ant4->rigidbody = aw::Rigidbody();
	m_ant4->start();

	m_ant5 = new Ant(m_objSequence, m_antMesh, aw::Material(), nullptr, m_player);
	m_ant5->transform = aw::Transform(glm::vec3(33.0665f, -0.978558f, -18.3758f), glm::vec3(0.0f, 110.727f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	m_ant5->isStatic = false;
	m_ant5->rigidbody = aw::Rigidbody();
	m_ant5->start();

	m_ant6 = new Ant(m_objSequence, m_antMesh, aw::Material(), nullptr, m_player);
	m_ant6->transform = aw::Transform(glm::vec3(16.78f, -0.978558f, -35.4848f), glm::vec3(0.0f, 169.316f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	m_ant6->isStatic = false;
	m_ant6->rigidbody = aw::Rigidbody();
	m_ant6->start();

	m_ant7 = new Ant(m_objSequence, m_antMesh, aw::Material(), nullptr, m_player);
	m_ant7->transform = aw::Transform(glm::vec3(-17.9316f, -0.978558f, -35.1558f), glm::vec3(0.0f, 193.526f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	m_ant7->isStatic = false;
	m_ant7->rigidbody = aw::Rigidbody();
	m_ant7->start();

	m_ant8 = new Ant(m_objSequence, m_antMesh, aw::Material(), nullptr, m_player);
	m_ant8->transform = aw::Transform(glm::vec3(-33.889f, -0.978558f, -20.1854f), glm::vec3(0.0f, 238.843f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	m_ant8->isStatic = false;
	m_ant8->rigidbody = aw::Rigidbody();
	m_ant8->start();

	m_ant9 = new Ant(m_objSequence, m_antMesh, aw::Material(), nullptr, m_player);
	m_ant9->transform = aw::Transform(glm::vec3(-35.6987f, -0.978558f, 14.5262f), glm::vec3(0.0f, 272.3f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	m_ant9->isStatic = false;
	m_ant9->rigidbody = aw::Rigidbody();
	m_ant9->start();

	m_gameObjects.push_back(m_player);
	m_gameObjects.push_back(m_muzzleGO);
	m_gameObjects.push_back(m_gunGO);
	m_gameObjects.push_back(m_handsGO);
	m_gameObjects.push_back(m_glovesGO);
	m_gameObjects.push_back(m_cpuGO);
	m_gameObjects.push_back(m_platformGO);

	m_ants.push_back(m_ant1);
	m_ants.push_back(m_ant2);
	m_ants.push_back(m_ant3);
	m_ants.push_back(m_ant4);
	m_ants.push_back(m_ant5);
	m_ants.push_back(m_ant6);
	m_ants.push_back(m_ant7);
	m_ants.push_back(m_ant8);
	m_ants.push_back(m_ant9);

	HUD.setHP(m_player->hp * 10);
	HUD.setInHandAmmo(m_player->inHandAmmo);
	HUD.setTotalAmmo(m_player->totalAmmo);
	HUD.setStatus(aw::ONGOING);
	HUD.setShaderProgram(Globals::shaderManager.getAssetPointer("hud")->m_program);

	gameStatus = aw::ONGOING;

	m_muzzleE = new Entity(m_muzzleMesh, aw::Material());
	m_muzzleE->m_isStatic = true;
	m_gunE = new Entity(m_gunMesh, aw::Material());
	m_gunE->m_isStatic = true;
	m_handsE = new Entity(m_handsMesh, aw::Material());
	m_handsE->m_isStatic = true;
	m_glovesE = new Entity(m_glovesMesh, aw::Material());
	m_glovesE->m_isStatic = true;

	playerNew = new PlayerNew(m_camera, m_cubeMesh, aw::Material(), Vector2f(-51.5f, -51.5f), Vector2f(51.5f, 51.5f));
	playerNew->addChild2(m_muzzleE);
	playerNew->addChild2(m_gunE);
	playerNew->addChild2(m_handsE);
	playerNew->addChild2(m_glovesE);
	playerNew->start();

	m_entities.push_back(playerNew);
	m_entities.push_back(m_muzzleE);
	m_entities.push_back(m_gunE);
	m_entities.push_back(m_handsE);
	m_entities.push_back(m_glovesE);

	playerNew->updateSelfAndChild();
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {

	for (auto gameObject : m_gameObjects)
		gameObject->fixedUpdate(m_fdt);

	for (auto entity : m_entities)
		entity->fixedUpdate(m_fdt);

	//for (auto ant : m_ants)
	//	ant->fixedUpdate(m_fdt);
}

void Game::update() {

	for (auto gameObject : m_gameObjects)
		gameObject->update(m_dt);

	for (auto entity : m_entities)
		entity->update(m_dt);

	//for (auto ant : m_ants)
	//	ant->update(m_dt);

	/*auto player = m_player;
	auto bullets = player->bullets;
	auto antsSize = m_ants.size();
	auto bulletsSize = player->bullets.size();
	bool isWin = true;
	for (unsigned i = 0; i < antsSize; ++i){

		if (m_ants[i]->getClass() == 3){

			isWin = false;
			auto ant = ((Ant *)(m_ants[i]));
			if (ant->timeToDestroy()){
				destroyGameObject(i);
				--i;
				--antsSize;
			}

			for (unsigned j = 0; j < bulletsSize; ++j){
				if (ant->aabb.isColliding(bullets[j].transform.getPosition()))
				{
					ant->damage(1);
					player->destroyBullet(j);
					--j;
					--bulletsSize;
				}
			}

			if (ant->aabb.isColliding(m_player->aabb) && Globals::clock.getElapsedTimeSec() > 2.0f && ant->timeSinceDealtDamage.getElapsedTimeSec() >= 1.0f){
				ant->timeSinceDealtDamage.reset();
				player->damage(1.0f);
				HUD.setIsHurting(true);
				player->timeSinceDamage = Globals::clock.getElapsedTimeSec();
				HUD.setHP(player->hp * 10);
			}
		}
	}

	if (Globals::clock.getElapsedTimeSec() - player->timeSinceDamage > 0.25f){
		HUD.setIsHurting(false);
	}

	if (isWin){
		gameStatus = aw::WIN;
		HUD.setStatus(aw::WIN);
		player->killSound();
	}else if (player->isDead() || (player->inHandAmmo <= 0 && player->totalAmmo <= 0)){
		gameStatus = aw::LOSE;
		HUD.setStatus(aw::LOSE);
		player->killSound();
	}*/
}

void Game::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("antware");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * Matrix4f::IDENTITY));
	
	shader->loadMatrix("u_model", (const float*)glm::value_ptr(m_cpuGO->applyTransform()));
	m_cpuGO->draw();
	shader->loadMatrix("u_model", (const float*)glm::value_ptr(m_platformGO->applyTransform()));
	m_platformGO->draw();

	Globals::textureManager.get("ant").bind();

	for (auto ant : m_ants) {
		shader->loadMatrix("u_model", (const float*)glm::value_ptr(ant->applyTransform()));
		ant->draw();
	}
	Globals::textureManager.get("ant").unbind();

	//glDisable(GL_DEPTH_TEST);
	/*shader->loadMatrix("u_model", (const float*)glm::value_ptr(m_muzzleGO->applyTransform()));
	m_muzzleGO->draw();
	shader->loadMatrix("u_model", (const float*)glm::value_ptr(m_gunGO->applyTransform()));
	m_gunGO->draw();
	shader->loadMatrix("u_model", (const float*)glm::value_ptr(m_handsGO->applyTransform()));
	m_handsGO->draw();
	shader->loadMatrix("u_model", (const float*)glm::value_ptr(m_glovesGO->applyTransform()));
	m_glovesGO->draw();*/
	//glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	for (auto entity : m_entities) {
		shader->loadMatrix("u_model", entity->getTransformation());
		entity->draw(m_camera);
	}

	shader->unuse();

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
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
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
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::destroyGameObject(int index) {
	delete m_ants[index];
	m_ants.erase(m_ants.begin() + index);
}