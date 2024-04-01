#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <engine/DebugRenderer.h>

#include "Basic.h"
#include "Menu.h"
#include "Application.h"
#include "Globals.h"

Basic::Basic(StateMachine& machine) : State(machine, States::BASIC) {

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

	vampire.loadModelAssimp("res/models/vampire/dancing_vampire.dae", true, true);
	vampire.m_meshes[0]->m_meshBones[0].initialPosition.translate(1.5f, 0.0f, 0.0f);
	vampire.m_meshes[0]->m_meshBones[0].initialRotation.rotate(Vector3f(0.0f, 1.0f, 0.0f), 180.0f);
	vampire.m_meshes[0]->m_meshBones[1].initialPosition.set(0.0f, 1.0f, 0.0f);
	vampire.m_meshes[0]->m_meshBones[1].initialScale.scale(0.01f, 0.01f, 0.01f);
	vampire.m_meshes[0]->createBones();
	vampire.addAnimationState(Globals::animationManagerNew.getAssetPointer("vampire_dance"));
	vampire.getAnimationState(0)->SetLooped(true);
	vampire.getAnimationState(0)->SetLooped(m_playAnimation);

	beta.loadModelMdl("res/models/BetaLowpoly/Beta.mdl");
	beta.m_meshes[0]->m_meshBones[0].initialPosition.translate(-1.0f, 0.0f, 0.0f);
	beta.m_meshes[0]->m_meshBones[0].initialRotation.rotate(0.0f, 180.0f, 0.0f);
	beta.m_meshes[0]->createBones();

	beta.addAnimationState(Globals::animationManagerNew.getAssetPointer("beta_run"));
	beta.getAnimationState(0)->SetLooped(true);

	cowboy.loadModelAssimp("res/models/cowboy/cowboy.dae", 1, false);
	cowboy.m_meshes[0]->m_meshBones[0].initialPosition.set(1.5f, -1.2f, 0.0f);

	cowboy.m_meshes[0]->m_meshBones[1].initialPosition.set(0.0f, 0.3f, 0.0f);
	cowboy.m_meshes[0]->m_meshBones[1].initialScale.scale(0.1f, 0.1f, 0.1f);
	cowboy.m_meshes[0]->createBones();
	cowboy.addAnimationState(Globals::animationManagerNew.getAssetPointer("cowboy_run"));
	cowboy.getAnimationState(0)->SetLooped(true);

	mushroom.loadModelAssimp("res/models/mushroom/mushroom.dae", true, false);
	mushroom.m_meshes[0]->m_meshBones[0].initialPosition.translate(0.0f, 1.0f, 0.0f);
	mushroom.m_meshes[0]->m_meshBones[2].initialPosition.set(0.0f, 0.1f, 0.0f);
	mushroom.m_meshes[0]->m_meshBones[2].initialScale.scale(0.1f, 0.1f, 0.1f);
	mushroom.m_meshes[0]->createBones();
	mushroom.addAnimationState(Globals::animationManagerNew.getAssetPointer("mushroom_jump"));
	mushroom.getAnimationState(0)->SetLooped(true);

	dragon.loadModelAssimp("res/models/dragon/dragon.dae", true, false);
	dragon.m_meshes[0]->m_meshBones[0].initialScale.scale(0.1f, 0.1f, 0.1f);
	dragon.m_meshes[0]->m_meshBones[0].initialPosition.translate(0.0f, -1.0f, 0.0f);
	dragon.m_meshes[0]->createBones();
	//dragon.addAnimationState(Globals::animationManagerNew.getAssetPointer("both_wing"));

	dragon.addAnimationState(Globals::animationManagerNew.getAssetPointer("right_wing"));
	dragon.addAnimationState(Globals::animationManagerNew.getAssetPointer("left_wing"));

	dragon.getAnimationState(0)->SetLooped(true);
	dragon.getAnimationState(0)->SetWeight(1.0f);
	dragon.getAnimationState(0)->SetBlendMode(ABM_LERP);

	dragon.getAnimationState(1)->SetLooped(true);
	dragon.getAnimationState(1)->SetWeight(1.0f);
	dragon.getAnimationState(1)->SetBlendMode(ABM_LERP);

	woman.loadModelAssimp("res/models/woman/Woman.gltf", true, false);
	woman.m_meshes[0]->m_meshBones[0].initialScale.scale(0.001f, 0.001f, 0.001f);
	woman.m_meshes[0]->m_meshBones[0].initialPosition.translate(0.0f, 0.0f, 2.0f);
	woman.m_meshes[0]->createBones();

	/*woman.addAnimationState(Globals::animationManagerNew.getAssetPointer("woman_walk"));
	woman.addAnimationState(Globals::animationManagerNew.getAssetPointer("woman_run"));
	woman.addAnimationState(Globals::animationManagerNew.getAssetPointer("woman_jump_1"));
	woman.addAnimationState(Globals::animationManagerNew.getAssetPointer("woman_lean_left"));

	woman.getAnimationState(0)->SetLooped(true);
	woman.getAnimationState(0)->SetWeight(0.0f);
	woman.getAnimationState(0)->SetBlendMode(ABM_LERP);

	woman.getAnimationState(1)->SetLooped(true);
	woman.getAnimationState(1)->SetWeight(0.0f);
	woman.getAnimationState(1)->SetBlendMode(ABM_LERP);
	woman.getAnimationState(1)->SetFadeLayerLength(woman.getAnimationState(0)->Length());

	woman.getAnimationState(3)->SetBackward(true);
	woman.getAnimationState(3)->SetWeight(0.0f);
	woman.getAnimationState(3)->SetBlendMode(ABM_ADDITIVE);*/

	m_animationController = new AnimationController(&woman);
	m_currentAnimation = { "woman_walk",  0.0f, m_animationController->GetAnimationState(StringHash("woman_walk")) };

	DebugRenderer::Get().setEnable(true);

	glGenBuffers(1, &BuiltInShader::matrixUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4f) * 96, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 3, BuiltInShader::matrixUbo, 0, sizeof(Matrix4f) * 96);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//StateMachine::ToggleWireframe();
}

