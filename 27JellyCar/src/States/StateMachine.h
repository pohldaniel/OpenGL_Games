#pragma once

#include <GL/glew.h>
#include <stack>

enum CurrentState {
	MENU,
	GAME,
	JELLYMENU,
	JELLYINTRO,
	JELLYSPLASH,
	JELLYGAME,
	JELLYOPTIONS,
	JELLYDIALOG,
	JELLYFINISH,
	JELLYPAUSE
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
	void resizeState(int deltaW, int deltaH, CurrentState state);
	
	const bool isRunning() const;
	std::stack<State*>& getStates();

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

	friend class StateMachine;

public:
	State(StateMachine& machine, CurrentState currentState);
	virtual ~State();

	virtual void fixedUpdate() = 0;
	virtual void update() = 0;
	virtual void render() = 0;
	virtual void resize(int deltaW, int deltaH) {};

	const bool isRunning() const;
	const bool isActive() const;
	CurrentState getCurrentState();
	
protected:

	StateMachine& m_machine;
	const float& m_fdt;
	const float& m_dt;
	bool m_isRunning = true;
	bool m_isActive = true;
	CurrentState m_currentState;
};