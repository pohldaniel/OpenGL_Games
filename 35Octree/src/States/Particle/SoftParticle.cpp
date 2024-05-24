#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <States/Menu.h>

#include "SoftParticle.h"
#include "Application.h"
#include "Globals.h"

SoftParticle::SoftParticle(StateMachine& machine) : State(machine, States::SOFTPARTICLE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(10.0f, 15.0f, 40.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	glClearColor(0.1607843f, 0.1568628f, 0.1568628f, 1.0f);
	glClearDepth(1.0f);

	m_depthBuffer.create(Application::Width, Application::Height);
	m_depthBuffer.attachTexture2D(AttachmentTex::DEPTH24);

	m_jesus.loadModel("res/models/Jesus.obj");
	m_jesus.initShader();
	m_jesus.getMesh(0)->setMaterialIndex(0);
	m_jesus.markForDelete();

	m_rock1.loadModel("res/models/Rock1.obj");
	m_rock1.initShader();
	m_rock1.getMesh(0)->setMaterialIndex(1);
	m_rock1.markForDelete();

	m_rock2.loadModel("res/models/Rock2.obj");
	m_rock2.initShader();
	m_rock2.getMesh(0)->setMaterialIndex(2);
	m_rock2.markForDelete();

	m_particleSystem.duration_sec = 25.0f;
	m_particleSystem.loop = true;
	m_particleSystem.setLifetime(25.0f);
	m_particleSystem.Speed.addKey(Utils::Key<float>(0.0f, 1.0f));
	m_particleSystem.Size.addKey(Utils::Key<float>(0.0f, 8.0f));
	m_particleSystem.setRateOverTime(1.5f);
	m_particleSystem.Color.addKey(Utils::Key<Vector3f>(0.0f, Vector3f(1.0f, 1.0f, 1.0f)));

	m_particleSystem.Alpha.addKey(Utils::Key<float>(0.0f, 0.0f));
	m_particleSystem.Alpha.addKey(Utils::Key<float>(0.05f, 1.0f));
	m_particleSystem.Alpha.addKey(Utils::Key<float>(0.95f, 1.0f));
	m_particleSystem.Alpha.addKey(Utils::Key<float>(1.0f, 0.0f));
	m_particleSystem.textureColor = Vector4f(0.3584906f, 0.3584906f, 0.3584906f, 0.6039216f);


	m_particleSystem.boxEmmiter.setPosition(0.0f, 3.5f, 12.97f);

	m_particleSystem.prewarmStart();
}

SoftParticle::~SoftParticle() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	ObjModel::CleanupShader();
	Material::CleanupMaterials();
}

void SoftParticle::fixedUpdate() {

}

void SoftParticle::update() {
	Keyboard &keyboard = Keyboard::instance();
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
			m_camera.move(direction * m_dt);
		}
	}
	m_particleSystem.update(std::min(m_dt, 1.0f / 24.0f));
}

void SoftParticle::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);

	m_jesus.drawRaw();

	shader->loadMatrix("u_model", Matrix4f::Translate(-5.0f, 0.0f, 0.0f));
	m_rock1.drawRaw();

	shader->loadMatrix("u_model", Matrix4f::Translate(5.0f, 0.0f, 0.0f));
	m_rock2.drawRaw();

	shader->unuse();

	m_depthBuffer.blitBackBufferToDepthTexture();

	m_particleSystem.sortPositions(m_camera.getPosition(), m_camera.getViewDirection());

	if (m_particleSystem.soft) {

		auto shader = Globals::shaderManager.getAssetPointer("particle_soft");
		shader->use();
		shader->loadMatrix("uMVP", m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix());
		shader->loadMatrix("uV_inv", m_camera.getInvViewMatrix());
		shader->loadVector("uColor", m_particleSystem.textureColor);
		shader->loadInt("uTexture", 0);
		shader->loadInt("uDepthTextureComponent24", 1);
		shader->loadVector("uScreenSize", Vector2f(static_cast<float>(Application::Width), static_cast<float>(Application::Height)));
		shader->loadVector("u_FMinusN_FPlusN_FTimesNTimes2_N", Vector4f(100.0f - 0.1f, 100.0f + 0.1f, 100.0f * 0.1f * 2.0f, 0.1f));

		Globals::textureManager.get("smoke").bind();
		m_depthBuffer.bindDepthTexture(1);

		draw();

		shader->unuse();

	}else {
		auto shader = Globals::shaderManager.getAssetPointer("particle_normal");
		shader->use();
		shader->loadMatrix("uMVP", m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix());
		shader->loadMatrix("uV_inv", m_camera.getInvViewMatrix());
		shader->loadVector("uColor", m_particleSystem.textureColor);
		shader->loadInt("uTexture", 0);
		Globals::textureManager.get("smoke").bind();

		draw();

		shader->unuse();
	}


	if (m_drawUi)
		renderUi();

}

void SoftParticle::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void SoftParticle::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void SoftParticle::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void SoftParticle::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void SoftParticle::OnKeyDown(Event::KeyboardEvent& event) {
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

void SoftParticle::OnKeyUp(Event::KeyboardEvent& event) {

}

void SoftParticle::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_depthBuffer.resize(Application::Width, Application::Height);
}

void SoftParticle::renderUi() {
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
	if (ImGui::Checkbox("Soft Particle", &m_soft)) {
		m_particleSystem.soft = !m_particleSystem.soft;
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SoftParticle::draw() {
	NormalAttributes aux;
	normalVertex.clear();

	for (int i = 0; i < m_particleSystem.particles.size(); i++) {
		const ParticleSystem::Particle& p = m_particleSystem.particles[i];

		aux.pos = p.pos;
		aux.color = p.color;
		aux.size = p.size;
		aux.alpha = p.alpha;

		aux.uv = Vector2f(0.0f, 0.0f);
		normalVertex.push_back(aux);
		aux.uv = Vector2f(1.0f, 1.0f);
		normalVertex.push_back(aux);
		aux.uv = Vector2f(0.0f, 1.0f);
		normalVertex.push_back(aux);

		aux.uv = Vector2f(0.0f, 0.0f);
		normalVertex.push_back(aux);
		aux.uv = Vector2f(1.0f, 0.0f);
		normalVertex.push_back(aux);
		aux.uv = Vector2f(1.0f, 1.0f);
		normalVertex.push_back(aux);
	}

	//glDepthMask(false);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].pos);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].uv);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].color);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].size);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].alpha);

	glDrawArrays(GL_TRIANGLES, 0, normalVertex.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}