#pragma once
#include <engine/interfaces/IStateMachine.h>

enum States {
	MENU,
	WIREFRAME,
	COMPUTE,
	SPECULARITY,
	NORMAL_MAP,
	MSDF_FONT,
	INSTANCED_CUBE,
	IMAGE_BASED_LIGHTING,
	SHADOW_MAPPING,
	SKINNED_MESH,
	COMPUTE_PARTICLE_LOGO,
	PRIMITIVE_PICKING,
	STENCIL_MASK,
	DEFFERED_RENDERING,
	VOLUME_RENDERING,
	OCCLUSION_QUERY,	
	VIDEO_DECODE,
	RENDER_BUNDLES
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
	void popState();
	States getCurrentState();

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