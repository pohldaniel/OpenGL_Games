#pragma once
#include <stack>

#include "Constants.h"
#include "Extension.h"
#include "Vector.h"
#include "Texture.h"
#include "Shader.h"
#include "Quad.h"

//base for holding the Framebuffer
class State;

class StateMachine {
public:
	StateMachine(const float& dt, const float& fdt);
	~StateMachine();

	void addStateAtTop(State* state);
	void addStateAtBottom(State* state);

	void fixedUpdate();
	void update();
	void render();

	void clearAndPush(State* state);

	const float& m_fdt;
	const float& m_dt;

private:

	void clearStates();

	std::stack<State*> m_states;
	Quad *m_quad;
	Shader *m_shader;

	unsigned int m_frameTexture;
	unsigned int m_frameBuffer;
};

class State {
public:
	State(StateMachine& machine);
	virtual ~State();

	virtual void fixedUpdate() = 0;
	virtual void update() = 0;
	virtual void render(unsigned int &m_frameBuffer) = 0;

	const bool isRunning() const;

protected:
	StateMachine& i_machine;
	const float& i_fdt;
	const float& i_dt;
	bool i_isRunning = true;
};