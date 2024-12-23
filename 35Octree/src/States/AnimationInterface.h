#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/animationNew/AnimatedModel.h>
#include <engine/scene/AnimationNode.h>
#include <engine/octree/Octree.h>
#include <engine/Vector.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

class AnimationInterface : public State, public MouseEventListener, public KeyboardEventListener {

public:

	AnimationInterface(StateMachine& machine);
	~AnimationInterface();

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
	Frustum m_frustum;
	AnimatedModel m_beta;
	SceneNodeLC* m_root;
	Octree* m_octree;
	std::vector<AnimationNode*> m_entities;

	float m_fovx = 45.0f;
	float m_far = 50.0f;
	float m_near = 0.1f;
	float m_distance = 0.01f;
	bool m_overview = true;
	bool m_useCulling = true;
	bool m_useOcclusion = true;
	bool m_debugTree = false;
	Matrix4f perspective, m_view;
};