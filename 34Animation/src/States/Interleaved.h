#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/Background.h>

#include <States/StateMachine.h>

class Interleaved : public State, public MouseEventListener, public KeyboardEventListener {

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

	const unsigned short MAX_TEXTURES = 4;
	const unsigned short MAX_VERTICES = 500;

public:

	Interleaved(StateMachine& machine);
	~Interleaved();

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
	void DrawVertexArrays();
	void DrawIndexedVertexArrays();
	void DrawInterleavedVertexArrays();
	void DrawArrayElementVertexArrays();
	void RenderScene();
	void AddTriangle(float xOffset, float yOffset, float zOffset);

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	Background m_background;

	CObject g_Object;
	CVertex g_InterleavedVertices[9];

};