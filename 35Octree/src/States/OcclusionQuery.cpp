#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <engine/Batchrenderer.h>
#include <engine/Fontrenderer.h>
#include <States/Menu.h>

#include "OcclusionQuery.h"
#include "Application.h"
#include "Globals.h"

Vector3f vCubeVertices[36] =
{
	// Front face
	Vector3f(-0.5f, 0.5f, 0.5f), Vector3f(0.5f, 0.5f, 0.5f), Vector3f(0.5f, -0.5f, 0.5f), Vector3f(0.5f, -0.5f, 0.5f), Vector3f(-0.5f, -0.5f, 0.5f), Vector3f(-0.5f, 0.5f, 0.5f),
	// Back face
	Vector3f(0.5f, 0.5f, -0.5f), Vector3f(-0.5f, 0.5f, -0.5f), Vector3f(-0.5f, -0.5f, -0.5f), Vector3f(-0.5f, -0.5f, -0.5f), Vector3f(0.5f, -0.5f, -0.5f), Vector3f(0.5f, 0.5f, -0.5f),
	// Left face
	Vector3f(-0.5f, 0.5f, -0.5f), Vector3f(-0.5f, 0.5f, 0.5f), Vector3f(-0.5f, -0.5f, 0.5f), Vector3f(-0.5f, -0.5f, 0.5f), Vector3f(-0.5f, -0.5f, -0.5f), Vector3f(-0.5f, 0.5f, -0.5f),
	// Right face
	Vector3f(0.5f, 0.5f, 0.5f), Vector3f(0.5f, 0.5f, -0.5f), Vector3f(0.5f, -0.5f, -0.5f), Vector3f(0.5f, -0.5f, -0.5f), Vector3f(0.5f, -0.5f, 0.5f), Vector3f(0.5f, 0.5f, 0.5f),
	// Top face
	Vector3f(-0.5f, 0.5f, -0.5f), Vector3f(0.5f, 0.5f, -0.5f), Vector3f(0.5f, 0.5f, 0.5f), Vector3f(0.5f, 0.5f, 0.5f), Vector3f(-0.5f, 0.5f, 0.5f), Vector3f(-0.5f, 0.5f, -0.5f),
	// Bottom face
	Vector3f(-0.5f, -0.5f, 0.5f), Vector3f(0.5f, -0.5f, 0.5f), Vector3f(0.5f, -0.5f, -0.5f), Vector3f(0.5f, -0.5f, -0.5f), Vector3f(-0.5f, -0.5f, -0.5f), Vector3f(-0.5f, -0.5f, 0.5f),
};
Vector2f vCubeTexCoords[6] = { Vector2f(0.0f, 1.0f), Vector2f(1.0f, 1.0f), Vector2f(1.0f, 0.0f), Vector2f(1.0f, 0.0f), Vector2f(0.0f, 0.0f), Vector2f(0.0f, 1.0f) };

Vector3f vCubeNormals[6] =
{
	Vector3f(0.0f, 0.0f, 1.0f),
	Vector3f(0.0f, 0.0f, -1.0f),
	Vector3f(-1.0f, 0.0f, 0.0f),
	Vector3f(1.0f, 0.0f, 0.0f),
	Vector3f(0.0f, 1.0f, 0.0f),
	Vector3f(0.0f, -1.0f, 0.0f)
};

Vector3f vGround[6] =
{
	Vector3f(-300.0f, 0.0f, -300.0f), Vector3f(300.0f, 0.0f, 300.0f), Vector3f(300.0f, 0.0f, -300.0f), Vector3f(300.0f, 0.0f, 300.0f), Vector3f(-300.0f, 0.0f, -300.0f), Vector3f(-300.0f, 0.0f, 300.0f)
};

Vector2f vGroundTexCoords[6] =
{
	Vector2f(0.0f, 10.0f), Vector2f(10.0f, 10.0f), Vector2f(10.0f, 0.0f), Vector2f(10.0f, 0.0f), Vector2f(0.0f, 0.0f), Vector2f(0.0f, 10.0f)
};

OcclusionQuery::OcclusionQuery(StateMachine& machine) : State(machine, States::DEFAULT) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 20.0f, 150.0f), Vector3f(0.0f, 20.0f, 150.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(30.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	prepareStaticSceneObjects();
	glGenQueries(1, &m_occlusionQuery);

	
}

OcclusionQuery::~OcclusionQuery() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	glDeleteVertexArrays(1, &m_vao);
	m_vao = 0;
	glDeleteBuffers(1, &m_vbo);
	m_vbo = 0;

	glDeleteVertexArrays(1, &m_vaoOcc);
	m_vaoOcc = 0;
	glDeleteBuffers(1, &m_vboOcc);
	m_vboOcc = 0;

	glDeleteQueries(1, &m_occlusionQuery);
}