Basic::~Basic() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Basic::fixedUpdate() {

}

void Basic::update() {
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();

	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;

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

	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {

		//m_animationController->StopLayer(0);
		//m_animationController->PlayExclusive("beta_jump_start", 0, false, 0.2f);
		//m_animationController->SetTime("beta_jump_start", 0);

		m_animationController->PlayExclusive("woman_jump_2", 0, false, Globals::animationManagerNew.getAssetPointer("woman_run")->Length());
		m_length = Globals::animationManagerNew.getAssetPointer("woman_jump_2")->Length();
	}

	if (mouse.buttonPressed(Mouse::MouseButton::BUTTON_LEFT)) {
		m_animationController->PlayExclusive("woman_punch", 0, false, Globals::animationManagerNew.getAssetPointer("woman_run")->Length());
		m_length = Globals::animationManagerNew.getAssetPointer("woman_punch")->Length();
	}

	//m_animationController->PlayExclusive(m_currentAnimation.name, 0, true, m_currentAnimation.fadeLength);
	m_animationController->Play(m_currentAnimation.name, 0, true, m_currentAnimation.fadeLength);

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
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

	beta.update(m_dt);
	vampire.update(m_dt);
	cowboy.update(m_dt);
	mushroom.update(m_dt);
	dragon.update(m_dt);
	woman.update(m_dt);
	m_animationController->Update(m_dt);

	beta.updateSkinning();
	vampire.updateSkinning();
	cowboy.updateSkinning();
	mushroom.updateSkinning();
	dragon.updateSkinning();
	woman.updateSkinning();
}

