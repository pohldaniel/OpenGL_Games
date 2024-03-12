#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "Interleaved.h"
#include "Menu.h"
#include "Application.h"
#include "Globals.h"

Interleaved::Interleaved(StateMachine& machine) : State(machine, States::INTERLEAVED) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 40.0f), Vector3f(0.0f, 0.0f, 40.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);
	m_background.resize(Application::Width, Application::Height);
	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);

	

	g_Object.m_pTextureCoords = new CVector2[MAX_VERTICES];
	g_Object.m_pVertices = new CVector3[MAX_VERTICES];
	g_Object.m_pIndices = new UINT[MAX_VERTICES];

	for (int triangleGroup = 0; triangleGroup < 5; triangleGroup++) {
		AddTriangle(0, 0, 0);			// Make a centered triangle
		AddTriangle(-2.0f, -4.0f, 0);	// Make a triangle on the left
		AddTriangle(2.0f, -4.0f, 0);	// Make a triangle on the right
	}


	for (int i = 0; i < MAX_VERTICES; i++)
		g_Object.m_pIndices[i] = i;

	for (int n = 0; n < 9; n++) {
		g_InterleavedVertices[n].m_textureCoord = g_Object.m_pTextureCoords[n];
		g_InterleavedVertices[n].m_vertex = g_Object.m_pVertices[n];
	}

}

Interleaved::~Interleaved() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	delete g_Object.m_pTextureCoords;
	delete g_Object.m_pVertices;
	delete g_Object.m_pIndices;
}

void Interleaved::fixedUpdate() {

}

void Interleaved::update() {
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
		m_background.addOffset(-0.001f);
		m_background.setSpeed(-0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
		m_background.addOffset(0.001f);
		m_background.setSpeed(0.005f);
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

	m_background.update(m_dt);
}

void Interleaved::render() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getPerspectiveMatrix()[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getViewMatrix()[0][0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderScene();
	m_background.draw();

	if (m_drawUi)
		renderUi();
}

void Interleaved::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Interleaved::OnMouseButtonDown(Event::MouseButtonEvent& event) {

}

void Interleaved::OnMouseButtonUp(Event::MouseButtonEvent& event) {

}

void Interleaved::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Interleaved::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Interleaved::OnKeyUp(Event::KeyboardEvent& event) {

}

void Interleaved::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Interleaved::renderUi() {
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

void Interleaved::DrawVertexArrays() {
	static float rotateY = 0;

	Globals::textureManager.get("texture_1").bind();

	glPushMatrix();
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 0, 9);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(8.0f, 0.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 9, 9);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-8.0f, 0.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 18, 9);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, -8.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 27, 9);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 8.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 36, 9);
	glPopMatrix();

	rotateY += 3;
}


void Interleaved::DrawIndexedVertexArrays() {

	static float rotateY = 0;

	Globals::textureManager.get("texture_2").bind();

	glPushMatrix();

	glRotatef(180, 1.0f, 0.0f, 0.0f);

	glPushMatrix();
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, g_Object.m_pIndices);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(8.0f, 0.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, &g_Object.m_pIndices[9]);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-8.0f, 0.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, &g_Object.m_pIndices[18]);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, -8.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, &g_Object.m_pIndices[27]);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 8.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, &g_Object.m_pIndices[36]);
	glPopMatrix();
	glPopMatrix();
	rotateY += 1;
}

void Interleaved::DrawInterleavedVertexArrays() {
	static float rotateY = 0;
	Globals::textureManager.get("texture_3").bind();
	glPushMatrix();
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, -10.0f);
	glInterleavedArrays(GL_T2F_V3F, sizeof(CVertex), &g_InterleavedVertices);
	glDrawArrays(GL_TRIANGLES, 0, 9);
	glPopMatrix();
	rotateY += 2;
}

void Interleaved::DrawArrayElementVertexArrays() {
	static float rotateY = 0;
	Globals::textureManager.get("texture_4").bind();
	glPushMatrix();
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 10.0f);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 9; i++)
		glArrayElement(i);
	glEnd();

	glPopMatrix();
	rotateY += 2;
}

void Interleaved::RenderScene() {
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, g_Object.m_pTextureCoords);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, g_Object.m_pVertices);

	DrawVertexArrays();
	DrawIndexedVertexArrays();
	DrawInterleavedVertexArrays();
	DrawArrayElementVertexArrays();
}

void Interleaved::AddTriangle(float xOffset, float yOffset, float zOffset) {

	// Set up the left, top and right corner of a new triangle, depending on the offset
	CVector3 vLeftCorner;
	vLeftCorner.x = xOffset - 2.0f; vLeftCorner.y = yOffset; vLeftCorner.z = zOffset;

	CVector3 vTopCorner;
	vTopCorner.x = xOffset; vTopCorner.y = yOffset + 4.0f; vTopCorner.z = zOffset;

	CVector3 vRightCorner;
	vRightCorner.x = xOffset + 2.0f; vRightCorner.y = yOffset; vRightCorner.z = zOffset;

	// Create the basic UV coordinates for the triangle corners
	CVector2 vLeftUV;
	vLeftUV.x = 0.0f; vLeftUV.y = 1.0f;

	CVector2 vTopUV;
	vTopUV.x = 0.5f; vTopUV.y = 0.0f;

	CVector2 vRightUV;
	vRightUV.x = 1.0f; vRightUV.y = 1.0f;

	// Add a new triangle to our array of vertices
	g_Object.m_pVertices[g_Object.m_numOfTriangles * 3] = vLeftCorner;
	g_Object.m_pVertices[g_Object.m_numOfTriangles * 3 + 1] = vTopCorner;
	g_Object.m_pVertices[g_Object.m_numOfTriangles * 3 + 2] = vRightCorner;

	// Add the triangle's UV coordinates to our texture coordinate array
	g_Object.m_pTextureCoords[g_Object.m_numOfTriangles * 3] = vLeftUV;
	g_Object.m_pTextureCoords[g_Object.m_numOfTriangles * 3 + 1] = vTopUV;
	g_Object.m_pTextureCoords[g_Object.m_numOfTriangles * 3 + 2] = vRightUV;

	// Increase the triangle count
	g_Object.m_numOfTriangles++;
}