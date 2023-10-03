#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "ZoomPan.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

ZoomPan::ZoomPan(StateMachine& machine) : State(machine, CurrentState::ZOOMPAN) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 10.0f), Vector3f(0.0f, 0.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));	

	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(400.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);

	m_fXOffset = 0.46193072;
	m_fYOffset = 0.24252957;
	m_fZoomWidth = 0.14254469;
	m_fZoomHeight = 0.14254469;

	m_zoomableQuad.mapBuffer(m_fXOffset, m_fYOffset, m_fZoomWidth, m_fZoomHeight);

	m_shader = new Shader();
	m_shader->attachShader(Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/quad.vert"), true);
	m_shader->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/quad.frag"), false);
	m_shader->linkShaders();
	m_pShader = m_shader;

	m_pTexture = &Globals::textureManager.get("flower");
}

ZoomPan::~ZoomPan() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void ZoomPan::fixedUpdate() {

}

void ZoomPan::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f directrion = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;
	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, -1.0f, 0.0f);
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
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);

		}

		if (move) {
			m_camera.move(directrion * m_dt);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	m_background.update(m_dt);
}

void ZoomPan::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//m_background.draw();
	DrawZoomedImage();
	DrawActualImage();
	
	if (m_drawUi)
		renderUi();
}

void ZoomPan::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);

	static int startX = 0, startY = 0;
	static bool bMoveStart = false;
	if (event.button == 1u && !bMoveStart && WithinImageArea(event.x, event.y)){
		bMoveStart = true;
		startX = event.x;
		startY = event.y;

	}else if (bMoveStart && event.button == 1u) {
		// Move 
		int nXDiff = -(event.x - startX);
		int nYDiff = event.y - startY;
		startX = event.x;
		startY = event.y;

		// If mouse move in X direction
		if (nXDiff){
			float fOldX = m_fXOffset;
			m_fXOffset += (static_cast<float>(nXDiff) * 0.75f)/ static_cast<float>(Globals::textureManager.get("flower").getWidth());

			// If new change display data outside of texture, then reset to old value.
			if (m_fXOffset < 0.0){
				m_fXOffset = 0.0;
			}
			if (m_fXOffset + m_fZoomWidth > 1.0){
				m_fXOffset = fOldX;
			}
		}

		// If mouse move in Y direction
		if (nYDiff){
			float fOldY = m_fYOffset;
			m_fYOffset += (static_cast<float>(nYDiff) * 0.75f) / static_cast<float>(Globals::textureManager.get("flower").getHeight());

			// If new change display data outside the texture, reset to old value.
			if (m_fYOffset < 0.0){
				m_fYOffset = 0.0;
			}

			if (m_fYOffset + m_fZoomHeight > 1.0){
				m_fYOffset = fOldY;
			}
		}

		// Cretae new vertex buffer for zoomed texture mapping.
		m_zoomableQuad.mapBuffer(m_fXOffset, m_fYOffset, m_fZoomWidth, m_fZoomHeight);

	}else{
		bMoveStart = false;

		// Check mouse area is within image area then, set Arrow cursor.
		if (WithinImageArea(event.x, event.y)){
			Application::SetCursorIcon(IDC_SIZEALL);
		}else{
			Application::SetCursorIcon(IDC_ARROW);
		}
	}
}

void ZoomPan::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}
	else if (event.button == 2u) {
		//m_drawUi = false;
		Mouse::instance().attach(Application::GetWindow());
	}
}

void ZoomPan::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}
	else if (event.button == 2u) {
		//m_drawUi = true;
		Mouse::instance().detach();
	}
}

void ZoomPan::OnMouseWheel(Event::MouseWheelEvent& event) {
	float fZoomValue = static_cast<float>(event.delta);
	HandleZoom(fZoomValue);
}

void ZoomPan::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void ZoomPan::OnKeyUp(Event::KeyboardEvent& event) {

}