void Basic::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * beta.m_meshes[0]->m_numBones, beta.m_meshes[0]->m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadVector("u_light", Vector3f(1.0f, 1.0f, 1.0f));
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

	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * woman.m_meshes[0]->m_numBones, woman.m_meshes[0]->m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	Globals::textureManager.get("woman").bind();
	woman.drawRaw();

	shader->unuse();
	if (m_debug) {
		DebugRenderer::Get().SetView(&m_camera);
		DebugRenderer::Get().AddSkeleton(beta.m_meshes[0]->m_bones, beta.m_meshes[0]->m_numBones, { 0.0f, 1.0f, 0.0f, 1.0f });
		DebugRenderer::Get().AddSkeleton(vampire.m_meshes[0]->m_bones, vampire.m_meshes[0]->m_numBones, { 0.0f, 1.0f, 0.0f, 1.0f });
		DebugRenderer::Get().AddSkeleton(cowboy.m_meshes[0]->m_bones, cowboy.m_meshes[0]->m_numBones, { 0.0f, 1.0f, 0.0f, 1.0f });
		DebugRenderer::Get().AddSkeleton(mushroom.m_meshes[0]->m_bones, mushroom.m_meshes[0]->m_numBones, { 0.0f, 1.0f, 0.0f, 1.0f });
		DebugRenderer::Get().AddSkeleton(dragon.m_meshes[0]->m_bones, dragon.m_meshes[0]->m_numBones, { 1.0f, 0.0f, 0.0f, 1.0f });
		DebugRenderer::Get().AddSkeleton(woman.m_meshes[0]->m_bones, woman.m_meshes[0]->m_numBones, { 0.0f, 1.0f, 0.0f, 1.0f });
		DebugRenderer::Get().drawBuffer();
	}

	if (m_drawUi)
		renderUi();
}

void Basic::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Basic::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void Basic::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void Basic::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Basic::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Basic::OnKeyUp(Event::KeyboardEvent& event) {

}

