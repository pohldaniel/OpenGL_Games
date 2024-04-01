#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "engine/input/EventDispatcher.h"

#include "Game.h"
#include "Application.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	EventDispatcher::AddMouseListener(this);
	
	m_camera = Camera();
	m_camera.perspective(60.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 10.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));


	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-3.0f);
	applyTransformation(m_trackball);

	progGeom[0] = new Shader("res/prog0.vert", "res/prog.frag", "res/prog0.gem");
	progGeom[1] = new Shader("res/prog1.vert", "res/prog.frag", "res/prog1.gem");
	progGeom[2] = new Shader("res/prog2.vert", "res/prog.frag", "res/prog1.gem");

	prog[0] = new Shader("res/prog0.vert", "res/prog.frag");
	prog[1] = new Shader("res/prog1.vert", "res/prog.frag");
	prog[2] = new Shader("res/prog2.vert", "res/prog.frag");

	const unsigned char edge_table[] = {
		0, 0, 0, 0,
		3, 0, 3, 1,
		2, 1, 2, 0,
		2, 0, 3, 0,
		1, 2, 1, 3,
		1, 0, 1, 2,
		1, 0, 2, 0,
		3, 0, 1, 0,
		0, 2, 0, 1,
		0, 1, 3, 1,
		0, 1, 0, 3,
		3, 1, 2, 1,
		0, 2, 1, 2,
		1, 2, 3, 2,
		0, 3, 2, 3,

		0, 0, 0, 1,
		3, 2, 0, 0,
		2, 3, 0, 0,
		2, 1, 3, 1,
		1, 0, 0, 0,
		3, 0, 3, 2,
		1, 3, 2, 3,
		2, 0, 0, 0,
		0, 3, 0, 0,
		0, 2, 3, 2,
		2, 1, 2, 3,
		0, 1, 0, 0,
		0, 3, 1, 3,
		0, 2, 0, 0,
		1, 3, 0, 0,
	};


	// create vertex buffers
	GLuint sizeLog2[3] = { X_SIZE_LOG2, Y_SIZE_LOG2, Z_SIZE_LOG2 };

	glGenBuffers(1, &position_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
	glBufferData(GL_ARRAY_BUFFER, TOTAL_POINTS * 4 * sizeof(GLfloat), 0, GL_STATIC_DRAW);
	float *positions = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	GeneratePositions(positions, sizeLog2);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glGenBuffers(1, &index_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, CELLS_COUNT * 24 * sizeof(GLuint), 0, GL_STATIC_DRAW);

	GLuint *indices = (GLuint *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
	GenerateIndices(indices, sizeLog2);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// create edge table texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &edge_tex);
	glBindTexture(GL_TEXTURE_RECTANGLE, edge_tex);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA8, 15, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, edge_table);
	//glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB8UI, 15, 2, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, edge_table);

	volume_tex = loadRawVolume("res/Bucky.raw", 32, 32, 32);
	noise_tex = create_noise_texture(GL_RGBA16F, 64, 64, 64);
	glEnable(GL_DEPTH_TEST);
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
	anim += 0.01;
};

void Game::render(unsigned int &frameBuffer) {

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = m_withGeom ? progGeom[renderMode] : prog[renderMode];

	glUseProgram(shader->m_program);
	shader->loadVector("SizeMask", std::array<int, 3>{ (1 << X_SIZE_LOG2) - 1, (1 << Y_SIZE_LOG2) - 1, (1 << Z_SIZE_LOG2) - 1 });
	shader->loadVector("SizeShift", std::array<int, 3>{ 0, X_SIZE_LOG2, X_SIZE_LOG2 + Y_SIZE_LOG2 });

	shader->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	shader->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
	shader->loadFloat("IsoValue", isovalue);

	if (renderMode == RenderMode::PROG0) {	
		shader->loadVector("Metaballs[1]", Vector4f(0.1f + sinf(anim)*0.5f, cosf(anim)*0.5f, 0.0f, 0.1f));
	}

	if (renderMode == RenderMode::PROG1) {
		shader->loadInt("volumeTex", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, volume_tex);
	}

	if (renderMode == RenderMode::PROG2) {	
		shader->loadFloat("time", anim);
		shader->loadInt("volumeTex", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, noise_tex);	
	}

	if (m_withGeom) {
		shader->loadInt("edgeTex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, edge_tex);
	}

	shader->loadBool("u_withFrag", m_withFrag);
	if (m_drawLayers)
		drawLayers();
	else
		drawTetrahedra();	


	glUseProgram(0);
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

	m_transform.fromMatrix(arc.getTransform());
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(60.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 10.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}
void Game::UnSwizzle(GLuint index, GLuint sizeLog2[3], GLuint * pPos) {

	// force index traversal to occur in 2x2x2 blocks by giving each of x, y, and z one
	// of the bottom 3 bits
	pPos[0] = index & 1;
	index >>= 1;
	pPos[1] = index & 1;
	index >>= 1;
	pPos[2] = index & 1;
	index >>= 1;

	// Treat the rest of the index like a row, collumn, depth array
	// Each dimension needs to grab sizeLog2 - 1 bits
	// This will make the blocks traverse the grid in a raster style order
	index <<= 1;
	pPos[0] = pPos[0] | (index &  ((1 << sizeLog2[0]) - 2));
	index >>= sizeLog2[0] - 1;
	pPos[1] = pPos[1] | (index &  ((1 << sizeLog2[1]) - 2));
	index >>= sizeLog2[1] - 1;
	pPos[2] = pPos[2] | (index &  ((1 << sizeLog2[2]) - 2));

}

//--------------------------------------------------------------------------------------
// Generate indices for the sampling grid in swizzled order
//--------------------------------------------------------------------------------------
void Game::GenerateIndices(GLuint *indices, GLuint sizeLog2[3]) {
	GLuint nTotalBits = sizeLog2[0] + sizeLog2[1] + sizeLog2[2];
	GLuint nPointsTotal = 1 << nTotalBits;

	for (GLuint i = 0; i<nPointsTotal; i++) {

		GLuint pos[3];
#if SWIZZLE
		UnSwizzle(i, sizeLog2, pos);	// un-swizzle current index to get x, y, z for the current sampling point
#else
		pos[0] = i & ((1 << X_SIZE_LOG2) - 1);
		pos[1] = (i >> X_SIZE_LOG2) & ((1 << Y_SIZE_LOG2) - 1);
		pos[2] = (i >> (X_SIZE_LOG2 + Y_SIZE_LOG2)) & ((1 << Z_SIZE_LOG2) - 1);
#endif
		if (pos[0] == (1 << sizeLog2[0]) - 1 || pos[1] == (1 << sizeLog2[1]) - 1 || pos[2] == (1 << sizeLog2[2]) - 1)
			continue;	// skip extra cells


						// NOTE: order of vertices matters! important for backface culling

						// T0
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);

		// T1
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1] + 1, pos[2], sizeLog2);

		// T2
		*indices++ = MAKE_INDEX(pos[0], pos[1] + 1, pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1] + 1, pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);

		// T3
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1] + 1, pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);

		// T4
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1], pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);

		// T5
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1], pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);
	}
}

