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

bool CreateTexture(GLuint &textureID, LPTSTR szFileName)                          // Creates Texture From A Bitmap File
{
	HBITMAP hBMP;                                                                 // Handle Of The Bitmap
	BITMAP  bitmap;																  // Bitmap Structure

	glGenTextures(1, &textureID);                                                 // Create The Texture
	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), szFileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	if (!hBMP)                                                                    // Does The Bitmap Exist?
		return FALSE;                                                           // If Not Return False

	GetObject(hBMP, sizeof(bitmap), &bitmap);                                     // Get The Object
																				  // hBMP:        Handle To Graphics Object
																				  // sizeof(bitmap): Size Of Buffer For Object Information
																				  // &bitmap:        Buffer For Object Information

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);                                        // Pixel Storage Mode (Word Alignment / 4 Bytes)

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, textureID);                                      // Bind To The Texture ID
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);			  // Linear Min Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);			  // Linear Mag Filter
	glTexImage2D(GL_TEXTURE_2D, 0, 3, bitmap.bmWidth, bitmap.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap.bmBits);
	// MUST NOT BE INDEX BMP, BUT RGB
	DeleteObject(hBMP);                                                           // Delete The Object

	return TRUE;                                                                  // Loading Was Successful
}


void DrawVertexArrays()
{
	// Create a static rotating variable for rotating around the y-axis
	static float rotateY = 0;

	// Bind our first texture map to the triangles
	glBindTexture(GL_TEXTURE_2D, g_Texture[1]);

	// Below, what we do is not render all of the triangles at a time,
	// but in order to rotate and translate our triangles we brake up
	// the calls the glDrawArrays().  This is also to show functionality.

	// Push on a new matrix so we don't effect the other triangles, then
	// rotate the first 3 triangles (9 vertices) around the y-axis.
	glPushMatrix();
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 0, 9);
	glPopMatrix();

	// Move the next 3 rotating triangles over to the right.  Notice that we start
	// from 9 and then render 9 indices (9 vertices).
	glPushMatrix();
	glTranslatef(8.0f, 0.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 9, 9);
	glPopMatrix();

	// Move 3 more rotating triangles over to the left (starting from the 18th index)
	glPushMatrix();
	glTranslatef(-8.0f, 0.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 18, 9);
	glPopMatrix();

	// Move 3 more rotating triangles down
	glPushMatrix();
	glTranslatef(0.0f, -8.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 27, 9);
	glPopMatrix();

	// Move 3 more rotating triangles up
	glPushMatrix();
	glTranslatef(0.0f, 8.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 36, 9);
	glPopMatrix();

	// Increase our rotation angle
	rotateY += 3;
}

///////////////////////////// DRAW INDEXED VERTEX ARRAYS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws triangles using vertex arrays with glDrawElements()
/////
///////////////////////////// DRAW INDEXED VERTEX ARRAYS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawIndexedVertexArrays()
{
	// Create a static rotating angle for the y-axis
	static float rotateY = 0;

	// Bind the second texture to these triangles
	glBindTexture(GL_TEXTURE_2D, g_Texture[2]);

	// Here we push on a matrix to flip the triangles upside down
	glPushMatrix();

	// Flip the triangles upside down
	glRotatef(180, 1.0f, 0.0f, 0.0f);

	// Draw 3 rotating triangles by passing in our indices and only rendering 9 vertices
	glPushMatrix();
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, g_Object.m_pIndices);
	glPopMatrix();

	// Move another rotating triangle group over to the right.  Notice that we
	// pass in the address to the 9th index of our object.  This makes sure we
	// pass in the next 9 indices in our vertex arrays.
	glPushMatrix();
	glTranslatef(8.0f, 0.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, &g_Object.m_pIndices[9]);
	glPopMatrix();

	// Move another rotating triangle group over to the left.
	glPushMatrix();
	glTranslatef(-8.0f, 0.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, &g_Object.m_pIndices[18]);
	glPopMatrix();

	// Move another rotating triangle group down.
	glPushMatrix();
	glTranslatef(0.0f, -8.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, &g_Object.m_pIndices[27]);
	glPopMatrix();

	// Move another rotating triangle group up.
	glPushMatrix();
	glTranslatef(0.0f, 8.0f, 0.0f);
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, &g_Object.m_pIndices[36]);
	glPopMatrix();

	// Go back to normal before we flipped the triangles upside down
	glPopMatrix();

	// Increase the rotation angle
	rotateY += 1;
}

