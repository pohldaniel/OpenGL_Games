#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <engine/DebugRenderer.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"


Game::Game(StateMachine& machine) : State(machine, States::GAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 5.0f), Vector3f(0.0f, 0.0f, 5.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(5.0f);

	//glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	vampire.loadModelAssimp("res/models/vampire/dancing_vampire.dae", true, false);
	vampire.m_meshes[0]->m_meshBones[0].initialPosition.translate(1.0f, 0.0f, 0.0f);
	vampire.m_meshes[0]->m_meshBones[0].initialRotation.rotate(Vector3f(0.0f, 1.0f, 0.0f), 180.0f);
	
	vampire.m_meshes[0]->m_meshBones[1].initialPosition.set(0.0f, 1.0f, 0.0f);
	vampire.m_meshes[0]->m_meshBones[1].initialScale.scale(0.01f, 0.01f, 0.01f);

	vampire.m_meshes[0]->createBones();

	Animation* animation = new Animation();
	animation->loadAnimationAssimp("res/models/vampire/dancing_vampire.dae", "Hips", "vampire_dance");
	animation->setPositionOfTrack("Hips", 0.0f, 1.0f, 0.0f);
	animation->scaleTrack("Hips", 0.01f, 0.01f, 0.01f);
	
	vampire.m_meshes[0]->m_animationStates.push_back(std::shared_ptr<AnimationState>(new AnimationState(animation, vampire.m_meshes[0]->m_rootBone)));
	vampire.m_meshes[0]->m_animationStates.back()->SetLooped(true);
	vampire.m_meshes[0]->m_animationStates.back()->SetEnabled(m_playAnimation);
	

	beta.loadModelMdl("res/models/BetaLowpoly/Beta.mdl");
	beta.m_meshes[0]->m_meshBones[0].initialPosition.translate(-1.0f, 0.0f, 0.0f);
	beta.m_meshes[0]->m_meshBones[0].initialRotation.rotate(0.0f, 180.0f, 0.0f);
	beta.m_meshes[0]->createBones();

	animation = new Animation();
	//animation->loadAnimation("res/models/BetaLowpoly/Beta_Idle.ani");
	animation->loadAnimation("res/models/BetaLowpoly/Beta_Run.ani");

	beta.m_meshes[0]->m_animationStates.push_back(std::shared_ptr<AnimationState>(new AnimationState(animation, beta.m_meshes[0]->m_rootBone)));
	beta.m_meshes[0]->m_animationStates.back()->SetLooped(true);
	//beta.m_meshes[0]->m_animationStates.back()->SetEnabled(false);

	cowboy.loadModelAssimp("res/models/cowboy/cowboy.dae", true, false);
	cowboy.m_meshes[0]->m_meshBones[1].initialPosition.set(0.0f, 0.3f, 0.0f);
	cowboy.m_meshes[0]->m_meshBones[1].initialScale.scale(0.1f, 0.1f, 0.1f);
	cowboy.m_meshes[0]->createBones();

	animation = new Animation();
	animation->loadAnimationAssimp("res/models/cowboy/cowboy.dae", "Armature_Armature", "cowboy_run");
	animation->setPositionOfTrack("Armature_Torso", 0.0f, 0.3f, 0.0f);
	animation->scaleTrack("Armature_Torso", 0.1f, 0.1f, 0.1f);

	cowboy.m_meshes[0]->m_animationStates.push_back(std::shared_ptr<AnimationState>(new AnimationState(animation, cowboy.m_meshes[0]->m_rootBone)));
	cowboy.m_meshes[0]->m_animationStates.back()->SetLooped(true);

	mushroom.loadModelAssimp("res/models/mushroom/mushroom.dae", true, false);
	mushroom.m_meshes[0]->m_meshBones[0].initialPosition.translate(0.0f, 1.0f, 0.0f);
	mushroom.m_meshes[0]->m_meshBones[2].initialPosition.set(0.0f, 0.1f, 0.0f);
	mushroom.m_meshes[0]->m_meshBones[2].initialScale.scale(0.1f, 0.1f, 0.1f);
	mushroom.m_meshes[0]->createBones();

	animation = new Animation();
	animation->loadAnimationAssimp("res/models/mushroom/mushroom.dae", "Armature_Armature", "mushroom_jump");
	animation->setPositionOfTrack("Armature_stalk", 0.0f, 0.1f, 0.0f);
	animation->scaleTrack("Armature_stalk", 0.1f, 0.1f, 0.1f);

	mushroom.m_meshes[0]->m_animationStates.push_back(std::shared_ptr<AnimationState>(new AnimationState(animation, mushroom.m_meshes[0]->m_rootBone)));
	mushroom.m_meshes[0]->m_animationStates.back()->SetLooped(true);

	dragon.loadModelAssimp("res/models/dragon/dragon.dae", true, false);
	dragon.m_meshes[0]->m_meshBones[0].initialScale.scale(0.1f, 0.1f, 0.1f);
	dragon.m_meshes[0]->m_meshBones[0].initialPosition.translate(0.0f, -1.0f, 0.0f);
	dragon.m_meshes[0]->createBones();

	animation = new Animation();
	animation->loadAnimationAssimp("res/models/dragon/dragon.dae", "both_wing", "both_wing");
	//animation->loadAnimationAssimp("res/models/dragon/dragon.dae", "right_wing", "right_wing");
	//animation->loadAnimationAssimp("res/models/dragon/dragon.dae", "left_wing", "left_wing");

	dragon.m_meshes[0]->m_animationStates.push_back(std::shared_ptr<AnimationState>(new AnimationState(animation, dragon.m_meshes[0]->m_rootBone)));
	dragon.m_meshes[0]->m_animationStates.back()->SetLooped(true);

	DebugRenderer::Get().setEnable(true);

	glGenBuffers(1, &BuiltInShader::matrixUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4f) * 96, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 3, BuiltInShader::matrixUbo, 0, sizeof(Matrix4f) * 96);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	StateMachine::ToggleWireframe();
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
	bool move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		directrion += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		directrion += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xDelta();
		dy = mouse.yDelta();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);
		}

		if (move) {
			m_camera.move(directrion * m_dt);
		}
	}

	beta.update(m_dt);
	vampire.update(m_dt);
	cowboy.update(m_dt);
	mushroom.update(m_dt);
	dragon.update(m_dt);

	beta.updateSkinning();
	vampire.updateSkinning();
	cowboy.updateSkinning();
	mushroom.updateSkinning();
	dragon.updateSkinning();
}