//
// Generate vertex positions
//
//////////////////////////////////////////////////////////////////////
void Game::GeneratePositions(float *positions, GLuint sizeLog2[3]) {
	GLuint nTotalBits = sizeLog2[0] + sizeLog2[1] + sizeLog2[2];
	GLuint nPointsTotal = 1 << nTotalBits;
	for (GLuint i = 0; i<nPointsTotal; i++) {
		GLuint pos[3];
		pos[0] = i & ((1 << X_SIZE_LOG2) - 1);
		pos[1] = (i >> X_SIZE_LOG2) & ((1 << Y_SIZE_LOG2) - 1);
		pos[2] = (i >> (X_SIZE_LOG2 + Y_SIZE_LOG2)) & ((1 << Z_SIZE_LOG2) - 1);

		*positions++ = (float(pos[0]) / float(1 << X_SIZE_LOG2))*2.0 - 1.0;
		*positions++ = (float(pos[1]) / float(1 << Y_SIZE_LOG2))*2.0 - 1.0;
		*positions++ = (float(pos[2]) / float(1 << Z_SIZE_LOG2))*2.0 - 1.0;
		*positions++ = 1.0f;
	}
}

//
//
//////////////////////////////////////////////////////////////////////
GLuint Game::loadRawVolume(const char *filename, int w, int h, int d) {
	FILE *fp = fopen(filename, "rb");
	if (!fp) return 0;

	GLubyte *data = new GLubyte[w*h*d];
	fread(data, sizeof(GLubyte), w*h*d, fp);
	fclose(fp);

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_3D, tex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE8, w, h, d, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
	delete[] data;

	return tex;
}

//
//
//////////////////////////////////////////////////////////////////////
float Game::frand() {
	return rand() / (float)RAND_MAX;
}

//
//
//////////////////////////////////////////////////////////////////////
GLuint Game::create_noise_texture(GLenum format, int w, int h, int d) {
	float *data = new float[w*h*d * 4];
	float *ptr = data;
	for (int i = 0; i<w*h*d; i++) {
		*ptr++ = frand()*2.0 - 1.0;
		*ptr++ = frand()*2.0 - 1.0;
		*ptr++ = frand()*2.0 - 1.0;
		*ptr++ = frand()*2.0 - 1.0;
	}

	GLuint texid;
	glGenTextures(1, &texid);
	GLenum target = GL_TEXTURE_3D;
	glBindTexture(target, texid);

	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage3D(target, 0, format, w, h, d, 0, GL_RGBA, GL_FLOAT, data);
	delete[] data;

	return texid;
}

//
//
//////////////////////////////////////////////////////////////////////
void Game::drawTetrahedra() {
	glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
	glDrawElements(prim, nprims, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
}

//
//
//////////////////////////////////////////////////////////////////////
void Game::drawLayers() {
	const int layers = 5;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);

	for (int i = 0; i<layers; i++) {
		float t = i / (float)(layers + 1);
		float iso = isovalue + t*0.5;
		progGeom[renderMode]->loadFloat("IsoValue", iso);
		drawTetrahedra();
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
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

	int currentRenderMode = renderMode;
	if (ImGui::Combo("Program", &currentRenderMode, "Prog 0\0Prog 1\0Prog 2\0\0")) {
		renderMode = static_cast<RenderMode>(currentRenderMode);
		if (renderMode == RenderMode::PROG0)
			isovalue = 1.0f;

		if (renderMode == RenderMode::PROG1)
			isovalue = 0.5f;

		if (renderMode == RenderMode::PROG2)
			isovalue = 0.5f;
	}

	ImGui::SliderFloat("Iso Value", &isovalue, 0.0f, 5.0f);
	ImGui::SliderInt("Prim Count", &nprims, 0, CELLS_COUNT * 24);

	ImGui::Checkbox("Draw Layers", &m_drawLayers);
	ImGui::Checkbox("With Geometry", &m_withGeom);
	ImGui::Checkbox("With Fragment", &m_withFrag);
	if (ImGui::Checkbox("Use Points", &m_usePoints)) {
		prim = m_usePoints ? GL_POINTS : GL_LINES_ADJACENCY;
	}
	ImGui::Checkbox("Draw Wirframe", &Globals::enableWireframe);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}