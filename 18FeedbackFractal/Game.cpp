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

	GLuint vertex = Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/prog.vert");
	const GLchar* feedbackVaryings[] = { "outValue" };

	prog[0] = new Shader();
	prog[0]->attachShader(vertex);
	prog[0]->attachShader(Shader::LoadShaderProgram(GL_GEOMETRY_SHADER, "res/prog0.gem"));
	glTransformFeedbackVaryings(prog[0]->m_program, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	prog[0]->linkShaders();

	prog[1] = new Shader();
	prog[1]->attachShader(vertex);
	prog[1]->attachShader(Shader::LoadShaderProgram(GL_GEOMETRY_SHADER, "res/prog1.gem"));
	glTransformFeedbackVaryings(prog[1]->m_program, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	prog[1]->linkShaders();

	prog[2] = new Shader();
	prog[2]->attachShader(vertex);
	prog[2]->attachShader(Shader::LoadShaderProgram(GL_GEOMETRY_SHADER, "res/prog2.gem"));
	glTransformFeedbackVaryings(prog[2]->m_program, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	prog[2]->linkShaders();

	draw[0] = new Shader("res/draw0.vert", "res/draw0.frag");
	draw[1] = new Shader("res/draw1.vert", "res/draw1.frag", "res/draw1.gem");


	// create buffer objects
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, max_buffer_verts * 4 * sizeof(GLfloat), 0, GL_STATIC_DRAW);

	// load initial vertex data
	GLfloat v[] = {
		-1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
	};
	glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * 4 * sizeof(GLfloat), v);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, max_buffer_verts * 4 * sizeof(GLfloat), 0, GL_STATIC_DRAW);

	// create query objects
	glGenQueries(1, &query);

	rand_tex = create_rand_texture(GL_TEXTURE_2D, GL_RGBA16F_ARB, 256, 256);

	init();
	activeProgram = prog[program];

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
};

void Game::render(unsigned int &frameBuffer) {

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	srand(seed);
	init();
	for (int i = 0; i < m_steps; i++) {		
		do_feedback();
	}

	auto shader = (m_enableLighting && program == Program::TERRAIN) ? draw[1] : draw[0];
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	shader->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_transform.getTransformationMatrix());	
	drawBuffer(vbo[current_buffer], m_enableLighting ? input_prim : render_prim, primitives_written*verts_per_prim / prim_output_div);
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

	m_transform.fromMatrix(arc.getTransform(Vector3f(0.0f, 1.0f, 0.0f), 180.0f));
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(60.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 10.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}

//
//
//////////////////////////////////////////////////////////////////////
float Game::frand() {
	return rand() / (float)RAND_MAX;
}

//
// create a texture containing random numbers
// 
//////////////////////////////////////////////////////////////////////
GLuint Game::create_rand_texture(GLenum target, GLenum format, int w, int h) {
	float *data = new float[w*h * 4];
	float *ptr = data;
	for (int i = 0; i<w*h; i++) {
		*ptr++ = frand()*2.0 - 1.0;
		*ptr++ = frand()*2.0 - 1.0;
		*ptr++ = frand()*2.0 - 1.0;
		*ptr++ = frand()*2.0 - 1.0;
	}

	GLuint texid;
	glGenTextures(1, &texid);
	glBindTexture(target, texid);

	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(target, 0, format, w, h, 0, GL_RGBA, GL_FLOAT, data);

	return texid;
}

//
// load initial vertex data for koch snowflake pattern
//
//////////////////////////////////////////////////////////////////////
void Game::init_koch() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	GLfloat v[] = {
		1.0, -1.0, 0.0, 1.0,
		-1.0, -1.0, 0.0, 1.0,

		-1.0, -1.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,

		0.0, 1.0, 0.0, 1.0,
		1.0, -1.0, 0.0, 1.0,
	};
	glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * 4 * sizeof(GLfloat), v);

	current_buffer = 0;
	verts_per_prim = 2;
	prim_output_div = 1;
	input_prim = render_prim = feedback_prim = GL_LINES;
	primitives_written = 3;
	
}

