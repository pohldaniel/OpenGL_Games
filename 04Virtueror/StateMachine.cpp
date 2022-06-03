#include "StateMachine.h"

StateMachine::StateMachine(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {

	m_mapLoader.loadLevel("res/maps/40x40-01.map");
	short stride = 5, offset = 3;

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_vboMap);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_mapLoader.m_vertices.size() * sizeof(float), &m_mapLoader.m_vertices[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//texcoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

	//map indices
	glBindBuffer(GL_ARRAY_BUFFER, m_vboMap);
	glBufferData(GL_ARRAY_BUFFER, m_mapLoader.m_indexMap.size() * sizeof(m_mapLoader.m_indexMap[0]), &m_mapLoader.m_indexMap[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 1 * sizeof(unsigned int), 0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mapLoader.m_indexBuffer.size() * sizeof(unsigned int), &m_mapLoader.m_indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("tiles");
	m_shaderLevel = Globals::shaderManager.getAssetPointer("level");

	glGenTextures(1, &m_frameTexture);
	glBindTexture(GL_TEXTURE_2D, m_frameTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);	
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_frameTexture, 0);

	// buffer for depth and stencil
	glGenRenderbuffers(1, &m_rbDepthStencil);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbDepthStencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbDepthStencil);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	m_projection = Matrix4f::GetOrthographic(m_projection, -static_cast<float>(WIDTH) * 0.5f, static_cast<float>(WIDTH) * 0.5f, -static_cast<float>(HEIGHT) * 0.5f, static_cast<float>(HEIGHT) * 0.5f, -1000.0f, 1000.0f);

	Matrix4f trans;
	trans.translate(m_offsetX * TILE_WIDTH, m_offsetY * TILE_HEIGHT, 0.0f);
	m_transform = trans;
}

StateMachine::~StateMachine() {
	clearStates();
	//delete m_quad;
}

void StateMachine::resize(unsigned int width, unsigned int height) {
	glBindTexture(GL_TEXTURE_2D, m_frameTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, m_rbDepthStencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbDepthStencil);
}

void StateMachine::addStateAtTop(State* state) {
	m_states.push(state);
}

void StateMachine::addStateAtBottom(State* state) {
	if (m_states.empty()) {
		m_states.push(state);
	}
	else {
		State* temp = m_states.top();
		m_states.pop();
		addStateAtBottom(state);
		m_states.push(temp);
	}
}

void StateMachine::fixedUpdate() {
	if (!m_states.empty())
		m_states.top()->fixedUpdate();
}

void StateMachine::update() {

	if (!m_states.empty()) {
		m_states.top()->update();
		if (!m_states.top()->isRunning()) {
			delete m_states.top();
			m_states.pop();
		}
	}
	else {
		m_isRunning = false;
	}

	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyDown(Keyboard::KEY_A)) {
		m_offsetX = m_offsetX + 0.1f;

		Matrix4f trans;
		trans.translate(m_offsetX * TILE_WIDTH, m_offsetY * TILE_HEIGHT, 0.0f);
		m_transform = trans;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		m_offsetX = m_offsetX - 0.1f;

		Matrix4f trans;
		trans.translate(m_offsetX * TILE_WIDTH, m_offsetY * TILE_HEIGHT, 0.0f);
		m_transform = trans;
	}

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		m_offsetY = m_offsetY - 0.2f;

		Matrix4f trans;
		trans.translate(m_offsetX * TILE_WIDTH, m_offsetY * TILE_HEIGHT, 0.0f);
		m_transform = trans;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		m_offsetY = m_offsetY + 0.2f;

		Matrix4f trans;
		trans.translate(m_offsetX * TILE_WIDTH, m_offsetY * TILE_HEIGHT, 0.0f);
		m_transform = trans;
	}
}

void StateMachine::render() {

	glClearColor(0.06f, 0.06f, 0.06f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_BLEND);
	glUseProgram(m_shaderLevel->m_program);
	m_shaderLevel->loadMatrix("u_transform", m_transform * m_projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_spriteSheet->getAtlas());
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_mapLoader.m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glUseProgram(0);
	glEnable(0);
}

void StateMachine::clearAndPush(State* state) {
	clearStates();
	m_states.push(state);
}

void StateMachine::clearStates() {
	while (!m_states.empty()) {
		delete m_states.top();
		m_states.pop();
	}
}

const bool StateMachine::isRunning() const {
	return m_isRunning;
}

void StateMachine::OnMouseMotion(Event::MouseMoveEvent& event) {
	std::cout << event.x << "  " << event.y << std::endl;
}

State::State(StateMachine& machine, CurrentState currentState) : m_machine(machine), m_dt(machine.m_dt), m_fdt(machine.m_fdt) {
	m_currentState = currentState;
}

State::~State() {
}

const bool State::isRunning() const {
	return m_isRunning;
}

