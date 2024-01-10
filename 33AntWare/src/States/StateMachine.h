#pragma once

#include <engine/interfaces/IStateMachine.h>

enum States {
	MENU,
	SETTINGS,
	CONTROLLS,
	DEFAULT,
	GAME
};

class State;
class StateMachine : public IStateMachine<State> {

	friend class IStateMachine<State>;

public:

	StateMachine(const float& dt, const float& fdt);

	void fixedUpdate();
	void update();
	void render();
	void resizeState(int deltaW, int deltaH, States state);
	
	const float& m_fdt;
	const float& m_dt;

	static void ToggleWireframe();
	static bool& GetEnableWireframe();

private:

	unsigned int m_frameTexture;
	unsigned int m_frameBuffer;
	unsigned int m_rbDepthStencil;

	static bool EnableWireframe;
	
};

class State : public IState<State> {

public:

	State(StateMachine& machine, States currentState);
	virtual ~State();

	States getCurrentState();

protected:

	StateMachine& m_machine;
	const float& m_fdt;
	const float& m_dt;
	
	States m_currentState;
};