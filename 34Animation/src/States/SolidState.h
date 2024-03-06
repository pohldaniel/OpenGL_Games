#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/animation/AssimpAnimatedModel.h>
#include <engine/MeshObject/Shape.h>
#include <engine/scene/SceneNode.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>
#include <States/StateMachine.h>
#include <Windows.h>
#include <Utils/SolidIO.h>
#include "Skeleton.h"

#define MAX_TEXTURES 4									
#define MAX_VERTICES 500

struct CVector3 {
	float x, y, z;
};

struct CVector2 {
	float x, y;
};

struct CObject {
	CVector3 *m_pVertices;
	CVector2 *m_pTextureCoords;
	unsigned int *m_pIndices;
	int m_numOfTriangles;
};

struct CVertex {
	CVector2 m_textureCoord;
	CVector3 m_vertex;
};

class SolidState : public State, public MouseEventListener, public KeyboardEventListener {

public:

	SolidState(StateMachine& machine);
	~SolidState();

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
	void RenderScene();

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;

	Utils::SolidIO solidConverter;
	Shape m_body;
	Skeleton m_skeleton;

};