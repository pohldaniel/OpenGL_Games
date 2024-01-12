#include <GL/glew.h>
#include <imgui.h>
#include "StateMachine.h"
#include "Application.h"
#include <iostream>

bool StateMachine::EnableWireframe = false;

StateMachine::StateMachine(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {

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
			ImGui::GetIO().WantCaptureMouse = false;
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

void StateMachine::ToggleWireframe() {
	EnableWireframe = !EnableWireframe;
}

bool& StateMachine::GetEnableWireframe(){
	return EnableWireframe;
}

/////////////////////////////////////////////
State::State(StateMachine& machine, States currentState) : m_machine(machine), m_dt(machine.m_dt), m_fdt(machine.m_fdt) {
	m_currentState = currentState;
}

State::~State() {

}

States State::getCurrentState() {
	return m_currentState;
}