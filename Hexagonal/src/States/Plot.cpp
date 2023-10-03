#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "Plot.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

Plot::Plot(StateMachine& machine) : State(machine, CurrentState::PLOT) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);

	// Create the vertex buffer object
	glGenBuffers(3, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	// Create our own temporary buffer
	point graph[2000];

	// Fill it in just like an array
	for (int i = 0; i < 2000; i++) {
		float x = (i - 1000.0) / 100.0;

		graph[i].x = x;
		graph[i].y = sin(x * 10.0) / (1.0 + x * x);
	}

	// Tell OpenGL to copy our array to the buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof graph, graph, GL_STATIC_DRAW);

	// Create a VBO for the border
	static const point border[4] = { { -1, -1 },{ 1, -1 },{ 1, 1 },{ -1, 1 } };
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof border, border, GL_STATIC_DRAW);

	m_shader = new Shader("res/graph.v.glsl", "res/graph.f.glsl");
}

Plot::~Plot() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Plot::fixedUpdate() {

}

void Plot::update() {
	Keyboard &keyboard = Keyboard::instance();
	
	if (keyboard.keyPressed(Keyboard::KEY_W)) {
		scale_x *= 1.5;
	}

	if (keyboard.keyPressed(Keyboard::KEY_S)) {
		scale_x /= 1.5;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		offset_x += 1.0f * m_dt;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		offset_x -= 1.0f * m_dt;
	}

	if (keyboard.keyDown(Keyboard::KEY_HOME)) {
		offset_x = 0.0;
		scale_x = 1.0;
	}
}

void Plot::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//m_background.draw();
	/* Draw the graph */
	glViewport(border + ticksize, border + ticksize, Application::Width - border * 2 - ticksize, Application::Height - border * 2 - ticksize);

	// Set the scissor rectangle,this will clip fragments
	glScissor(border + ticksize, border + ticksize, Application::Width - border * 2 - ticksize, Application::Height - border * 2 - ticksize);

	glEnable(GL_SCISSOR_TEST);

	// Set our coordinate transformation matrix
	Matrix4f transform = Matrix4f::Translate(offset_x, 0.0f, 0.0f) * Matrix4f::Scale(scale_x, 1.0f, 1.0f);
	m_shader->use();
	m_shader->loadMatrix("transform", transform);
	m_shader->loadVector("color", Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

	
	// Draw using the vertices in our vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINE_STRIP, 0, 2000);

	// Stop clipping
	glViewport(0, 0, Application::Width, Application::Height);
	glDisable(GL_SCISSOR_TEST);
	
	/* Draw the borders */

	float pixel_x, pixel_y;

	// Calculate a transformation matrix that gives us the same normalized device coordinates as above
	transform = viewportTransform(border + ticksize, border + ticksize, Application::Width - border * 2 - ticksize, Application::Height - border * 2 - ticksize, &pixel_x, &pixel_y);
	m_shader->loadMatrix("transform", transform);
	// Tell our vertex shader about it
	// Set the color to black
	m_shader->loadVector("color", Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

	// Draw a border around our graph
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

	
	/* Draw the y tick marks */

	point ticks[42];

	for (int i = 0; i <= 20; i++) {
		float y = -1 + i * 0.1;
		float tickscale = (i % 10) ? 0.5 : 1;

		ticks[i * 2].x = -1;
		ticks[i * 2].y = y;
		ticks[i * 2 + 1].x = -1 - ticksize * tickscale * pixel_x;
		ticks[i * 2 + 1].y = y;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof ticks, ticks, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 42);

	/* Draw the x tick marks */

	float tickspacing = 0.1 * powf(10, -floor(log10(scale_x)));	// desired space between ticks, in graph coordinates
	float left = -1.0 / scale_x - offset_x;	// left edge, in graph coordinates
	float right = 1.0 / scale_x - offset_x;	// right edge, in graph coordinates
	int left_i = ceil(left / tickspacing);	// index of left tick, counted from the origin
	int right_i = floor(right / tickspacing);	// index of right tick, counted from the origin
	float rem = left_i * tickspacing - left;	// space between left edge of graph and the first tick

	float firsttick = -1.0 + rem * scale_x;	// first tick in device coordinates

	int nticks = right_i - left_i + 1;	// number of ticks to show

	if (nticks > 21)
		nticks = 21;	// should not happen

	for (int i = 0; i < nticks; i++) {
		float x = firsttick + i * tickspacing * scale_x;
		float tickscale = ((i + left_i) % 10) ? 0.5 : 1;

		ticks[i * 2].x = x;
		ticks[i * 2].y = -1;
		ticks[i * 2 + 1].x = x;
		ticks[i * 2 + 1].y = -1 - ticksize * tickscale * pixel_y;
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof ticks, ticks, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, nticks * 2);

	// And we are done.

	glDisableVertexAttribArray(0);

	//m_background.draw();

	//if (m_drawUi)
		//renderUi();
}

void Plot::OnMouseMotion(Event::MouseMoveEvent& event) {
	
}

void Plot::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {

	}else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Plot::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		
	}else if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Plot::OnMouseWheel(Event::MouseWheelEvent& event) {
	
}

void Plot::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void Plot::OnKeyUp(Event::KeyboardEvent& event) {

}

void Plot::resize(int deltaW, int deltaH) {

}

void Plot::renderUi() {
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
		ImGui::DockBuilderDockWindow("Settings", dock_id_right);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

Matrix4f Plot::viewportTransform(float x, float y, float width, float height, float *pixel_x, float *pixel_y) {
	// Map OpenGL coordinates (-1,-1) to window coordinates (x,y),
	// (1,1) to (x + width, y + height).

	// First, we need to know the real window size:
	float window_width = static_cast<float>(Application::Width);
	float window_height = static_cast<float>(Application::Height);

	// Calculate how to translate the x and y coordinates:
	float offset_x = (2.0 * x + (width - window_width)) / window_width;
	float offset_y = (2.0 * y + (height - window_height)) / window_height;

	// Calculate how to rescale the x and y coordinates:
	float scale_x = width / window_width;
	float scale_y = height / window_height;

	// Calculate size of pixels in OpenGL coordinates
	if (pixel_x)
		*pixel_x = 2.0 / width;
	if (pixel_y)
		*pixel_y = 2.0 / height;

	return Matrix4f::Scale(scale_x, scale_y, 1.0f) * Matrix4f::Translate(offset_x, offset_y, 0.0f);
}