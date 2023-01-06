#include "StateMachine.h"

StateMachine::StateMachine(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {

	float pointX;
	float pointY;

	float pointXTrans;
	float pointYTrans;

	for (int i = 4; i >= 0; i--) {
		for (int j = 4; j >= 0; j--) {
			pointX = j * 0.5f;
			pointY = i * 0.5f;

			pointXTrans = pointX - pointY;
			pointYTrans = (pointX + pointY) * ((float)TILE_WIDTH / TILE_HEIGHT) * 0.5f;

			m_quads.push_back(new Quad(false, pointXTrans, pointXTrans + 1.0f, pointYTrans, pointYTrans + 1.0f, TILE_WIDTH, TILE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0));
		}
	}

	for (int i = 4; i >= 0; i--) {
		for (int j = 4; j >= 0; j--) {
			pointX = -j;
			pointY = i;
			m_quads2.push_back(new Quad(false, pointX, pointX + 1.0f, pointY, pointY + 1.0f, TILE_WIDTH * scale + dist, TILE_WIDTH * scale + dist, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0));
		}
	}

	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("tiles");
	m_spriteSheet2 = Globals::spritesheetManager.getAssetPointer("tiles2");
	m_sprites["tile"] = Globals::textureManager.get("tile").getTexture();


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


	m_projection = Matrix4f::GetOrthographic(m_projection, -static_cast<float>(WIDTH) * 0.5f, static_cast<float>(WIDTH) * 0.5f, -static_cast<float>(HEIGHT) * 0.5f, static_cast<float>(HEIGHT)  * 0.5f, -1000.0f, 1000.0f);
	m_view.lookAt(Vector3f(dist, dist, dist), Vector3f(0.0f, 0.8f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));

	Matrix4f rot, trans;
	rot.rotate(Vector3f(1.0, 0.0, 0.0), 90.0f);

	float offsetX2 = m_offsetX;
	float offsetY2 = 0.5 + (m_offsetY * ((float)TILE_HEIGHT / TILE_WIDTH) * 2.0f);

	trans.translate((offsetX2 - offsetY2) * (TILE_WIDTH * scale + dist), (offsetX2 + offsetY2) * (TILE_WIDTH * scale + dist), 0.0f);
	m_transform2 = rot * trans;

	offsetX2 = m_offsetX * TILE_WIDTH * 0.99f;
	offsetY2 = m_offsetY * (TILE_HEIGHT);
	trans.translate(offsetX2, offsetY2, 0.0f);
	m_transform = trans;
}

StateMachine::~StateMachine() {
	clearStates();
	delete m_quad;
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

	if (Globals::CONTROLLS & Globals::KEY_A) {
		m_offsetX = m_offsetX - 0.1f;

		Matrix4f rot, trans;
		rot.rotate(Vector3f(1.0, 0.0, 0.0), 90.0f);

		float offsetX2 = m_offsetX;
		float offsetY2 = 0.5 + (m_offsetY * ((float)TILE_HEIGHT / TILE_WIDTH) * 2.0f);

		trans.translate((offsetX2 - offsetY2) * (TILE_WIDTH * scale + dist), (offsetX2 + offsetY2) * (TILE_WIDTH * scale + dist), 0.0f);
		m_transform2 = rot * trans;

		offsetX2 = (m_offsetX)* TILE_WIDTH * 0.99f;
		offsetY2 = m_offsetY * (TILE_HEIGHT);

		trans.translate(offsetX2, offsetY2, 0.0f);
		m_transform = trans;
	}

	if (Globals::CONTROLLS & Globals::KEY_D) {
		m_offsetX = m_offsetX + 0.1f;

		Matrix4f rot, trans;
		rot.rotate(Vector3f(1.0, 0.0, 0.0), 90.0f);

		float offsetX2 = m_offsetX;
		float offsetY2 = 0.5 + (m_offsetY * ((float)TILE_HEIGHT / TILE_WIDTH) * 2.0f);

		trans.translate((offsetX2 - offsetY2) * (TILE_WIDTH * scale + dist), (offsetX2 + offsetY2) * (TILE_WIDTH * scale + dist), 0.0f);
		m_transform2 = rot * trans;

		offsetX2 = (m_offsetX)* TILE_WIDTH * 0.99f;
		offsetY2 = m_offsetY * (TILE_HEIGHT);

		trans.translate(offsetX2, offsetY2, 0.0f);
		m_transform = trans;
	}

	if (Globals::CONTROLLS & Globals::KEY_W) {
		m_offsetY = m_offsetY + 0.1f;

		Matrix4f rot, trans;
		rot.rotate(Vector3f(1.0, 0.0, 0.0), 90.0f);

		float offsetX2 = m_offsetX;
		float offsetY2 = 0.5 + (m_offsetY * ((float)TILE_HEIGHT / TILE_WIDTH) * 2.0f);

		trans.translate((offsetX2 - offsetY2) * (TILE_WIDTH * scale + dist), (offsetX2 + offsetY2) * (TILE_WIDTH * scale + dist), 0.0f);
		m_transform2 = rot * trans;

		offsetX2 = (m_offsetX)* TILE_WIDTH * 0.99f;
		offsetY2 = m_offsetY * (TILE_HEIGHT);

		trans.translate(offsetX2, offsetY2, 0.0f);
		m_transform = trans;
	}

	if (Globals::CONTROLLS & Globals::KEY_S) {
		m_offsetY = m_offsetY - 0.1f;

		Matrix4f rot, trans;
		rot.rotate(Vector3f(1.0, 0.0, 0.0), 90.0f);

		float offsetX2 = m_offsetX;
		float offsetY2 = 0.5 + (m_offsetY * ((float)TILE_HEIGHT / TILE_WIDTH) * 2.0f);

		trans.translate((offsetX2 - offsetY2) * (TILE_WIDTH * scale + dist), (offsetX2 + offsetY2) * (TILE_WIDTH * scale + dist), 0.0f);
		m_transform2 = rot * trans;

		offsetX2 = (m_offsetX)* TILE_WIDTH * 0.99f;
		offsetY2 = m_offsetY * (TILE_HEIGHT);

		trans.translate(offsetX2, offsetY2, 0.0f);
		m_transform = trans;
	}
}

void StateMachine::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glUseProgram(m_shaderArray->m_program);

	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadInt("u_layer", 1);
	m_shaderArray->loadMatrix("u_transform", m_projection * m_transform);
	for (int i = 0; i < m_quads.size(); i++) {
		m_quads[i]->render(m_spriteSheet2->getAtlas(), true);
	}
	glUseProgram(0);

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", m_projection * m_view * m_transform2);
	for (int i = 0; i < m_quads2.size(); i++) {
		m_quads2[i]->render(m_sprites["tile"]);
	}
	glUseProgram(0);

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

