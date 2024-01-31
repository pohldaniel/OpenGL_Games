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

	glGenBuffers(1, &Globals::lightUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, Globals::lightUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightStruct) * 20, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, Globals::lightUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	
	glGenBuffers(1, &BuiltInShader::materialUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::materialUbo);
	glBufferData(GL_UNIFORM_BUFFER, 56, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, BuiltInShader::materialUbo, 0, 56);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	

	auto shader = Globals::shaderManager.getAssetPointer("antware");
	/*Material::AddTexture("res/textures/Ant.png");
	Material::AddTexture("res/textures/Gun.png");
	Material::AddTexture("res/textures/Hands.png");
	Material::AddTexture("res/textures/Gloves.png");
	Material::AddTexture("res/textures/CPU.jpg");
	Material::AddTexture("res/textures/Bullet.png");
	Material::AddTexture("res/textures/transparent.png");
	Material::AddTexture("res/textures/flash.png");*/

	//Material::AddMaterial({ {0.1f, 0.1f, 0.1f, 1.0f}, {0.6f, 0.6f, 0.6f, 1.0f}, {0.3f, 0.3f, 0.3f, 1.0f}, 8.0f, 1.0f });

	Material::SetTextures(scene.getTextures());
	Material::SetMaterials(scene.getMaterials());
	m_meshes = scene.getMeshes();

	const BoundingBox& box = m_meshes[0]->getAABB();
	m_meshes[0]->getAABB().inset(Vector3f(-(2.0f * box.min[0] + 2.5f), 0.6f, 1.3f), Vector3f(2.0f * box.max[0] - 2.5f, 0.1f, 0.5f));

	m_objSequence = scene.getObjSequence();


	
	

	m_muzzleE = new Entity(m_meshes[7]);
	m_muzzleE->m_isStatic = true;
	m_gunE = new Entity(m_meshes[2]);
	m_gunE->m_isStatic = true;
	m_handsE = new Entity(m_meshes[3]);
	m_handsE->m_isStatic = true;
	m_glovesE = new Entity(m_meshes[4]);
	m_glovesE->m_isStatic = true;
	m_cpuE = new Entity(m_meshes[1]);
	m_cpuE->m_isStatic = true;
	m_platformE = new Entity(m_meshes[8]);
	m_platformE->m_isStatic = true;

	m_player = new Player(m_camera, m_meshes[6], Vector2f(-51.5f, -51.5f), Vector2f(51.5f, 51.5f));

	m_player->setPosition(0.0f, 0.0f, 5.0f);
	m_player->addChild(m_muzzleE, true);
	m_player->addChild(m_gunE, true);
	m_player->addChild(m_handsE, false);
	m_player->addChild(m_glovesE, true);
	m_player->start();

	//m_entities.push_back(m_player);
	m_entities.push_back(m_muzzleE);
	m_entities.push_back(m_gunE);
	m_entities.push_back(m_handsE);
	m_entities.push_back(m_glovesE);
	//m_entities.push_back(m_cpuE);
	//m_entities.push_back(m_platformE);

	HUD.setHP(m_player->hp * 10);
	HUD.setInHandAmmo(m_player->inHandAmmo);
	HUD.setTotalAmmo(m_player->totalAmmo);
	HUD.setStatus(aw::ONGOING);
	HUD.setShaderProgram(Globals::shaderManager.getAssetPointer("hud")->m_program);

	gameStatus = aw::ONGOING;

	Bullet::Init(m_meshes[5]);

	m_ant1 = new Ant(m_objSequence, m_meshes[0], m_player);
	m_ant1->setPosition(-21.3863f, -0.978558f, -1.92476f);
	m_ant1->setOrientation(0.0f, 262.062f, 0.0f);
	m_ant1->m_isStatic = false;
	m_ant1->rigidbody = Rigidbody();
	m_ant1->start();

	m_ant2 = new Ant(m_objSequence, m_meshes[0], m_player);
	m_ant2->setPosition(-23.6894f, -0.978558f, 34.7609f);
	m_ant2->setOrientation(0.0f, -11.0968f, 0.0f);
	m_ant2->m_isStatic = false;
	m_ant2->rigidbody = Rigidbody();
	m_ant2->start();

	m_ant3 = new Ant(m_objSequence, m_meshes[0], m_player);
	m_ant3->setPosition(23.6894f, -0.978558f, 34.1029f);
	m_ant3->setOrientation(0.0f, 18.5357f, 0.0f);
	m_ant3->m_isStatic = false;
	m_ant3->rigidbody = Rigidbody();
	m_ant3->start();

	m_ant4 = new Ant(m_objSequence, m_meshes[0], m_player);
	m_ant4->setPosition(33.3955f, -0.978558f, 16.0068f);
	m_ant4->setOrientation(0.0f, 86.8875f, 0.0f);
	m_ant4->m_isStatic = false;
	m_ant4->rigidbody = Rigidbody();
	m_ant4->start();

	m_ant5 = new Ant(m_objSequence, m_meshes[0], m_player);
	m_ant5->setPosition(33.0665f, -0.978558f, -18.3758f);
	m_ant5->setOrientation(0.0f, 110.727f, 0.0f);
	m_ant5->m_isStatic = false;
	m_ant5->rigidbody = Rigidbody();
	m_ant5->start();

	m_ant6 = new Ant(m_objSequence, m_meshes[0], m_player);
	m_ant6->setPosition(16.78f, -0.978558f, -35.4848f);
	m_ant6->setOrientation(0.0f, 169.316f, 0.0f);
	m_ant6->m_isStatic = false;
	m_ant6->rigidbody = Rigidbody();
	m_ant6->start();

	m_ant7 = new Ant(m_objSequence, m_meshes[0], m_player);
	m_ant7->setPosition(-17.9316f, -0.978558f, -35.1558f);
	m_ant7->setOrientation(0.0f, 193.526f, 0.0f);
	m_ant7->m_isStatic = false;
	m_ant7->rigidbody = Rigidbody();
	m_ant7->start();

	m_ant8 = new Ant(m_objSequence, m_meshes[0],  m_player);
	m_ant8->setPosition(-33.889f, -0.978558f, -20.1854f);
	m_ant8->setOrientation(0.0f, 238.843f, 0.0f);
	m_ant8->m_isStatic = false;
	m_ant8->rigidbody = Rigidbody();
	m_ant8->start();

	m_ant9 = new Ant(m_objSequence, m_meshes[0], m_player);
	m_ant9->setPosition(-35.6987f, -0.978558f, 14.5262f);
	m_ant9->setOrientation(0.0f, 272.3f, 0.0f);
	m_ant9->m_isStatic = false;
	m_ant9->rigidbody = Rigidbody();
	m_ant9->start();

	m_ants.push_back(m_ant1);
	m_ants.push_back(m_ant2);
	m_ants.push_back(m_ant3);
	m_ants.push_back(m_ant4);
	m_ants.push_back(m_ant5);
	m_ants.push_back(m_ant6);
	m_ants.push_back(m_ant7);
	m_ants.push_back(m_ant8);
	m_ants.push_back(m_ant9);
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {
	m_player->fixedUpdate(m_fdt);

	for (auto entity : m_entities)
		entity->fixedUpdate(m_fdt);

	for (auto ant : m_ants)
		ant->fixedUpdate(m_fdt);
}