void OcclusionQuery::fixedUpdate() {

}

void OcclusionQuery::update() {
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

	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			for (int z = 0; z < 3; z++) {
				float fLocalRotAngle = m_globalAngle + x * 60.0f + y * 20.0f + z * 6.0f;					
				Vector3f vOcclusionCubePos = Vector3f(-fCubeHalfSize + fCubeHalfSize * x*2.0f / 3.0f + fCubeHalfSize / 3.0f, -fCubeHalfSize + fCubeHalfSize * y*2.0f / 3.0f + fCubeHalfSize / 3.0f, -fCubeHalfSize + fCubeHalfSize * z*2.0f / 3.0f + fCubeHalfSize / 3.0f);
				m_modelMatrices[x][y][z] = Matrix4f::Translate(Vector3f(0.0f, fCubeHalfSize, 0.0f) + vOcclusionCubePos);
				m_modelMatrices[x][y][z] *= Matrix4f::Rotate(Vector3f(1.0f, 0.0f, 0.0f), fLocalRotAngle);
				m_modelMatrices[x][y][z] *= Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), fLocalRotAngle);
				m_modelMatrices[x][y][z] *= Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), fLocalRotAngle);
				m_renderSphere[x][y][z] = false;
			}
		}
	}
	iSpheresPassed = 0;
	m_globalAngle += (45.0f * m_dt);
}

