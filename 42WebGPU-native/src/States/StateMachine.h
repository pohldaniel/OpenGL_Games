#pragma once
#include <engine/interfaces/IStateMachine.h>

enum States {
	MENU,
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
	static bool& GetWireframeEnabled();
	static bool IsWireframeToggled();

private:

	static bool WireframeToggled;
	static bool WireframeEnabled;

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