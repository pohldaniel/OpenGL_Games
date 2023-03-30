#include "StateMachine.h"
#include "Application.h"

bool StateMachine::EnableWireframe = false;

StateMachine::StateMachine(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {

}

StateMachine::~StateMachine() {
	clearStates();
}

void StateMachine::resize(unsigned int width, unsigned int height) {
	
}

State* StateMachine::addStateAtTop(State* state) {
	m_states.push(state);
	return state;
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
	}else {
		m_isRunning = false;
	}
}

void StateMachine::render() {
	
	if (!m_states.empty()) {	
		glPolygonMode(GL_FRONT_AND_BACK, EnableWireframe ? GL_LINE : GL_FILL);
		m_states.top()->render(m_frameBuffer);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		
	}
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

void StateMachine::ToggleWireframe() {
	EnableWireframe = !EnableWireframe;
}

bool& StateMachine::GetEnableWireframe(){
	return EnableWireframe;
}

State::State(StateMachine& machine, CurrentState currentState) : m_machine(machine), m_dt(machine.m_dt), m_fdt(machine.m_fdt) {
	m_currentState = currentState;
}

State::~State() {

}

const bool State::isRunning() const {
	return m_isRunning;
}