void OcclusionQuery::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_skybox.draw(m_camera, "calm");

	glBindVertexArray(m_vao);

	// Render ground
	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	
	Globals::textureManager.get("dirt").bind();
	glDrawArrays(GL_TRIANGLES, 36, 6);

	glDisable(GL_CULL_FACE);
	shader->loadMatrix("u_model", Matrix4f::Translate(0.0f, 30.0f, 0.0f));
	Globals::textureManager.get("metal").bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_CULL_FACE);

	shader = Globals::shaderManager.getAssetPointer("occlusion");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadVector("u_color", Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

	glBindVertexArray(m_vaoOcc);
	// Occlusion query begins here
	// First of all, disable writing to the color buffer and depth buffer. We just wanna check if they would be rendered, not actually render them
	glColorMask(false, false, false, false);
	glDepthMask(GL_FALSE);
	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			for (int z = 0; z < 3; z++) {
				
				if (m_enableOcclusionQuery){
					shader->loadMatrix("u_model", m_modelMatrices[x][y][z] * Matrix4f::Scale(fCubeHalfSize / 3.0f, fCubeHalfSize / 3.0f, fCubeHalfSize / 3.0f));
					glBeginQuery(GL_SAMPLES_PASSED, m_occlusionQuery);
					glDrawArrays(GL_TRIANGLES, 0, 36);
					glEndQuery(GL_SAMPLES_PASSED);
					int iSamplesPassed = 0;
					glGetQueryObjectiv(m_occlusionQuery, GL_QUERY_RESULT, &iSamplesPassed);
			
					if (iSamplesPassed > 0){
						m_renderSphere[x][y][z] = true;
						iSpheresPassed++;
					}
				}else{
					m_renderSphere[x][y][z] = true;
					iSpheresPassed++;
				}
			}
		}
	}
	glColorMask(true, true, true, true);
	glDepthMask(GL_TRUE);

	if (m_showOccluders){
		for (int x = 0; x < 3; x++) {
			for (int y = 0; y < 3; y++) {
				for (int z = 0; z < 3; z++) {
					shader->loadMatrix("u_model", m_modelMatrices[x][y][z] * Matrix4f::Scale(fCubeHalfSize / 3.0f, fCubeHalfSize / 3.0f, fCubeHalfSize / 3.0f));
					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
		}
		glBindVertexArray(0);
		shader->unuse();
	}else{
		glBindVertexArray(m_vao);
		shader = Globals::shaderManager.getAssetPointer("texture");
		shader->use();
		Globals::textureManager.get("wood").bind();
		for (int x = 0; x < 3; x++) {
			for (int y = 0; y < 3; y++) {
				for (int z = 0; z < 3; z++) {
					if (m_renderSphere[x][y][z] == false)
						continue;
					shader->loadMatrix("u_model", m_modelMatrices[x][y][z]);
					Globals::shapeManager.get("sphere_occ").drawRaw();
				}
			}
		}
		glBindVertexArray(0);
		shader->unuse();
	}

	Globals::fontManager.get("arial_20").bind(0);
	Fontrenderer::Get().addText(Globals::fontManager.get("arial_20"), Application::Width - 270 , Application::Height - 50, "Spheres Rendered: " + std::to_string(iSpheresPassed) + " / 27", Vector4f(0.9f, 0.9f, 0.9f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	if (m_drawUi)
		renderUi();
}

void OcclusionQuery::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void OcclusionQuery::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void OcclusionQuery::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void OcclusionQuery::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void OcclusionQuery::OnKeyDown(Event::KeyboardEvent& event) {
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

void OcclusionQuery::OnKeyUp(Event::KeyboardEvent& event) {

}

void OcclusionQuery::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void OcclusionQuery::renderUi() {
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
	ImGui::Checkbox("Show Occluders", &m_showOccluders);
	ImGui::Checkbox("Enable Occlusion Query", &m_enableOcclusionQuery);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OcclusionQuery::prepareStaticSceneObjects(){
	std::vector<Vertex> data;
	
	
	float fCubeHalfSize = 30.0f;
	
	Vector2f vTexCoords[] ={
		Vector2f(0.0f, fCubeHalfSize * 0.1f),
		Vector2f(fCubeHalfSize*0.1f, fCubeHalfSize*0.1f),
		Vector2f(fCubeHalfSize*0.1f, 0.0f),
		Vector2f(0.0f, 0.0f)
	};

	int indices[] = { 0, 3, 1, 1, 3, 2 };

	for(int i = 0; i < 2; i++){
		float fSign = i ? -1.0f : 1.0f;
		Vector3f vNormal(0.0f, 1.0f, 0.0f);
		Vector3f vQuad[] ={
			Vector3f(-fCubeHalfSize, -fCubeHalfSize + fCubeHalfSize * (i + 1)*2.0f / 3.0f, -fCubeHalfSize),
			Vector3f(fCubeHalfSize, -fCubeHalfSize + fCubeHalfSize * (i + 1)*2.0f / 3.0f, -fCubeHalfSize),
			Vector3f(fCubeHalfSize, -fCubeHalfSize + fCubeHalfSize * (i + 1)*2.0f / 3.0f, fCubeHalfSize),
			Vector3f(-fCubeHalfSize, -fCubeHalfSize + fCubeHalfSize * (i + 1)*2.0f / 3.0f, fCubeHalfSize),
		};

		for (int j = 0; j < 6; j++){
			int k = indices[j];
			data.push_back({ vQuad[k], vTexCoords[k], vNormal });
		}
	}

	for (int i = 0; i < 2; i++) {
		float fSign = i ? -1.0f : 1.0f;
		Vector3f vNormal(1.0f, 0.0f, 0.0f);
		Vector3f vQuad[] =
		{
			Vector3f(-fCubeHalfSize + fCubeHalfSize * (i + 1)*2.0f / 3.0f, fCubeHalfSize, -fCubeHalfSize),
			Vector3f(-fCubeHalfSize + fCubeHalfSize * (i + 1)*2.0f / 3.0f, fCubeHalfSize, fCubeHalfSize),
			Vector3f(-fCubeHalfSize + fCubeHalfSize * (i + 1)*2.0f / 3.0f, -fCubeHalfSize, fCubeHalfSize),
			Vector3f(-fCubeHalfSize + fCubeHalfSize * (i + 1)*2.0f / 3.0f, -fCubeHalfSize, -fCubeHalfSize),
		};

		for (int j = 0; j < 6; j++) {
			int k = indices[j];
			data.push_back({ vQuad[k], vTexCoords[k], vNormal });
		}
	}


	for (int i = 0; i < 2; i++) {
		float fSign = i ? -1.0f : 1.0f;
		Vector3f vNormal(0.0f, 0.0f, 1.0f);
		Vector3f vQuad[] =
		{
			Vector3f(-fCubeHalfSize, fCubeHalfSize, -fCubeHalfSize + fCubeHalfSize * (i + 1)*2.0f / 3.0f),
			Vector3f(fCubeHalfSize, fCubeHalfSize, -fCubeHalfSize + fCubeHalfSize * (i + 1)*2.0f / 3.0f),
			Vector3f(fCubeHalfSize, -fCubeHalfSize, -fCubeHalfSize + fCubeHalfSize * (i + 1)*2.0f / 3.0f),
			Vector3f(-fCubeHalfSize, -fCubeHalfSize, -fCubeHalfSize + fCubeHalfSize * (i + 1)*2.0f / 3.0f),
		};

		for (int j = 0; j < 6; j++) {
			int k = indices[j];
			data.push_back({ vQuad[k], vTexCoords[k], vNormal });
		}
	}

	Vector3f vNormal(0.0f, 1.0f, 0.0f);
	for(int i = 0; i < 6; i++){
		data.push_back({ vGround[i], vGroundTexCoords[i], vNormal });
	}

	glGenVertexArrays(1, &m_vao);
	glGenVertexArrays(1, &m_vaoOcc);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_vboOcc);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Vector3f));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3f) + sizeof(Vector2f)));

	glBindVertexArray(m_vaoOcc);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboOcc);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(Vector3f), &vCubeVertices[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), 0);

	glBindVertexArray(0);
}