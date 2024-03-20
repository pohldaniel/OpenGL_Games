#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <engine/Material.h>

#include "ParticleInterface.h"

#include "Application.h"
#include "Globals.h"

ParticleInterface::ParticleInterface(StateMachine& machine) : State(machine, States::PARTICLE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(60.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(25.0f, 10.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(5.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_rocket.loadModel("res/models/Rocket_Ship_01.gltf");
	m_rocket.initShader();

	m_alphaSpline.addPoint(0.0, 0.0);
	m_alphaSpline.addPoint(0.1, 1.0);
	m_alphaSpline.addPoint(0.6, 1.0);
	m_alphaSpline.addPoint(1.0, 0.0);

	m_sizeSpline.addPoint(0.0, 1.0);
	m_sizeSpline.addPoint(0.5, 5.0);
	m_sizeSpline.addPoint(1.0, 1.0);

	m_colorSpline.addPoint(0.0f, Vector4f(1.0f, 1.0f, 0.5019607843137255f, 0.0f));
	m_colorSpline.addPoint(1.0f, Vector4f(1.0f, 0.5019607843137255f, 0.5019607843137255f, 0.0f));


	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleVertex) * MAXPARTICLES, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, size));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, color));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, angle));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	srand(time(0));

	particlesBatch = new ParticleVertex[MAXPARTICLES];
	particleBatchPtr = particlesBatch;
}

ParticleInterface::~ParticleInterface() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void ParticleInterface::fixedUpdate() {

}

void ParticleInterface::update() {
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

	addParticles(m_dt);
	updateParticles(m_dt);
}

void ParticleInterface::render() {
	//std::cout << "Size: " << m_particles.size() << std::endl;
	updateGeometry();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	m_rocket.draw(m_camera);
	m_skybox.draw(m_camera);

	particleBatchPtr = particlesBatch;

	if (m_drawUi)
		renderUi();
}

void ParticleInterface::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void ParticleInterface::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void ParticleInterface::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void ParticleInterface::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void ParticleInterface::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void ParticleInterface::OnKeyUp(Event::KeyboardEvent& event) {

}

void ParticleInterface::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void ParticleInterface::renderUi() {
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

float ParticleInterface::getRand() {
	return ((float)rand()) / RAND_MAX;
}

void ParticleInterface::addParticles(float dt) {
	gdfsghk += dt;
	int n = std::floor(gdfsghk * 75.0f);
	gdfsghk -= n / 75.0f;

	for (int i = 0; i < n; i++) {
		float life = (getRand() * 0.75f + 0.25f) * 10.0f;
		float size = (getRand() * 0.5f + 0.5f) * 4.0f;
		m_particles.push_back({
				{getRand() * 2.0f - 1.0f, getRand() * 2.0f - 1.0f, getRand() * 2.0f - 1.0f},
				{0.0f, 15.0f, 0.0f},
				{1.0f, 1.0f, 1.0, 0.0f},
				size,
				size,
				life,
				life,
				getRand() * TWO_PI,
				1.0f,				
		});
	}
}

void ParticleInterface::updateParticles(float dt) {
	for (ParticleNew& particle : m_particles) {
		particle.life -= m_dt;
	}

	m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(), [](const ParticleNew& particle) {return particle.life < 0; }), m_particles.end());

	for (ParticleNew& particle : m_particles) {
		float t = 1.0f - particle.life / particle.maxLife;

		particle.angle += dt * 0.5f;
		particle.alpha = m_alphaSpline.get(t);
		particle.currentSize = particle.size * m_sizeSpline.get(t);
		particle.color = m_colorSpline.get(t);
		particle.position += particle.velocity * dt;
	
		Vector3f drag;
		drag[0] = Math::Sgn(particle.velocity[0]) * std::min(std::fabs(particle.velocity[0] * 0.1f), std::fabs(particle.velocity[0]));
		drag[1] = Math::Sgn(particle.velocity[1]) * std::min(std::fabs(particle.velocity[1] * 0.1f), std::fabs(particle.velocity[1]));
		drag[2] = Math::Sgn(particle.velocity[2]) * std::min(std::fabs(particle.velocity[2] * 0.1f), std::fabs(particle.velocity[2]));
		particle.velocity -= drag;


	}

	std::sort(m_particles.begin(), m_particles.end(), [this](const ParticleNew& lhs, const ParticleNew& rhs){
		float d1 = Vector3f::Length(m_camera.getPosition(), lhs.position);
		float d2 = Vector3f::Length(m_camera.getPosition(), rhs.position);

		if (d1 > d2) {
			return false;
		}

		if (d1 < d2) {
			return true;
		}

		return false;
	});
}

void ParticleInterface::updateGeometry() {
	for (const ParticleNew& particle : m_particles) {
		particleBatchPtr->position = particle.position;
		particleBatchPtr->size = particle.size;
		particleBatchPtr->color = particle.color;	
		particleBatchPtr->angle = particle.angle;
		particleBatchPtr++;
	}
}