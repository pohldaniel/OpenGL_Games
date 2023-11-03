#pragma once

#include <GL/glew.h>
#include <stack>

enum CurrentState {
	MENU,
	GAME,
	JELLYMENU,
	JELLYINTRO,
	JELLYSPLASH,
	JELLYGAME
};

class State;

class StateMachine {

	friend class Application;

public:

	StateMachine(const float& dt, const float& fdt);
	~StateMachine();

	State* addStateAtTop(State* state);
	void addStateAtBottom(State* state);
	void clearAndPush(State* state);

	void fixedUpdate();
	void update();
	void render();
	
	const bool isRunning() const;

	const float& m_fdt;
	const float& m_dt;

	static void ToggleWireframe();
	static bool& GetEnableWireframe();

private:

	void clearStates();

	std::stack<State*> m_states;

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