void ZoomPan::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void ZoomPan::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void ZoomPan::renderUi() {
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

	int currentFilter = filter;
	if (ImGui::Combo("Filter", &currentFilter, "Nearest\0Linear\0\0")) {
		filter = static_cast<Filter>(currentFilter);
		switch (filter) {
			case NEAREST:
				Globals::textureManager.get("flower").setNearest();
				break;
			case LINEAR:
				Globals::textureManager.get("flower").setLinear(GL_LINEAR);
				break;
		}
	}

	int currentMode = mode;
	if (ImGui::Combo("Mode", &currentMode, "None\0Bilinear\0Bicubic Triangular\0Bicubic Bell Shaped\0Bicubic B Spline\0Catmull Rom Spline\0\0")) {
		mode = static_cast<Mode>(currentMode);

		switch (mode){
			case NONE:			
				m_shader->attachShader(Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/quad.vert"), true);
				m_shader->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/quad.frag"), false);
				m_shader->linkShaders();
				m_pShader = m_shader;
				break;
			case BILINEAR:
				m_shader->attachShader(Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/quad.vert"), true);
				m_shader->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/pixel_shader_BiLinear.cg"), false);
				m_shader->linkShaders();
				m_pShader = m_shader;
				break;
			case BICUBIC_TRIANGULAR:
				m_shader->attachShader(Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/quad.vert"), true);
				m_shader->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/pixel_shader_BiCubic.cg"), false);
				m_shader->linkShaders();
				m_pShader = m_shader;
				break;
			case BICUBIC_BELL_SHAPED:
				m_shader->attachShader(Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/quad.vert"), true);
				m_shader->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/pixel_shader_Bell.cg"), false);
				m_shader->linkShaders();
				m_pShader = m_shader;
				break;
			case BICUBIC_B_SPLINE:
				m_shader->attachShader(Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/quad.vert"), true);
				m_shader->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/pixel_shader_BSpline.cg"), false);
				m_shader->linkShaders();
				m_pShader = m_shader;
				break;
			case CATMULL_ROM_SPLINE:
				m_shader->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/quad.vert"), true);
				m_shader->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/pixel_shader_CatMull.cg"), false);
				m_shader->linkShaders();
				m_pShader = m_shader;
				break;
			default:
				break;
		}
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ZoomPan::DrawZoomedImage() {
	glViewport(0, 0, 800, 600);

	m_pShader->use();
	m_pShader->loadInt("ImageTexture", 0);
	m_pShader->loadFloat("fWidth", static_cast<float>(m_pTexture->getWidth()));
	m_pShader->loadFloat("fHeight", static_cast<float>(m_pTexture->getHeight()));
	m_pTexture->bind();
	m_zoomableQuad.drawRaw();
	m_pTexture->unbind();
	m_pShader->unuse();
}

void ZoomPan::DrawActualImage() {
	// Set Rendering area of Actual image.
	glViewport(805, 10, 200, 150);

	// Image is attached.
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadBool("u_flip", false);
	Globals::textureManager.get("flower").bind();
	Globals::shapeManager.get("quad").drawRaw();
	Globals::textureManager.get("flower").unbind();
	shader->unuse();

	// Set Red color for Zoom area indication.
	glColor3f(1.0, 0.0, 0.0);

	float fXStart = m_fXOffset * 2;
	float fYStart = m_fYOffset * 2;
	float fWidth = m_fZoomWidth * 2;
	float fHeight = m_fZoomHeight * 2;

	// Draw a rectangle indicate zoom area.
	glBegin(GL_LINE_LOOP);
	glVertex2d(-1.0 + fXStart, -1.0 + fYStart);
	glVertex2d(-1.0 + fXStart + fWidth, -1.0 + fYStart);
	glVertex2d(-1.0 + fXStart + fWidth, -1.0 + fYStart + fHeight);
	glVertex2d(-1.0 + fXStart, -1.0 + fYStart + fHeight);
	glVertex2d(-1.0 + fXStart, -1.0 + fYStart);
	glEnd();

	glColor3f(0.0, 0.0, 1.0);

	glBegin(GL_LINE_LOOP);

	static float fPixel = 1.0 / 600.0;
	glVertex2d(-1 + fPixel, -1 + fPixel);
	glVertex3f(1, -1 + fPixel, 0.0);

	glVertex3f(1, 1, 0.0);
	glVertex3f(-1, 1, 0.0);
	glVertex3f(-1 + fPixel, -1, 0.0);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
}

bool ZoomPan::WithinImageArea(int posX, int posY) {
	return(posX > 0 && posX < 800 && posY > 0 && (Application::Height - posY) < 600);
}

void ZoomPan::HandleZoom(const short delta) {
	float fZoomXIncr = m_fZoomHeight * 0.025 * 3.0f;
	float fZoomYIncr = m_fZoomWidth  * 0.025 * 3.0f;

	// Zoom
	if (delta < 0){

		/*
		Increase of Zoomed area in Left and Right will be same.
		That is implemented through decrementing offset and increasing the width.
		In effect equal increase in X and Y directional will be achieved.
		*/
		m_fXOffset -= fZoomXIncr / 2;
		m_fYOffset -= fZoomYIncr / 2;

		m_fZoomHeight += fZoomYIncr;
		m_fZoomWidth += fZoomXIncr;

		// Validating new zoom region.
		if (m_fYOffset + m_fZoomHeight > 1.0 || m_fXOffset + m_fZoomWidth > 1.0 ||
			m_fYOffset < 0.0 || m_fXOffset < 0.0)
		{
			// Revert the change.
			m_fXOffset += fZoomXIncr / 2;
			m_fYOffset += fZoomYIncr / 2;

			m_fZoomHeight -= fZoomYIncr;
			m_fZoomWidth -= fZoomXIncr;
		}
	}else{
		// Zoom
		m_fXOffset += fZoomXIncr / 2;
		m_fYOffset += fZoomYIncr / 2;

		m_fZoomHeight -= fZoomYIncr;
		m_fZoomWidth -= fZoomXIncr;

		// Validating new zoom region.
		// Unzooom is possible from a correct zoom region therefore 
		// validating the minimum size is OK.
		if (m_fZoomWidth < 0.005)
		{
			m_fZoomWidth = 0.005;
		}
		if (m_fZoomHeight < 0.005)
		{
			m_fZoomHeight = 0.005;
		}
	}

	m_zoomableQuad.mapBuffer(m_fXOffset, m_fYOffset, m_fZoomWidth, m_fZoomHeight);
}