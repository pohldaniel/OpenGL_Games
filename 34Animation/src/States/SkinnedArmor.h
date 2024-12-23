#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/animation/AssimpAnimatedModel.h>
#include <engine/MeshObject/Shape.h>
#include <engine/scene/SceneNode.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>
#include <States/StateMachine.h>

#include <engine/animationNew/AnimatedModel.h>
#include <engine/animationNew/AnimationController.h>
#include <Utils/SolidIO.h>
#include "CharacterSkinned.h"

class SkinnedArmor : public State, public MouseEventListener, public KeyboardEventListener {

public:

	SkinnedArmor(StateMachine& machine);
	~SkinnedArmor();

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
	AnimationController* m_animationController;
	CharacterSkinned m_character;
	float m_offsetDistance = 10.0f;
	
};