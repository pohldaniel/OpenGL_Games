#pragma once

#include <GL/glew.h>
#include <stack>

#include "engine/Shader.h"
#include "engine/Quad.h"

enum CurrentState {
	GAME,
	TUTORIAL,
	MAINMENU,
};

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
	
	const bool isRunning() const;

	const float& m_fdt;
	const float& m_dt;

	static void ToggleWireframe();
	static bool& GetEnableWireframe();

private:

	void clearStates();

	std::stack<State*> m_states;
	Quad *m_quad;
	Shader *m_shader;

	unsigned int m_frameTexture;
	unsigned int m_frameBuffer;
	unsigned int m_rbDepthStencil;
	bool m_isRunning = true;

	static bool EnableWireframe;
	
};

class State {

public:
	State(StateMachine& machine, CurrentState currentState);
	virtual ~State();

	virtual void fixedUpdate() = 0;
	virtual void update() = 0;
	virtual void render() = 0;
	virtual void resize(int deltaW, int deltaH) {};

	const bool isRunning() const;
	CurrentState getCurrentState();
	
protected:

	StateMachine& m_machine;
	const float& m_fdt;
	const float& m_dt;
	bool m_isRunning = true;
	CurrentState m_currentState;
};