void Basic::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Basic::renderUi() {
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

	if (ImGui::Checkbox("Debug", &m_debug)) {
		if(m_debug)
			StateMachine::GetEnableWireframe() = true;
		else
			StateMachine::GetEnableWireframe() = false;
	}

	if (ImGui::Checkbox("Play Dance", &m_playAnimation)) {
		vampire.getAnimationState(0)->SetLooped(m_playAnimation);
	}

	if (ImGui::SliderFloat("Weight Left Wing", &m_weightLeft, 0.0f, 1.0f)) {
		dragon.getAnimationState(0)->SetWeight(m_weightLeft);
	}

	if (ImGui::SliderFloat("Weight Right Wing", &m_weightRight, 0.0f, 1.0f)) {
		dragon.getAnimationState(1)->SetWeight(m_weightRight);
	}

	if (ImGui::Button("Walk")) {

		if (m_currentAnimation.name == "woman_jump_2") {
			float restTime = m_currentAnimation.state->getRestTime();

			//offsetSpeed > 1.0f --> woman_walk starts behind woman_jump
			//offsetSpped < 1.0f --> woman_walk starts within woman_jump
			float offsetSpeed = 0.85f;


			m_currentAnimation = { "woman_walk",
								   0.6f,
								   m_animationController->AddAnimationStateFront(Globals::animationManagerNew.getAssetPointer("woman_walk"), true, 1.0f, restTime * offsetSpeed) };
			//fade begins during the last 25% of woman_jump
			float weightOffset = 0.75f;
			m_animationController->FadeOtherExclusive(m_currentAnimation.name, 0.0f, restTime, weightOffset);
		}
		else if (m_currentAnimation.name == "woman_punch") {
			m_currentAnimation = { "woman_walk",  Globals::animationManagerNew.getAssetPointer("woman_walk")->Length() };
			m_animationController->FadeOthers(m_currentAnimation.name, 0.0f, m_currentAnimation.fadeLength);
		}
		else {
			m_currentAnimation = { "woman_walk",
								   Globals::animationManagerNew.getAssetPointer("woman_walk")->Length(),
								   m_animationController->AddAnimationStateFront(Globals::animationManagerNew.getAssetPointer("woman_walk")) };
			m_animationController->FadeOthers(m_currentAnimation.name, 0.0f, m_currentAnimation.fadeLength);
		}
	}

	if (ImGui::Button("Run")) {



		if (m_currentAnimation.name == "woman_jump_2") {
			float restTime = m_currentAnimation.state->getRestTime();
			m_currentAnimation = { "woman_run",
								   Globals::animationManagerNew.getAssetPointer("woman_run")->Length(),
								   m_animationController->AddAnimationStateFront(Globals::animationManagerNew.getAssetPointer("woman_run")) };
			m_animationController->FadeOtherExclusive(m_currentAnimation.name, 0.0f, restTime);

		}
		else if (m_currentAnimation.name == "woman_punch") {
			m_currentAnimation = { "woman_run",  Globals::animationManagerNew.getAssetPointer("woman_run")->Length() };
			m_animationController->FadeOthers(m_currentAnimation.name, 0.0f, m_currentAnimation.fadeLength);
		}
		else {

			m_currentAnimation = { "woman_run",
								   Globals::animationManagerNew.getAssetPointer("woman_run")->Length(),
								   m_animationController->AddAnimationStateFront(Globals::animationManagerNew.getAssetPointer("woman_run")) };
			m_animationController->FadeOthers(m_currentAnimation.name, 0.0f, m_currentAnimation.fadeLength);
		}
	}

	if (ImGui::Button("Jump")) {
		m_currentAnimation = { "woman_jump_2",
							   Globals::animationManagerNew.getAssetPointer("woman_jump_2")->Length(),
							   m_animationController->AddAnimationStateFront(Globals::animationManagerNew.getAssetPointer("woman_jump_2")) };
		m_animationController->FadeOthers(m_currentAnimation.name, 0.0f, 0.2f);
		//m_currentAnimation = { "woman_jump_2",  Globals::animationManagerNew.getAssetPointer("woman_jump_2")->Length() };
		//m_animationController->FadeOthers(m_currentAnimation.name, 0.0f, m_currentAnimation.fadeLength);
	}

	if (ImGui::Button("Punch")) {
		m_currentAnimation = { "woman_punch",
							   Globals::animationManagerNew.getAssetPointer("woman_punch")->Length(),
							   m_animationController->AddAnimationStateFront(Globals::animationManagerNew.getAssetPointer("woman_punch")) };
		m_animationController->FadeOthers(m_currentAnimation.name, 0.0f, m_currentAnimation.fadeLength);


		//m_animationController->AddAnimationStateFront(Globals::animationManagerNew.getAssetPointer("woman_punch"));
		//m_currentAnimation = { "woman_punch",  Globals::animationManagerNew.getAssetPointer("woman_punch")->Length() };
		//m_animationController->FadeOthers(m_currentAnimation.name, 0.0f, m_currentAnimation.fadeLength);
	}

	if (ImGui::Button("Pick Up")) {

		if (m_currentAnimation.name == "woman_jump_2") {
			float restTime = m_currentAnimation.state->getRestTime();
			m_currentAnimation = { "woman_pick_up",
								   Globals::animationManagerNew.getAssetPointer("woman_pick_up")->Length(),
								   m_animationController->AddAnimationStateFront(Globals::animationManagerNew.getAssetPointer("woman_pick_up")) };
			m_animationController->FadeOtherExclusive(m_currentAnimation.name, 0.0f, restTime);
		}
		else {
			m_animationController->Stop(m_currentAnimation.name, 0.2f);
			m_currentAnimation = { "woman_pick_up",
								   Globals::animationManagerNew.getAssetPointer("woman_pick_up")->Length(),
								   m_animationController->AddAnimationStateFront(Globals::animationManagerNew.getAssetPointer("woman_pick_up")) };
			m_animationController->FadeOthers(m_currentAnimation.name, 0.0f, 0.0f);
		}

		//m_animationController->AddAnimationStateFront(Globals::animationManagerNew.getAssetPointer("woman_punch"));
		//m_currentAnimation = { "woman_punch",  Globals::animationManagerNew.getAssetPointer("woman_punch")->Length() };
		//m_animationController->FadeOthers(m_currentAnimation.name, 0.0f, m_currentAnimation.fadeLength);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

