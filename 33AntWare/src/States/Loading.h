#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/scene/Object.h>
#include <engine/Texture.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

#include "Loadingmanager.h"

class Loading : public State {

	struct LoadSceneTask : public MemberFunctionTask<Loading> {
		LoadSceneTask(Loading* object_, MemberWorkFunctionPtr OnProcess_, MemberWorkFunctionPtr OnComplete_) : MemberFunctionTask<Loading>(object_, OnProcess_, OnComplete_) { }
	};

public:

	Loading(StateMachine& machine);
	~Loading();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	
	void OnComplete();
	void OnProcess();

private:

	void renderUi();

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	Texture m_background;
	Object m_sop;
};