//
// load initial vertex data for terrain
//
//////////////////////////////////////////////////////////////////////
void Game::init_terrain() {
	GLfloat h[4];
	for (int i = 0; i<4; i++)
		h[i] = frand();

	GLfloat v[] = {
		-1.0, h[0], -1.0, 1.0,
		1.0, h[1], -1.0, 1.0,
		1.0, h[2], 1.0, 1.0,
		-1.0, h[3], 1.0, 1.0,
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * 4 * sizeof(GLfloat), v);

	current_buffer = 0;
	verts_per_prim = 4;
	prim_output_div = 4;
	input_prim = GL_LINES_ADJACENCY;
	render_prim = GL_QUADS;
	feedback_prim = GL_POINTS;
	primitives_written = 4;
	rand_scale = init_scale;
}

//
//
//////////////////////////////////////////////////////////////////////
void Game::init() {

	switch (program){
		case KOCH: case LEAVE:
			init_koch();
			break;
		case TERRAIN:
			init_terrain();
			break;
	}
}

void Game::drawBuffer(GLuint buffer, GLenum prim, int verts) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glDrawArrays(prim, 0, verts);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//
// display geometry stored in buffer object
//
//////////////////////////////////////////////////////////////////////
void Game::subdivideBuffer(GLuint buffer, GLenum prim, int verts) {

	glUseProgram(activeProgram->m_program);
	activeProgram->loadFloat("rand_scale", rand_scale);
	activeProgram->loadVector("rand_xform", Vector2f(rand_xform_scale, rand_xform_offset));

	activeProgram->loadInt("rand_tex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rand_tex);

	glBeginTransformFeedback(feedback_prim);
	drawBuffer(buffer, prim, verts);
	glEndTransformFeedback();

	glUseProgram(0);
}

//
// render object, storing positions to buffer object
//
//////////////////////////////////////////////////////////////////////
void Game::do_feedback() {
	// set base for transform feedback
	glEnable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[1 - current_buffer]);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);

	subdivideBuffer(vbo[current_buffer], input_prim, primitives_written*verts_per_prim / prim_output_div);

	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_NV);

	glDisable(GL_RASTERIZER_DISCARD);

	// read back query results
	glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives_written);
	current_buffer = 1 - current_buffer;
	rand_scale *= 0.5;
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
	ImGui::Checkbox("Draw Wirframe", &Globals::enableWireframe);
	ImGui::Checkbox("Enable Lighting", &m_enableLighting);
	if (ImGui::Checkbox("Continuous Subdivision", &m_continuousSubdivision)) {
		m_steps = !m_continuousSubdivision ? 0 : m_steps;
	}

	int currentRenderMode = program;
	if (ImGui::Combo("Program", &currentRenderMode, "Koch\0Leave\0Terrain\0\0")) {
		program = static_cast<Program>(currentRenderMode);
		activeProgram = prog[program];	

		switch (program) {
			case KOCH:
				max_steps = 8;
				break;
			case LEAVE:
				max_steps = 7;
				break;
			case TERRAIN:
				max_steps = 9;
				break;
		}
		m_steps = m_steps > max_steps ? max_steps : m_steps;
	}
	if(m_continuousSubdivision)
		ImGui::SliderInt("Steps", &m_steps, 0, max_steps);
	else {
		if (ImGui::Button("-")) {
			m_steps--;
			m_steps = (std::max)(0, (std::min)(m_steps, max_steps));
		}
		ImGui::SameLine();
		ImGui::Text(std::to_string(m_steps).c_str());
		ImGui::SameLine();
		if (ImGui::Button("+")) {
			m_steps++;
			m_steps = (std::max)(0, (std::min)(m_steps, max_steps));
		}
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}