///////////////////////////// DRAW INTERLEAVED VERTEX ARRAYS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws triangles using vertex arrays with glInterleavedArrays()
/////
///////////////////////////// DRAW INTERLEAVED VERTEX ARRAYS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawInterleavedVertexArrays()
{
	// Create a static rotation angle for the y-axis
	static float rotateY = 0;

	// Bind the third texture for these triangles
	glBindTexture(GL_TEXTURE_2D, g_Texture[3]);

	// Push on a new matrix to do some translation and rotations.  We want to rotate
	// a group of 3 triangles around the whole group of spinning triangles.  Notice that
	// we tell OpenGL that we want to pass in a structure that stores our texture and
	// vertex data (in that exact order), then we draw the vertex interleaved arrays
	// with glDrawArrays().
	glPushMatrix();
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, -10.0f);
	glInterleavedArrays(GL_T2F_V3F, sizeof(CVertex), &g_InterleavedVertices);
	glDrawArrays(GL_TRIANGLES, 0, 9);
	glPopMatrix();

	// Increase our rotation angle
	rotateY += 2;
}

///////////////////////////// DRAW ARRAY ELEMENT VERTEX ARRAYS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws triangles using vertex arrays with glArrayElement()
/////
///////////////////////////// DRAW ARRAY ELEMENT VERTEX ARRAYS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawArrayElementVertexArrays()
{
	// Create a static rotation angle for the y-axis
	static float rotateY = 0;

	// Bind the fourth texture for these triangles
	glBindTexture(GL_TEXTURE_2D, g_Texture[3]);

	// Push on a new matrix to do some rotations and translations around the other
	// groups of triangles.  For rendering vertex arrays with glArrayElement() we
	// just go through 9 vertices of our vertex arrays inside of a glBegin()/glEnd()
	// scope.
	glPushMatrix();

	// Rotate around the axis and move away from the origin
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 10.0f);

	// Draw 9 vertices from our vertex array
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 9; i++)
		glArrayElement(i);
	glEnd();

	glPopMatrix();

	// Increase the rotation angle
	rotateY += 2;
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *



///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void SolidState::RenderScene(){
	

}


void AddTriangle(float xOffset, float yOffset, float zOffset)
{
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
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 5.0f), Vector3f(0.0f, 0.0f, 5.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(5.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;

	solidConverter.solidToBuffer("res/Body.solid", true, { 180.0f, 0.0f, 0.0f }, { 0.04f, 0.04f, 0.04f }, vertexBuffer, indexBuffer);
	//solidConverter.loadSkeleton("res/BasicFigure", vertexBuffer);
	m_body.fromBuffer(vertexBuffer, indexBuffer, 5);
	m_skeleton.Load("res/BasicFigure", "res/BasicFigureLow" , "res/RabbitBelt",
                    "res/Body.solid" , "res/Body2.solid",
                    "res/Body3.solid", "res/Body4.solid",
                    "res/Body5.solid", "res/Body6.solid",
                    "res/Body7.solid", "res/BodyLow.solid",
                    "res/Belt.solid", true);

	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);


	CreateTexture(g_Texture[1], "Texture1.bmp");
	CreateTexture(g_Texture[2], "Texture2.bmp");
	CreateTexture(g_Texture[3], "Texture3.bmp");
	CreateTexture(g_Texture[4], "Texture4.bmp");

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

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, g_Object.m_pTextureCoords);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, g_Object.m_pVertices);

	DrawVertexArrays();
	DrawIndexedVertexArrays();
	DrawInterleavedVertexArrays();
	DrawArrayElementVertexArrays();

	/*float data[] = { -29.0f,  -158.0f, 8.99998f,-37.0f, -168.0f, 2.99999f,-34.0f,  -157.0f, 2.99999f};

	glEnableClientState(GL_VERTEX_ARRAY);
	glInterleavedArrays(GL_V3F, 0, data);
	glDrawArrays(GL_TRIANGLES, 0, 3);*/

	//m_skeleton.drawmodel.draw();

	/*glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 0.0f);	

	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-29.0f,  -158.0f, 8.99998f);
	glVertex3f(-37.0f, -168.0f, 2.99999f);
	glVertex3f(-34.0f,  -157.0f, 2.99999f);
	glEnd();

	
	
	glColor3f(1.0, 1.0, 0.0);*/

	//glEnableClientState(GL_COLOR_ARRAY);
	//glEnableClientState(GL_VERTEX_ARRAY);

	//glInterleavedArrays(GL_V3F, sizeof(float) * 3, data);
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	/*auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadInt("u_texture", 1);
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	Globals::textureManager.get("null").bind(1);
	m_body.drawRaw();
	shader->unuse();*/

	//if (m_drawUi)
		//renderUi();
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