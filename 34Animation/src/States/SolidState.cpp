#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <engine/DebugRenderer.h>

#include "SolidState.h"
#include "Application.h"
#include "Globals.h"

CObject g_Object;
CVertex g_InterleavedVertices[9];
UINT g_Texture[MAX_TEXTURES];

bool CreateTexture(GLuint &textureID, LPTSTR szFileName) {
	HBITMAP hBMP;
	BITMAP  bitmap;	

	glGenTextures(1, &textureID);
	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), szFileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	if (!hBMP) 
		return FALSE;

	GetObject(hBMP, sizeof(bitmap), &bitmap);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, bitmap.bmWidth, bitmap.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap.bmBits);
	DeleteObject(hBMP);
	return TRUE;
}

void DrawVertexArrays(){
	static float rotateY = 0;

	glBindTexture(GL_TEXTURE_2D, g_Texture[0]);

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


void DrawIndexedVertexArrays(){

	static float rotateY = 0;

	glBindTexture(GL_TEXTURE_2D, g_Texture[1]);
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

void DrawInterleavedVertexArrays(){
	static float rotateY = 0;
	glBindTexture(GL_TEXTURE_2D, g_Texture[2]);
	glPushMatrix();
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, -10.0f);
	glInterleavedArrays(GL_T2F_V3F, sizeof(CVertex), &g_InterleavedVertices);
	glDrawArrays(GL_TRIANGLES, 0, 9);
	glPopMatrix();
	rotateY += 2;
}

void DrawArrayElementVertexArrays(){
	
	static float rotateY = 0;


	glBindTexture(GL_TEXTURE_2D, g_Texture[3]);
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

void SolidState::RenderScene(){
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, g_Object.m_pTextureCoords);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, g_Object.m_pVertices);

	DrawVertexArrays();
	DrawIndexedVertexArrays();
	DrawInterleavedVertexArrays();
	DrawArrayElementVertexArrays();
}

void AddTriangle(float xOffset, float yOffset, float zOffset){

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


SolidState::SolidState(StateMachine& machine) : State(machine, States::SOLID) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 7.0f, 20.0f), Vector3f(0.0f, 7.0f, 20.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(5.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;

	solidConverter.solidToBuffer("res/Body.solid", true, { 180.0f, 0.0f, 0.0f }, { 0.04f, 0.04f, 0.04f }, vertexBuffer, indexBuffer);
	solidConverter.loadSkeleton("res/BasicFigure", vertexBuffer);
	std::cout << "######################" << std::endl;
	m_body.fromBuffer(vertexBuffer, indexBuffer, 5);
	m_skeleton.Load("res/BasicFigure", "res/BasicFigureLow" , "res/RabbitBelt",
                    "res/Body.solid" , "res/Body2.solid",
                    "res/Body3.solid", "res/Body4.solid",
                    "res/Body5.solid", "res/Body6.solid",
                    "res/Body7.solid", "res/BodyLow.solid",
                    "res/Belt.solid", true);

	CreateTexture(g_Texture[0], "Texture1.bmp");
	CreateTexture(g_Texture[1], "Texture2.bmp");
	CreateTexture(g_Texture[2], "Texture3.bmp");
	CreateTexture(g_Texture[3], "Texture4.bmp");

	g_Object.m_pTextureCoords = new CVector2[MAX_VERTICES];
	g_Object.m_pVertices = new CVector3[MAX_VERTICES];
	g_Object.m_pIndices = new UINT[MAX_VERTICES];

	for (int triangleGroup = 0; triangleGroup < 5; triangleGroup++)
	{
		AddTriangle(0, 0, 0);			// Make a centered triangle
		AddTriangle(-2.0f, -4.0f, 0);	// Make a triangle on the left
		AddTriangle(2.0f, -4.0f, 0);	// Make a triangle on the right
	}


	for (int i = 0; i < MAX_VERTICES; i++)
		g_Object.m_pIndices[i] = i;

	for (int n = 0; n < 9; n++)
	{
		g_InterleavedVertices[n].m_textureCoord = g_Object.m_pTextureCoords[n];
		g_InterleavedVertices[n].m_vertex = g_Object.m_pVertices[n];
	}
}

SolidState::~SolidState() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void SolidState::fixedUpdate() {

}

void SolidState::update() {
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();

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
}

void some_func1() {

}

void SolidState::render() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getPerspectiveMatrix()[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getViewMatrix()[0][0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//m_skeleton.model[1].draw();

	//m_skeleton.drawmodel.draw();
	
	renderModel();
	m_skeleton.drawmodel.draw();

	/*auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadInt("u_texture", 1);
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	Globals::textureManager.get("null").bind(1);
	m_body.drawRaw();
	shader->unuse();*/

	if (m_drawUi)
		renderUi();
}

void SolidState::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void SolidState::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void SolidState::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void SolidState::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void SolidState::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void SolidState::OnKeyUp(Event::KeyboardEvent& event) {

}

void SolidState::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void SolidState::renderUi() {
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

void SolidState::renderModel(){

	static int start, endthing;
	static float M[16];

	for (unsigned int i = 0; i < m_skeleton.muscles.size(); i++) {
		XYZ mid = (m_skeleton.muscles[i].parent1->position + m_skeleton.muscles[i].parent2->position) / 2;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glTranslatef(mid.x, mid.y, mid.z);
		glRotatef(m_skeleton.muscles[i].rotate1 - 90, 0, 1, 0);
		glRotatef(m_skeleton.muscles[i].rotate2 - 90, 0, 0, 1);
		glRotatef(m_skeleton.muscles[i].rotate3, 0, 1, 0);

		for (unsigned j = 0; j < m_skeleton.muscles[i].vertices.size(); j++) {
			XYZ& v0 = m_skeleton.model[0].vertex[m_skeleton.muscles[i].vertices[j]];
			
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glTranslatef(v0.x, v0.y, v0.z);

			glGetFloatv(GL_MODELVIEW_MATRIX, M);

			//std::cout << M[0] << "  " << M[1] << "  " << M[2] << "  " << M[3] << std::endl;
			//std::cout << M[4] << "  " << M[5] << "  " << M[6] << "  " << M[7] << std::endl;
			//std::cout << M[8] << "  " << M[9] << "  " << M[10] << "  " << M[11] << std::endl;
			//std::cout << M[12] << "  " << M[13] << "  " << M[14] << "  " << M[15] << std::endl;

			m_skeleton.drawmodel.vertex[m_skeleton.muscles[i].vertices[j]].x = M[12];
			m_skeleton.drawmodel.vertex[m_skeleton.muscles[i].vertices[j]].y = M[13];
			m_skeleton.drawmodel.vertex[m_skeleton.muscles[i].vertices[j]].z = M[14];
			glPopMatrix();
		}
		

		glPopMatrix();
	}

	m_skeleton.drawmodel.UpdateVertexArrayNoTexNoNorm();		
}