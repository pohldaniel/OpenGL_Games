#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <States/Menu.h>

#include "Md2State.h"
#include "Application.h"
#include "Globals.h"

std::string sModelNames[] =
{
	"Dr. Freak",
	"Blade",
	"Samourai",
	"HoboGoblin"
};

const GLuint STREAM_BUFFER_CAPACITY = 8192 * 1024; // 8MBytes

Md2State::Md2State(StateMachine& machine) : State(machine, States::DEFAULT){

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 20.0f, 50.0f), Vector3f(0.0f, 19.5f, 50.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	md2Models[0].LoadModel("res/models/drfreak/drfreak.md2");
	md2Models[1].LoadModel("res/models/blade/Blade.md2");
	md2Models[2].LoadModel("res/models/samourai/Samourai.md2");
	md2Models[3].LoadModel("res/models/HoboGoblin/Model.md2");
	animationStateMain = md2Models[0].StartAnimation(STAND);

	m_sequence.setStride(8u);
	md2Converter.md2ToSequence("data/models/dynamic/corpse/corpse.md2", true, { 0.0f, -90.0f, 0.0f }, { 0.5f, 0.5f, 0.5f }, m_sequence);
	m_sequence.loadSequenceGpu();

	//md2Converter.loadMd2("data/models/dynamic/hero/hero.md2", true, { 0.0f, -90.0f, 0.0f }, { 0.5f, 0.5f, 0.5f }, res, indexBuffer, m_animation);
	md2Converter.loadMd2("data/models/dynamic/ripper/ripper.md2", true, { 0.0f, -90.0f, 0.0f }, { 0.5f, 0.5f, 0.5f }, res, indexBuffer, m_animation);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, STREAM_BUFFER_CAPACITY, NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// configure vertex arrays
	glBindVertexArray(m_vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 8 * sizeof(float), (void*)0);

	glVertexAttribPointer(1, 2, GL_FLOAT, 0, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	glVertexAttribPointer(2, 2, GL_FLOAT, 0, 8 * sizeof(float), (void*)(5 * sizeof(float)));


	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned int), &indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	currentAnimation = &m_animation[0];
}

Md2State::~Md2State() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Md2State::fixedUpdate() {

}

void Md2State::update() {
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

	if (keyboard.keyPressed(Keyboard::KEY_1)) {
		currentAnimation = &m_animation[0];
		streamOffset = 0;
		drawOffset = 0;
		mActiveFrame = 0.0f;
	}

	if (keyboard.keyPressed(Keyboard::KEY_2)) {
		currentAnimation = &m_animation[1];
		streamOffset = 0;
		drawOffset = 0;
		mActiveFrame = 0.0f;
	}

	if (keyboard.keyPressed(Keyboard::KEY_3)) {
		currentAnimation = &m_animation[2];
		streamOffset = 0;
		drawOffset = 0;
		mActiveFrame = 0.0f;
	}

	if (keyboard.keyPressed(Keyboard::KEY_4)) {
		currentAnimation = &m_animation[3];
		streamOffset = 0;
		drawOffset = 0;
		mActiveFrame = 0.0f;
	}

	if (keyboard.keyPressed(Keyboard::KEY_5)) {
		currentAnimation = &m_animation[4];
		streamOffset = 0;
		drawOffset = 0;
		mActiveFrame = 0.0f;
	}

	if (keyboard.keyPressed(Keyboard::KEY_6)) {
		currentAnimation = &m_animation[5];
		streamOffset = 0;
		drawOffset = 0;
		mActiveFrame = 0.0f;
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

	md2Models[iCurrentModel].UpdateAnimation(&animationStateMain, m_dt);

	// increment frame
	mActiveFrame += m_speed * m_dt * currentAnimation->fps;

	int len = currentAnimation->frames.size() - 1;
	float lenf = static_cast<float>(len);
	
	// loop animation
	if (mActiveFrame >= lenf)
		mActiveFrame = std::modf(mActiveFrame, &mActiveFrame) + std::fmod(mActiveFrame, lenf + 1.0f);

	int16_t activeFrameIdx = static_cast<int16_t>(mActiveFrame);
	uint16_t nextFrame = activeFrameIdx == len ? 0 : activeFrameIdx + 1;

	float lerp = mActiveFrame - floor(mActiveFrame);
	float oneMinusLerp = 1.0f - lerp;

	for (int i = 0; i < res.size(); i = i + 8) {
		const Utils::MD2IO::Frame& frameA = currentAnimation->frames[activeFrameIdx];
		const Utils::MD2IO::Frame& frameB = currentAnimation->frames[nextFrame];

		res[i + 0] = oneMinusLerp * frameA.vertices[i + 0] + lerp * frameB.vertices[i + 0];
		res[i + 1] = oneMinusLerp * frameA.vertices[i + 1] + lerp * frameB.vertices[i + 1];
		res[i + 2] = oneMinusLerp * frameA.vertices[i + 2] + lerp * frameB.vertices[i + 2];

		res[i + 5] = oneMinusLerp * frameA.vertices[i + 5] + lerp * frameB.vertices[i + 5];
		res[i + 6] = oneMinusLerp * frameA.vertices[i + 6] + lerp * frameB.vertices[i + 6];
		res[i + 7] = oneMinusLerp * frameA.vertices[i + 7] + lerp * frameB.vertices[i + 7];
	}	
}

void Md2State::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	Globals::textureManager.get("ripper").bind(0);
	auto shader = Globals::shaderManager.getAssetPointer("shape");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	//gm_sequence.draw(index);
	

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * res.size(), &res[0]);

	//orphan the buffer if full
	GLuint streamDataSize = sizeof(float) * res.size();	
	streamOffset = streamOffset + streamDataSize > STREAM_BUFFER_CAPACITY ? 0 : streamOffset;
	

	//get memory safely
	float* vertices = (float*)(glMapBufferRange(GL_ARRAY_BUFFER, streamOffset, streamDataSize, GL_MAP_WRITE_BIT| GL_MAP_UNSYNCHRONIZED_BIT));
	memcpy(vertices, res.data(), sizeof(float) * res.size());

	//unmap buffer
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	drawOffset = streamOffset / (sizeof(float) * 8);
	glBindVertexArray(m_vao);
	//glDrawElements(GL_TRIANGLES, indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glDrawElementsBaseVertex(GL_TRIANGLES, indexBuffer.size(), GL_UNSIGNED_INT, 0, drawOffset);
	glBindVertexArray(0);

	streamOffset += streamDataSize;
	shader->unuse();

	if (m_drawUi)
		renderUi();
}

void Md2State::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Md2State::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void Md2State::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void Md2State::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Md2State::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Md2State::OnKeyUp(Event::KeyboardEvent& event) {

}

void Md2State::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Md2State::renderUi() {
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
	ImGui::SliderInt("Index", &index, 0, m_sequence.getMeshes().size() - 1);
	ImGui::SliderFloat("Speed", &m_speed, 0.0f, 20.0f);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}