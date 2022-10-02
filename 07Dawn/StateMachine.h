#pragma once
#include <stack>

#include "engine/Extension.h"
#include "engine/Shader.h"
#include "engine/Quad.h"
#include "Constants.h"

enum CurrentState {
	GAME,
	EDITOR,
	PAUSE,
	MAINMENU,
	LOADINGSCREEN
};

//base for holding the Framebuffer
class State;

class StateMachine {
	friend class Application;
public:
	StateMachine(const float& dt, const float& fdt);
	~StateMachine();

	State* addStateAtTop(State* state);
	void addStateAtBottom(State* state);

	void fixedUpdate();
	void update();
	void render();
	void resize(unsigned int width, unsigned int height);

	void clearAndPush(State* state);
	void toggleWireframe() {
		Globals::enableWireframe = !Globals::enableWireframe;
	}
	const bool isRunning() const;

	const float& m_fdt;
	const float& m_dt;

private:
	void clearStates();

	std::stack<State*> m_states;
	Quad *m_quad;
	Shader *m_shader;

	unsigned int m_frameTexture;
	unsigned int m_frameBuffer;
	unsigned int m_rbDepthStencil;
	bool m_isRunning = true;
	
};

class State {
public:
	State(StateMachine& machine, CurrentState currentState);
	virtual ~State();

	virtual void fixedUpdate() = 0;
	virtual void update() = 0;
	virtual void render(unsigned int &m_frameBuffer) = 0;
	virtual void resize() {};

	const bool isRunning() const;

	CurrentState m_currentState;

protected:
	StateMachine& m_machine;
	const float& m_fdt;
	const float& m_dt;
	bool m_isRunning = true;
};