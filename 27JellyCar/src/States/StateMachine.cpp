#include <iostream>
#include "StateMachine.h"
#include "Application.h"

bool StateMachine::EnableWireframe = false;

StateMachine::StateMachine(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {

}

StateMachine::~StateMachine() {
	clearStates();
}

State* StateMachine::addStateAtTop(State* state) {
	if (!m_states.empty())
		m_states.top()->m_isActive = false;
	
	m_states.push(state);
	state->m_isActive = true;
	return state;
}

void StateMachine::addStateAtBottom(State* state) {
	if (m_states.empty()) {
		m_states.push(state);
		state->m_isActive = true;
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
		glPolygonMode(GL_FRONT_AND_BACK, EnableWireframe ? GL_LINE : GL_FILL);
		m_states.top()->render();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		
	}
}

void StateMachine::clearAndPush(State* state) {
	clearStates();
	m_states.push(state);
	state->m_isActive = true;
}

void StateMachine::clearStates() {
	while (!m_states.empty()) {
		delete m_states.top();
		m_states.pop();
	}
}

void StateMachine::resizeState(int deltaW, int deltaH, States state) {
	if (m_states.empty()) return;
	
	if (m_states.top()->getCurrentState() == state) {
		m_states.top()->resize(deltaW, deltaH);
	}else {
		State* temp = m_states.top();
		m_states.pop();
		resizeState(deltaW, deltaH, state);
		m_states.push(temp);
	}
}

const bool StateMachine::isRunning() const {
	return m_isRunning;
}


std::stack<State*>& StateMachine::getStates(){
	return m_states;
}

void StateMachine::ToggleWireframe() {
	EnableWireframe = !EnableWireframe;
}

bool& StateMachine::GetEnableWireframe(){
	return EnableWireframe;
}
/////////////////////////////////////////////
const bool StateInterface::isRunning() const {
	return m_isRunning;
}

const bool StateInterface::isActive() const {
	return m_isActive;
}

void StateInterface::stopState() {
	m_isRunning = false;
}

State::State(StateMachine& machine, States currentState) : m_machine(machine), m_dt(machine.m_dt), m_fdt(machine.m_fdt) {
	m_currentState = currentState;
}

State::~State() {

}

States State::getCurrentState() {
	return m_currentState;
}