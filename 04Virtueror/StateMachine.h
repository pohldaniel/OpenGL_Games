#pragma once
#include <stack>

#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/Extension.h"
#include "engine/Vector.h"
#include "engine/Texture.h"
#include "engine/Shader.h"
#include "engine/input/MouseEventListener.h"
#include "engine/Quad.h"

#include "Constants.h"
#include "MapLoader.h"

enum CurrentState {
	GAME,
	SETTINGS,
	PAUSE,
	MENU,
	LEVELSELECT
};

//base for holding the Framebuffer
class State;

class StateMachine : public MouseEventListener {
	friend class Application;
public:
	StateMachine(const float& dt, const float& fdt);
	~StateMachine();

	void addStateAtTop(State* state);
	void addStateAtBottom(State* state);

	void fixedUpdate();
	void update();
	void render();
	void resize(unsigned int width, unsigned int height);

	void clearAndPush(State* state);
	const bool isRunning() const;

	const float& m_fdt;
	const float& m_dt;

	MapLoader m_mapLoader;

private:
	void clearStates();
	void OnMouseMotion(Event::MouseMoveEvent& event) override;

	std::stack<State*> m_states;
	Quad *m_quad;
	
	std::vector<Matrix4f> m_transforms;
	Spritesheet *m_spriteSheet;

	Shader *m_shader;
	Shader *m_shaderArray;
	Shader *m_shaderLevel;

	unsigned int m_frameTexture;
	unsigned int m_frameBuffer;
	unsigned int m_rbDepthStencil;
	bool m_isRunning = true;

	Matrix4f m_view = Matrix4f::IDENTITY;
	

	Matrix4f m_transform = Matrix4f::IDENTITY;
	Matrix4f m_transform2 = Matrix4f::IDENTITY;

	Matrix4f m_projection = Matrix4f::IDENTITY;

	float m_offsetX = -3.0f;
	float m_offsetY = -2.0f;

	float dist = 4.7f;
	float scale = 0.65f;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_vboMap = 0;
	unsigned int m_ibo = 0;
};

class State {
public:
	State(StateMachine& machine, CurrentState currentState);
	virtual ~State();

	virtual void fixedUpdate() = 0;
	virtual void update() = 0;
	virtual void render(unsigned int &m_frameBuffer) = 0;

	const bool isRunning() const;
	CurrentState m_currentState;
protected:
	StateMachine& m_machine;
	const float& m_fdt;
	const float& m_dt;
	bool m_isRunning = true;
};