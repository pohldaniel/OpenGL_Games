#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/MeshObject/Shape.h>
#include <engine/octree/Octree.h>
#include <engine/scene/Md2Node.h>
#include <engine/Camera.h>
#include <engine/Background.h>
#include <engine/MeshSequence.h>
#include <engine/Frustum.h>
#include <engine/Md2Model.h>

#include <States/StateMachine.h>

class Md2State : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Md2State(StateMachine& machine);
	~Md2State();

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
	bool m_debugTree = true;

	Camera m_camera;
	Octree* m_octree;
	Frustum m_frustum;
	Md2Model m_hero, m_ripper, m_corpse;
	SceneNodeLC* m_root;

	Md2Node *m_heroNode, *m_heroNode2, *m_ripperNode, *m_corpseNode;
};