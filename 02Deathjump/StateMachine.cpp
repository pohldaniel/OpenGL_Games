#include "StateMachine.h"

StateMachine::StateMachine(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt){
	//m_frame.create(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_shader = new Shader("shader/quad.vs", "shader/quad.fs");
	m_quad = new Quad();
	
	
	glGenTextures(1, &m_frameTexture);
	glBindTexture(GL_TEXTURE_2D, m_frameTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
}

void StateMachine::addStateAtTop(State* state) {
	m_states.push(state);
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
		//m_window.close();
	}
}

void StateMachine::render() {
	

	if (!m_states.empty())
		m_states.top()->render(m_frameBuffer);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	m_quad->render(m_frameTexture);
	glUseProgram(0);

	//m_frame.clear();

	//if (!m_states.empty())
		//m_states.top()->Render(m_frame);

	//m_frame.display();

	//m_frameSprite.setTexture(m_frame.getTexture());

	//m_window.draw(m_frameSprite, &Transition::Get().GetShader());

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

State::State(StateMachine& machine) : i_machine(machine), i_dt(machine.m_dt), i_fdt(machine.m_fdt){

}

State::~State() {

}

const bool State::isRunning() const {
	return i_isRunning;
}

