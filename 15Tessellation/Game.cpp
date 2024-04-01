#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "engine/input/EventDispatcher.h"

#include "Game.h"
#include "Application.h"
#include "teapot.h"

//#define USE_STRIPS

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(20.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 0.0f, -5.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-2.0f);
	applyTransformation(m_trackball);

	tess = new Shader("res/tess.vert", "res/tess.frag");
	stat = new Shader("res/static.vert", "res/static.frag");
	eva = new Shader("res/eva.vert", "res/eva.frag");
	m_teapot.loadModel("res/models/teapot.obj", false, false, true);

	initUniformBuffers();
	initPatchGeometry(m_tessLevel + 1);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
}

Game::~Game() {}

void Game::fixedUpdate() {

}

void Game::update() {
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
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotateSmoothly(dx, dy, 0.0f);

		}

		if (move) {
			m_camera.move(direction * 5.0f * m_dt);
		}
	}

	m_trackball.idle();
	applyTransformation(m_trackball);

};

void Game::render(unsigned int &frameBuffer) {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, m_wireframe ? GL_LINE : GL_FILL);
	if (renderMode == RenderMode::TESS) {
		glUseProgram(tess->m_program);
		tess->loadMatrix("u_projection", m_camera.getProjectionMatrix());
		tess->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
		tess->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
		tess->loadVector("u_viewPos", m_camera.getPosition());
		glBindBuffer(GL_ARRAY_BUFFER, s_vbHandle);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ibHandle);

#ifdef USE_STRIPS
		glDrawElementsInstanced(GL_TRIANGLE_STRIP, s_patchIndexCount, GL_UNSIGNED_INT, 0, c_teapotPatchCount);
#else
		glDrawElementsInstanced(GL_TRIANGLES, s_patchIndexCount, GL_UNSIGNED_INT, 0, c_teapotPatchCount);
