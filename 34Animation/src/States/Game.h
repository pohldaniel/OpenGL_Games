#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/animation/AssimpAnimatedModel.h>
#include <engine/scene/SceneNode.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>
#include <States/StateMachine.h>

#include "ModelBone.h"
#include "Bone.h"
#include "AnimationState.h"
#include "Animation.h"

#include "Utils/SolidIO.h"
#include "AnimatedModel.h"

class Game : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	unsigned int m_vao;
	unsigned int m_vbo[3];
	unsigned int m_ibo;

	void renderUi();

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	AnimatedModel animatedModel;
	AnimatedModel beta;

	void UpdateAnimation();
	void UpdateSkinning();
	void UpdateAnimation2();
	void UpdateSkinning2();

	Animation* animation;
	Animation* animation2;
	
	std::vector<std::shared_ptr<AnimationState>> animationStates;
	std::vector<std::shared_ptr<AnimationState>> animationStates2;

	
	bool m_playAnimation = true;

};