#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/animation/AssimpAnimatedModel.h>
#include <engine/scene/SceneNode.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>
#include <States/StateMachine.h>

#include <Animation/AnimatedModel.h>
#include <Animation/AnimationController.h>

struct CurrentAnimation {
	std::string name;
	float fadeLength = 0.0f;
	AnimationState* state = nullptr;
};

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

	void renderUi();

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	AnimatedModel vampire, beta, cowboy, mushroom, dragon, woman;
	AnimationController* m_animationController;

	bool m_playAnimation = true;
	float m_weightLean = 1.0f;
	float m_weightWalk = 1.0f;
	float m_weightRun = 1.0f;
	
	float m_weightLeft = 1.0f;
	float m_weightRight = 1.0f;

	float m_length = 0.0f;

	CurrentAnimation m_currentAnimation;
};