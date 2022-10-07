#include "StateMachine.h"

StateMachine::StateMachine(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {

	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_quad = new Quad(false, -1.0f, 1.0f, -1.0f, 1.0f);

	glGenTextures(1, &m_frameTexture);
	glBindTexture(GL_TEXTURE_2D, m_frameTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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

State* StateMachine::addStateAtTop(State* state) {
	m_states.push(state);
	return state;
}

void StateMachine::addStateAtBottom(State* state) {
	if (m_states.empty()) {
		m_states.push(state);

	}else {
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

	}else {
		m_isRunning = false;
	}
}

void StateMachine::render() {
	if (!m_states.empty()) {
		glPolygonMode(GL_FRONT_AND_BACK, Globals::enableWireframe ? GL_LINE : GL_FILL);

		m_states.top()->render(m_frameBuffer);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);
	glUseProgram(m_shader->m_program);
	//m_shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	m_quad->render(m_frameTexture);
	glUseProgram(0);
	Message::Get().draw();
	Message::Get().deleteDecayed();
	glDisable(GL_BLEND);	
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

State::State(StateMachine& machine, CurrentState currentState) : m_machine(machine), m_dt(machine.m_dt), m_fdt(machine.m_fdt) {
	m_currentState = currentState;
}

State::~State() {
}

const bool State::isRunning() const {
	return m_isRunning;
}