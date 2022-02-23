#pragma once
#include <stack>

#include "Constants.h"
#include "Extension.h"
#include "Vector.h"
#include "Texture.h"
#include "Shader.h"
#include "Quad.h"

//Base for holding the Framebuffer
class State;

class StateMachine {
public:
	StateMachine(const float& dt, const float& fdt);
	~StateMachine();

	void AddStateAtTop(State* state);
	void AddStateAtBottom(State* state);

	void FixedUpdate();
	void Update();
	void Render();

	void ClearAndPush(State* state);

	//sf::RenderWindow& m_window;
	const float& m_fdt;
	const float& m_dt;

private:
	//sf::Sprite		   m_frameSprite;
	//sf::RenderTexture  m_frame;
	std::stack<State*> m_states;

	Quad *m_quad;
	Shader *m_shader;

	void ClearStates();

	unsigned int m_frameTexture;
	unsigned int m_frameBuffer;
};

class State {
public:
	State(StateMachine& machine);
	virtual ~State();

	virtual void FixedUpdate() = 0;
	virtual void Update() = 0;
	virtual void Render(unsigned int &m_frameBuffer) = 0;

	const bool IsRunning() const;

protected:
	StateMachine& i_machine;
	const float& i_fdt;
	const float& i_dt;
	bool i_isRunning = true;
};