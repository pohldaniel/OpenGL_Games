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

struct Vertex2 {
	Vector3f pos;
	Vector2f tex;
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
	void updateModel();
	void updateModel2();

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;

	Utils::SolidIO solidConverter;
	Shape m_body, m_body2;
	Skeleton m_skeleton;
	void load();

	GLuint id;
	std::string filename = "res/textures/FurBlackWhite.jpg";
	bool hasMipmap = false;
	bool isSkin = true;
	int skinsize;
	GLubyte* data;
	int datalen;

	std::vector<float> m_vertexBuffer;
	std::vector<float> vertexBuffer;
	Matrix4f* m_skinMatrices;
	Utils::Skeleton m_skeleton2;
	
	std::vector<std::array<float, 4>> m_weights;
	std::vector<std::array<unsigned int, 4>> m_boneIds;

	Vertex2* buffer;
	Vertex2* bufferPtr;
	unsigned int m_vao = 0u;
	unsigned int m_vbo[3] = { 0u };
	unsigned int m_drawCount;
};