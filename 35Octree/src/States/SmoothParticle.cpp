#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <engine/Material.h>

#include "SmoothParticle.h"

#include "Application.h"
#include "Globals.h"

SmoothParticle::SmoothParticle(StateMachine& machine) : State(machine, States::SMOOTHPARTICLE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(60.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(25.0f, 10.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(15.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_rocket.loadModel("res/models/Rocket_Ship_01.gltf");
	m_rocket.initShader();

	m_alphaSpline.addPoint(0.0f, 0.0f);
	m_alphaSpline.addPoint(0.3f, 1.0f);
	m_alphaSpline.addPoint(0.6f, 1.0f);
	m_alphaSpline.addPoint(1.0f, 0.0f);

	m_sizeSpline.addPoint(0.0f, 1.0f);
	m_sizeSpline.addPoint(0.5f, 5.0f);
	m_sizeSpline.addPoint(1.0f, 1.0f);

	m_colorSpline.addPoint(0.0f, Vector4f(1.0f, 1.0f, 0.5019607843137255f, 0.0f));
	m_colorSpline.addPoint(1.0f, Vector4f(1.0f, 0.5019607843137255f, 0.5019607843137255f, 0.0f));


	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleVertex) * MAX_PARTICLES, nullptr, GL_DYNAMIC_DRAW);
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

	particleBatch = new ParticleVertex[MAX_PARTICLES];
	particleBatchPtr = particleBatch;

	//glEnable(GL_ALPHA_TEST);
	//glAlphaFunc(GL_GREATER, 0.05f);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SPRITE);

	m_depthBuffer.create(Application::Width, Application::Height);
	m_depthBuffer.attachTexture2D(AttachmentTex::DEPTH24);

	m_sceneBuffer.create(Application::Width, Application::Height);
	m_sceneBuffer.attachTexture2D(AttachmentTex::RGBA);
	m_sceneBuffer.attachTexture2D(AttachmentTex::DEPTH24);
}

SmoothParticle::~SmoothParticle() {
	glAlphaFunc(GL_ALWAYS, 0.0f);
	glDisable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void SmoothParticle::fixedUpdate() {

}

void SmoothParticle::update() {
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

	if (keyboard.keyDown(Keyboard::KEY_SPACE)) {
		if (m_particles.size() < MAX_PARTICLES)
			addParticles(0, 1);
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

	if (m_addParticle)
		addParticles(m_dt);

	updateParticles(m_dt);
}

void SmoothParticle::render() {

	m_depthBuffer.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("depth");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());

	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	m_rocket.drawRaw();

	shader->loadMatrix("u_model", Matrix4f::Translate(0.0f, -10.0f, 0.0f));
	Globals::shapeManager.get("torus_knot").drawRaw();


	shader->loadMatrix("u_model", Matrix4f::Translate(0.0f, -30.0f, 0.0f));
	Globals::shapeManager.get("plane").drawRaw();
	shader->unuse();
	m_depthBuffer.unbind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_rocket.draw(m_camera);
	shader = Globals::shaderManager.getAssetPointer("color");
	shader->use();
	

	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());

	shader->loadVector("u_color", Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	shader->loadMatrix("u_model", Matrix4f::Translate(0.0f, -10.0f, 0.0f));
	Globals::shapeManager.get("torus_knot").drawRaw();

	shader->loadVector("u_color", Vector4f(0.25098039215686274f, 0.25098039215686274f, 0.25098039215686274f, 1.0f));
	shader->loadMatrix("u_model", Matrix4f::Translate(0.0f, -30.0f, 0.0f));

	Globals::shapeManager.get("plane").drawRaw();
	shader->unuse();
	m_skybox.draw(m_camera);

	glDepthMask(false);
	updateGeometry();
	glBlendFunc(GL_ONE, GL_ONE);
	glBindVertexArray(m_vao);
	GLsizeiptr size = (uint8_t*)particleBatchPtr - (uint8_t*)particleBatch;
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, particleBatch);
	shader = Globals::shaderManager.getAssetPointer("particle_smooth");
	shader->use();
	shader->loadFloat("u_fallOff", m_fallOff);
	shader->loadFloat("u_near", m_camera.getNear());
	shader->loadFloat("u_far", m_camera.getFar());
	shader->loadVector("u_resolution", Vector2f(static_cast<float>(Application::Width), static_cast<float>(Application::Height)));
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	shader->loadFloat("pointMultiplier", static_cast<float>(Application::Height / 2) / (m_camera.getScaleFactor()));
	shader->loadInt("u_texture", 0);
	shader->loadInt("u_depth", 1);
	Globals::textureManager.get("fire").bind(0u);
	m_depthBuffer.bindDepthTexture(1u);
	glDrawArrays(GL_POINTS, 0, m_particleCount);
	shader->unuse();
	particleBatchPtr = particleBatch;
	m_particleCount = 0;
	glDepthMask(true);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", Matrix4f::IDENTITY);
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	shader->loadMatrix("u_model", Matrix4f::Translate(static_cast<float>(Application::Width) - 205.0f, 205.0f, 0.0f) * Matrix4f::Scale(200.0f, 200.0f, 0.0f));
	m_depthBuffer.bindDepthTexture(0u);
	
	Globals::shapeManager.get("quad").drawRaw();
	shader->unuse();

	if (m_drawUi)
		renderUi();
}

void SmoothParticle::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void SmoothParticle::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void SmoothParticle::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void SmoothParticle::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void SmoothParticle::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void SmoothParticle::OnKeyUp(Event::KeyboardEvent& event) {

}

void SmoothParticle::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void SmoothParticle::renderUi() {
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
	ImGui::Checkbox("Add Particle", &m_addParticle);
	ImGui::SliderFloat("Fall Off", &m_fallOff, 0.0f, 1.0f);
	if (!m_addParticle)
		ImGui::Text("Hold Space");

	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

float SmoothParticle::getRand() {
	return ((float)rand()) / RAND_MAX;
}

void SmoothParticle::addParticles(float dt, int count) {

	int n;
	if (count == 0) {
		gdfsghk += dt;
		n = std::floor(gdfsghk * 75.0f);
		gdfsghk -= n / 75.0f;
	}
	else {
		n = count;
	}


	for (int i = 0; i < n; i++) {
		float life = (getRand() * 0.75f + 0.25f) * 10.0f;
		float size = (getRand() * 0.5f + 0.5f) * 4.0f;
		m_particles.push_back({
				{getRand() * 2.0f - 1.0f, getRand() * 2.0f - 1.0f, getRand() * 2.0f - 1.0f},
				{0.0f, -15.0f, 0.0f},
				{1.0f, 1.0f, 1.0, 1.0f},
				size,
				size,
				life,
				life,
				getRand() * TWO_PI,
				1.0f,
				0.0f
			});
	}
}



void SmoothParticle::updateParticles(float dt) {
	for (SmoothParticle::Particle& particle : m_particles) {
		particle.life -= m_dt;
	}

	m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(), [](const SmoothParticle::Particle& particle) {return particle.life < 0; }), m_particles.end());

	for (SmoothParticle::Particle& particle : m_particles) {
		float t = 1.0f - particle.life / particle.maxLife;

		particle.angle += dt * 0.5f;
		particle.alpha = m_alphaSpline.get(t);
		particle.color = m_colorSpline.get(t);
		particle.color[3] = particle.alpha;

		particle.currentSize = particle.size * m_sizeSpline.get(t);
		particle.position += particle.velocity * dt;
		particle.cameraDistance = (particle.position - m_camera.getPosition()).lengthSq();

		Vector3f drag;
		drag[0] = Math::Sgn(particle.velocity[0]) * std::min(std::fabs(particle.velocity[0] * dt * 0.1f), std::fabs(particle.velocity[0]));
		drag[1] = Math::Sgn(particle.velocity[1]) * std::min(std::fabs(particle.velocity[1] * dt * 0.1f), std::fabs(particle.velocity[1]));
		drag[2] = Math::Sgn(particle.velocity[2]) * std::min(std::fabs(particle.velocity[2] * dt * 0.1f), std::fabs(particle.velocity[2]));
		particle.velocity -= drag;


	}

	std::sort(m_particles.begin(), m_particles.end());
}

void SmoothParticle::updateGeometry() {
	for (const SmoothParticle::Particle& particle : m_particles) {
		particleBatchPtr->position = particle.position;
		particleBatchPtr->size = particle.currentSize;
		particleBatchPtr->color = particle.color;
		particleBatchPtr->angle = particle.angle;


		particleBatchPtr++;
		m_particleCount++;
	}
}