#endif

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}

	if (renderMode == RenderMode::STATIC) {
		glUseProgram(stat->m_program);
		stat->loadMatrix("u_projection", m_camera.getProjectionMatrix());
		stat->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
		stat->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
		stat->loadVector("u_viewPos", m_camera.getPosition());

		m_teapot.drawRaw();
		glUseProgram(0);
	}

	if (renderMode == RenderMode::EVALUATERS) {
		glFrontFace(GL_CW);
		glUseProgram(eva->m_program);
		eva->loadMatrix("u_projection", m_camera.getProjectionMatrix());
		eva->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
		eva->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
		eva->loadVector("u_viewPos", m_camera.getPosition());
		
		glEnable(GL_AUTO_NORMAL);
		glEnable(GL_MAP2_VERTEX_3);
		//glEnable(GL_MAP2_NORMAL);
		//static float tex[4][2] = {{0,0}, {1,0}, {0,1}, {1,1}};
		
		// Draw teapot using evaluators.
		for (int p = 0; p < c_teapotPatchCount; p++) {
			GLfloat patch[16][3];
			
			for (int j = 0; j < 16; j++) {
				patch[j][0] = teapotVertices[3 * (teapotIndices[16 * p + j] - 1) + 0];
				patch[j][1] = teapotVertices[3 * (teapotIndices[16 * p + j] - 1) + 1];
				patch[j][2] = teapotVertices[3 * (teapotIndices[16 * p + j] - 1) + 2];

				
			}
			/*GLfloat normal[16][3];
			for (int j = 0; j < 16; j++) {
			
				normal[j][0] = 0.5f;
				normal[j][1] = 0.5f;
				normal[j][2] = 0.5f;
			}*/

			//glMap2d(GL_MAP2_TEXTURE_COORD_2, 0.0, 1.0, 2, 2, 0.0, 1.0, 4, 2, &tex[0][0]);
			glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4, 0.0, 1.0, 12, 4, &patch[0][0]);
			//glMap2f(GL_MAP2_NORMAL, 0.0, 1.0, 3, 4, 0.0, 1.0, 12, 4, &normal[0][0]);
			glMapGrid2d(m_tessLevel + 1, 0.0, 1.0, m_tessLevel + 1, 0.0, 1.0);
			glEvalMesh2(GL_FILL, 0, m_tessLevel + 1, 0, m_tessLevel + 1);
		}
		
		glDisable(GL_NORMALIZE);
		//glDisable(GL_MAP2_NORMAL);
		glUseProgram(0);
		glFrontFace(GL_CCW);
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (m_drawControl) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(&m_camera.getProjectionMatrix()[0][0]);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(&(m_camera.getViewMatrix() * m_transformControl.getTransformationMatrix())[0][0]);

		glColor3f(0.0f, 0.0f, 1.0f);

		glBegin(GL_LINES);

		for (int p = 0; p < c_teapotPatchCount; p++) {
			int v[16];
			for (int i = 0; i < 16; i++){
				v[i] = 3 * (teapotIndices[p * 16 + i] - 1);
			}

			for (int i = 0; i < 4; i++){
				glVertex3fv(teapotVertices + v[4 * i + 0]);
				glVertex3fv(teapotVertices + v[4 * i + 1]);

				glVertex3fv(teapotVertices + v[4 * i + 1]);
				glVertex3fv(teapotVertices + v[4 * i + 2]);

				glVertex3fv(teapotVertices + v[4 * i + 2]);
				glVertex3fv(teapotVertices + v[4 * i + 3]);

				glVertex3fv(teapotVertices + v[i + 0]);
				glVertex3fv(teapotVertices + v[i + 4]);

				glVertex3fv(teapotVertices + v[i + 4]);
				glVertex3fv(teapotVertices + v[i + 8]);

				glVertex3fv(teapotVertices + v[i + 8]);
				glVertex3fv(teapotVertices + v[i + 12]);
			}
		}

		glEnd();
	}
	renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {	
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {	
	m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {	
	m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::applyTransformation(TrackBall& arc) {
	m_transformControl.fromMatrix(arc.getTransform(-m_center, Quaternion(0.0f, -0.707107f, -0.707107f, 0.0f), Vector3f(0.1f, 0.1f, 0.1f)));

	if (renderMode == RenderMode::TESS || renderMode == RenderMode::EVALUATERS) {
		m_transform.fromMatrix(arc.getTransform(-m_center, Quaternion(0.0f, -0.707107f, -0.707107f, 0.0f), Vector3f(0.1f, 0.1f, 0.1f)));
	}

	if (renderMode == RenderMode::STATIC) {
		m_transform.fromMatrix(arc.getTransform(-m_teapot.getCenter(), Quaternion(0.0f, -1.0f, 0.0f, 0.0f), Vector3f(0.1f, 0.1f, 0.1f)));
		m_transform.translate(0.005f, 0.0f, 0.0f);
	}
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(20.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}


void Game::initUniformBuffers() {
	int c_teapotVertexCount = sizeof(teapotVertices) / (3 * sizeof(float));
	int c_teapotIndexCount = sizeof(teapotIndices) / sizeof(int);

	// create vertex buffer.
	GLint vlocation = glGetUniformLocation(tess->m_program, "u_vertices");
	GLint vsize = 4 * sizeof(float) * c_teapotIndexCount;
	
	// In the present implementation all constants must be 128-bit aligned, so an array of 
	// vec3s is padded to vec4s. A future OpenGL extension may remove this limitation.
	float * tmp = new float[c_teapotIndexCount * 4];
	for (int i = 0; i < c_teapotIndexCount; i++) {
		int idx = teapotIndices[i] - 1;
		tmp[4 * i + 0] = teapotVertices[3 * idx + 0];
		tmp[4 * i + 1] = teapotVertices[3 * idx + 1];
		tmp[4 * i + 2] = teapotVertices[3 * idx + 2];
		tmp[4 * i + 3] = 1.0f;    // pad
	}
	
	glGenBuffers(1, &viewUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, viewUbo);
	glBufferData(GL_UNIFORM_BUFFER, vsize, tmp, GL_STATIC_READ);
	glBindBufferRange(GL_UNIFORM_BUFFER, viewBinding, viewUbo, 0, vsize);
	//glUniformBlockBinding(tess->m_program, vlocation, viewBinding);

	delete[] tmp;
}

void Game::initPatchGeometry(int density) {
	if (s_vbHandle) glDeleteBuffers(1, &s_vbHandle);
	if (s_ibHandle) glDeleteBuffers(1, &s_ibHandle);

#ifdef USE_STRIPS
	s_patchIndexCount = density * (2 * density + 3);
	if (m_preWarmCache) {
		//s_patchIndexCount += 2 * density + 2;
		s_patchIndexCount += 1 * density + 3;
	}
#else
	s_patchIndexCount = density * density * 6;

	int strips = 0;
	if (m_preWarmCache) {
		s_patchIndexCount += 3 * density;
	}
#endif
	s_patchVertexCount = (density + 1) * (density + 1);

	float * const vertexPtr = new float[2 * s_patchVertexCount];
	GLuint * const indexPtr = new GLuint[s_patchIndexCount];

	int idx = 0;
	for (int i = 0; i <= density; i++) {
		float v = float(i) / density;

		for (int e = 0; e <= density; e++) {
			float u = float(e) / density;

			vertexPtr[idx++] = u;
			vertexPtr[idx++] = v;
		}
	}
	idx = 0;

#ifdef USE_STRIPS
	if (m_preWarmCache) {
		// Create degenerate triangles to prewarm cache.
		for (int i = 0; i < density; i++) {
			indexPtr[idx++] = i + 1;
			//indexPtr[idx++] = i + 1;
		}

		indexPtr[idx++] = 0xFFFF; // Restart strip.
	}

	// Real triangles:
	for (int i = 0; i < density; i++) {
		indexPtr[idx++] = (density + 1) * (i + 1);
		indexPtr[idx++] = (density + 1) * (i + 0);

		for (int e = 0; e < density; e++) {
			indexPtr[idx++] = (density + 1) * (i + 1) + (e + 1);
			indexPtr[idx++] = (density + 1) * (i + 0) + (e + 1);
		}

		indexPtr[idx++] = 0xFFFF; // Restart strip.
	}

#else
	if (m_preWarmCache) {
		// Create degenerate triangles to prewarm cache.
		for (int i = 0; i < density; i++) {
			indexPtr[idx++] = i;
			indexPtr[idx++] = i + 1;
			indexPtr[idx++] = i + 1;
		}
	}

	// Real triangles:
	for (int i = 0; i < density; i++) {
		for (int e = 0; e < density; e++) {
			indexPtr[idx++] = (density + 1) * (i + 0) + (e + 0);
			indexPtr[idx++] = (density + 1) * (i + 0) + (e + 1);
			indexPtr[idx++] = (density + 1) * (i + 1) + (e + 0);

			indexPtr[idx++] = (density + 1) * (i + 1) + (e + 0);
			indexPtr[idx++] = (density + 1) * (i + 0) + (e + 1);
			indexPtr[idx++] = (density + 1) * (i + 1) + (e + 1);
		}
	}
#endif

	glGenBuffers(1, &s_vbHandle);
	glBindBuffer(GL_ARRAY_BUFFER, s_vbHandle);
	glBufferData(GL_ARRAY_BUFFER, s_patchVertexCount * sizeof(float) * 2, vertexPtr, GL_STATIC_DRAW);

	glGenBuffers(1, &s_ibHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ibHandle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, s_patchIndexCount * sizeof(unsigned int), indexPtr, GL_STATIC_DRAW);

	delete[] vertexPtr;
	delete[] indexPtr;
}

void Game::renderUi() {
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
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Checkbox("Draw control mesh", &m_drawControl);
	ImGui::Checkbox("Draw Wirframe", &m_wireframe);
	int currentRenderMode = renderMode;
	if (ImGui::Combo("Render", &currentRenderMode, "Instanced Tessellation\0OpenGL Evaluators\0Static Mesh\0\0")) {
		renderMode = static_cast<RenderMode>(currentRenderMode);
	}
	
	if (renderMode == RenderMode::TESS) {
		if (ImGui::Checkbox("Optimize vertex cache", &m_preWarmCache)) {
			initPatchGeometry(m_tessLevel + 1);
		}
	}

	if (renderMode == RenderMode::TESS || renderMode == RenderMode::EVALUATERS) {
		if (ImGui::SliderInt("Tessellation Level", &m_tessLevel, 1, 32)) {
			initPatchGeometry(m_tessLevel + 1);
		}
	}


	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}