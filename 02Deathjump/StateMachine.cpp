#include "StateMachine.h"
#include "Transition.h"
#include "ViewEffect.h"

StateMachine::StateMachine(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt){
	m_quad = new Quad(false);
		
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
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_frameTexture, 0);
	
	// buffer for depth and stencil
	unsigned int rbDepthStencil;
	glGenRenderbuffers(1, &rbDepthStencil);
	glBindRenderbuffer(GL_RENDERBUFFER, rbDepthStencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbDepthStencil);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

StateMachine::~StateMachine() {
	clearStates();
	delete m_quad;
}

/*void StateMachine::addStateAtTop(State* state) {
	m_states.push(state);
}*/

void StateMachine::addStateAtTop(State* state, std::string currentState) {
	m_states.push(state);
	//std::cout << currentState << std::endl;
}

/*void StateMachine::addStateAtBottom(State* state) {
	if (m_states.empty()) {
		m_states.push(state);
	}else {
		State* temp = m_states.top();
		m_states.pop();
		addStateAtBottom(state);
		m_states.push(temp);
	}
}*/

void StateMachine::addStateAtBottom(State* state, std::string currentState) {
	if (m_states.empty()) {
		m_states.push(state);
	}
	else {
		State* temp = m_states.top();
		m_states.pop();
		addStateAtBottom(state, currentState);
		m_states.push(temp);
	}

	//std::cout << currentState << std::endl;
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
		
	if (!m_states.empty())
		m_states.top()->render(m_frameBuffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	//maybe a second post process will be the better approach over combining transition ans shake in one shader
	glEnable(GL_BLEND);
	glUseProgram(*&Transition::get().getShader().m_program);
	Transition::get().getShader().loadMatrix("u_transform", ViewEffect::get().getView());
	m_quad->render(m_frameTexture);
	glUseProgram(0);
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

State::State(StateMachine& machine, CurrentState currentState) : m_machine(machine), m_dt(machine.m_dt), m_fdt(machine.m_fdt){
	m_currentState = currentState;
}

State::~State() {
}

const bool State::isRunning() const {
	return m_isRunning;
}