void Game::update() {
	m_player->update(m_dt);

	for (auto entity : m_entities)
		entity->update(m_dt);

	for (auto ant : m_ants)
		ant->update(m_dt);

	auto player = m_player;
	auto& bullets = player->bullets;
	auto antsSize = m_ants.size();
	auto bulletsSize = bullets.size();
	bool isWin = true;

	for (unsigned i = 0; i < antsSize; ++i) {

		isWin = false;
		auto ant = m_ants[i];
		if (ant->timeToDestroy()) {
			destroyAnt(i);
			--i;
			--antsSize;
		}

		for (unsigned j = 0; j < bulletsSize; ++j) {

			if (ant->getWorldBoundingBox().isColliding(bullets[j].getPosition())) {
				ant->damage(1);
				player->destroyBullet(j);
				--j;
				--bulletsSize;
			}
		}

		if (ant->getWorldBoundingBox().isColliding(m_player->getWorldBoundingBox()) && Globals::clock.getElapsedTimeSec() > 2.0f && ant->timeSinceDealtDamage.getElapsedTimeSec() >= 1.0f) {
			ant->timeSinceDealtDamage.reset();
			player->damage(1.0f);
			HUD.setIsHurting(true);
			player->timeSinceDamage = Globals::clock.getElapsedTimeSec();
			HUD.setHP(player->hp * 10);
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
	}
}

void Game::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DebugRenderer::Get().SetView(&m_camera);

	auto shader = Globals::shaderManager.getAssetPointer("antware");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * Matrix4f::IDENTITY));

	shader->loadMatrix("u_model", m_cpuE->getTransformation());
	m_cpuE->draw(m_camera);
	m_cpuE->OnRenderOBB();

	shader->loadMatrix("u_model", m_platformE->getTransformation());
	m_platformE->draw(m_camera);
	m_platformE->OnRenderOBB();

	for (auto ant : m_ants) {
		shader->loadMatrix("u_model", ant->getTransformation());
		ant->draw(m_camera);
		ant->OnRenderOBB();
	}

	for (auto& bullet : m_player->getBullets()) {
		shader->loadMatrix("u_model", bullet.getTransformationSOP());
		bullet.draw(m_camera);
	}

	glClear(GL_DEPTH_BUFFER_BIT);

	m_player->OnRenderOBB({ 1.0f, 1.0f, 0.0f, 1.0f });
	for (auto entity : m_entities) {
		shader->loadMatrix("u_model", entity->getTransformation());
		entity->draw(m_camera);
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
	ImGui::Checkbox("Draw OBB", &DebugRenderer::Enabled());
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::destroyAnt(int index) {
	delete m_ants[index];
	m_ants.erase(m_ants.begin() + index);
}