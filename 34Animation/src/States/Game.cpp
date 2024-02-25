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

	animatedModel.loadModelAssimp("res/models/vampire/dancing_vampire.dae", true);
	animatedModel.m_meshes[0]->m_meshBones[0].initialPosition.translate(1.0f, 0.0f, 0.0f);
	animatedModel.m_meshes[0]->m_meshBones[0].initialRotation.rotate(Vector3f(0.0f, 1.0f, 0.0f), 180.0f);
	
	animatedModel.m_meshes[0]->m_meshBones[1].initialPosition.set(0.0f, 0.0f, 0.0f);
	animatedModel.m_meshes[0]->m_meshBones[1].initialScale.set(0.01f, 0.01f, 0.01f);
	animatedModel.m_meshes[0]->m_meshBones[1].initialPosition.translate(0.0f, 1.0f, 0.0f);
	animatedModel.m_meshes[0]->createBones();

	animation2 = new Animation();
	animation2->loadAnimationAssimp("res/models/vampire/dancing_vampire.dae", "Hips", "vampire_dance");
	animation2->setPositionOfTrack("Hips", 0.0f, 1.0f, 0.0f);
	animation2->setScaleOfTrack("Hips", 0.01f, 0.01f, 0.01f);

	
	AnimationState* state2 = new AnimationState(animation2, animatedModel.m_meshes[0]->m_rootBone);
	state2->SetLooped(true);
	state2->SetEnabled(m_playAnimation);
	animationStates2.push_back(std::shared_ptr<AnimationState>(state2));

	beta.loadModelMdl("res/models/BetaLowpoly/Beta.mdl");
	beta.m_meshes[0]->m_meshBones[0].initialPosition.translate(-1.0f, 0.0f, 0.0f);
	beta.m_meshes[0]->m_meshBones[0].initialRotation.rotate(0.0f, 180.0f, 0.0f);
	beta.m_meshes[0]->createBones();

	animation = new Animation();
	//animation->loadAnimation("res/models/BetaLowpoly/Beta_Idle.ani");
	animation->loadAnimation("res/models/BetaLowpoly/Beta_Run.ani");

	AnimationState* state = new AnimationState(animation, beta.m_meshes[0]->m_rootBone);
	state->SetLooped(true);
	//state->SetEnabled(false);
	animationStates.push_back(std::shared_ptr<AnimationState>(state));


	DebugRenderer::Get().setEnable(true);

	glGenBuffers(1, &BuiltInShader::matrixUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4f) * 96, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 3, BuiltInShader::matrixUbo, 0, sizeof(Matrix4f) * animatedModel.m_meshes[0]->m_numBones);
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

	UpdateAnimation();
	UpdateAnimation2();
}

void Game::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	UpdateSkinning();

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
	
	UpdateSkinning2();
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * animatedModel.m_meshes[0]->m_numBones, animatedModel.m_meshes[0]->m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	//shader->loadMatrix("u_model", Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), 90.0f, Vector3f(1.0f, 0.0f, 0.0f)));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	Globals::textureManager.get("vampire").bind();
	animatedModel.drawRaw();

	shader->unuse();

	DebugRenderer::Get().SetView(&m_camera);

	//DebugRenderer::Get().AddBoundingBox(boundingBox, { 1.0f, 0.0f, 0.0f, 1.0f });
	DebugRenderer::Get().AddSkeleton(beta.m_meshes[0]->m_bones, beta.m_meshes[0]->m_numBones, { 0.0f, 1.0f, 0.0f, 1.0f });
	DebugRenderer::Get().AddSkeleton(animatedModel.m_meshes[0]->m_bones, animatedModel.m_meshes[0]->m_numBones, { 0.0f, 1.0f, 0.0f, 1.0f });
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
		animationStates2[0]->SetEnabled(m_playAnimation);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::UpdateAnimation() {

	//if (animatedModelFlags & AMF_ANIMATION_ORDER_DIRTY)
	//	std::sort(animationStates.begin(), animationStates.end(), CompareAnimationStates);

	//animatedModelFlags |= AMF_IN_ANIMATION_UPDATE | AMF_BONE_BOUNDING_BOX_DIRTY;

	// Reset bones to initial pose, then apply animations
	for (size_t i = 0; i < beta.m_meshes[0]->m_numBones; ++i) {
		Bone* bone = beta.m_meshes[0]->m_bones[i];
		const ModelBone& modelBone = beta.m_meshes[0]->m_meshBones[i];
		if (bone->AnimationEnabled()) {
			bone->SetTransformSilent(modelBone.initialPosition, modelBone.initialRotation, modelBone.initialScale);
		}
	}

	for (auto it = animationStates.begin(); it != animationStates.end(); ++it) {
		AnimationState* state = (*it).get();

		if (state->Enabled()) {
			state->AddTime(m_dt);
			state->Apply();
		}
	}

	/*// Dirty the bone hierarchy now. This will also dirty and queue reinsertion for attached models
	SetBoneTransformsDirty();

	animatedModelFlags &= ~(AMF_ANIMATION_ORDER_DIRTY | AMF_ANIMATION_DIRTY | AMF_IN_ANIMATION_UPDATE);

	// Update bounding box already here to take advantage of threaded update, and also to update bone world transforms for skinning
	OnWorldBoundingBoxUpdate();

	// If updating only when visible, queue octree reinsertion for next frame. This also ensures shadowmap rendering happens correctly
	// Else just dirty the skinning
	if (!TestFlag(DF_UPDATE_INVISIBLE))
	{
		if (octree && octant && !TestFlag(DF_OCTREE_REINSERT_QUEUED))
			octree->QueueUpdate(this);
	}

	animatedModelFlags |= AMF_SKINNING_DIRTY;*/
}

void Game::UpdateSkinning() {

	for (size_t i = 0; i < beta.m_meshes[0]->m_numBones; ++i) {
		beta.m_meshes[0]->m_skinMatrices[i] = beta.m_meshes[0]->m_bones[i]->getWorldTransformation() * beta.m_meshes[0]->m_meshBones[i].offsetMatrix;
	}

	//animatedModelFlags &= ~AMF_SKINNING_DIRTY;
	//animatedModelFlags |= AMF_SKINNING_BUFFER_DIRTY;
}

void Game::UpdateAnimation2() {
	for (size_t i = 0; i < animatedModel.m_meshes[0]->m_numBones; ++i) {
		Bone* bone = animatedModel.m_meshes[0]->m_bones[i];
		const ModelBone& modelBone = animatedModel.m_meshes[0]->m_meshBones[i];
		
		if (bone->AnimationEnabled()) {
			bone->SetTransformSilent(modelBone.initialPosition, modelBone.initialRotation, modelBone.initialScale);		
		}
	}

	for (auto it = animationStates2.begin(); it != animationStates2.end(); ++it) {
		AnimationState* state = (*it).get();
		if (state->Enabled()) {
			state->AddTime(m_dt);
			state->Apply();
		}
	}
}

void Game::UpdateSkinning2() {
	for (size_t i = 0; i < animatedModel.m_meshes[0]->m_numBones; ++i) {
		animatedModel.m_meshes[0]->m_skinMatrices[i] = animatedModel.m_meshes[0]->m_bones[i]->getWorldTransformation() * animatedModel.m_meshes[0]->m_meshBones[i].offsetMatrix;
	}

}