void Game::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * beta.m_meshes[0]->m_numBones, beta.m_meshes[0]->m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	//shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	shader->loadVector("u_color", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	
	Globals::textureManager.get("null").bind();
	beta.drawRaw();

	shader->unuse();
	
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * vampire.m_meshes[0]->m_numBones, vampire.m_meshes[0]->m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	//shader->loadMatrix("u_model", Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), 90.0f, Vector3f(1.0f, 0.0f, 0.0f)));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	Globals::textureManager.get("vampire").bind();
	vampire.drawRaw();

	shader->unuse();

	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * cowboy.m_meshes[0]->m_numBones, cowboy.m_meshes[0]->m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	//shader->loadMatrix("u_model", Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), 90.0f, Vector3f(1.0f, 0.0f, 0.0f)));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	Globals::textureManager.get("cowboy").bind();
	cowboy.drawRaw();

	shader->unuse();

	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * mushroom.m_meshes[0]->m_numBones, mushroom.m_meshes[0]->m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	Globals::textureManager.get("mushroom").bind();
	mushroom.drawRaw();

	shader->unuse();

	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * dragon.m_meshes[0]->m_numBones, dragon.m_meshes[0]->m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	Globals::textureManager.get("dragon").bind();
	dragon.drawRaw();

	shader->unuse();

	DebugRenderer::Get().SetView(&m_camera);

	//DebugRenderer::Get().AddBoundingBox(boundingBox, { 1.0f, 0.0f, 0.0f, 1.0f });
	DebugRenderer::Get().AddSkeleton(beta.m_meshes[0]->m_bones, beta.m_meshes[0]->m_numBones, { 0.0f, 1.0f, 0.0f, 1.0f });
	DebugRenderer::Get().AddSkeleton(vampire.m_meshes[0]->m_bones, vampire.m_meshes[0]->m_numBones, { 0.0f, 1.0f, 0.0f, 1.0f });
	DebugRenderer::Get().AddSkeleton(cowboy.m_meshes[0]->m_bones, cowboy.m_meshes[0]->m_numBones, { 0.0f, 1.0f, 0.0f, 1.0f });
	DebugRenderer::Get().AddSkeleton(mushroom.m_meshes[0]->m_bones, mushroom.m_meshes[0]->m_numBones, { 0.0f, 1.0f, 0.0f, 1.0f });
	DebugRenderer::Get().AddSkeleton(dragon.m_meshes[0]->m_bones, dragon.m_meshes[0]->m_numBones, { 1.0f, 0.0f, 0.0f, 1.0f });
	DebugRenderer::Get().drawBuffer();

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
	if(ImGui::Checkbox("Play Dance", &m_playAnimation)){
		vampire.m_meshes[0]->m_animationStates[0]->SetEnabled(m_playAnimation);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

