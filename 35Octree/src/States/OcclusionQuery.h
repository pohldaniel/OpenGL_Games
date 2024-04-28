#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

#include "Skybox.h"

class OcclusionQuery : public State, public MouseEventListener, public KeyboardEventListener {

	struct Vertex {
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
	};

public:

	OcclusionQuery(StateMachine& machine);
	~OcclusionQuery();

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
	void prepareStaticSceneObjects();

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	Skybox m_skybox;
	Matrix4f m_modelMatrices[3][3][3];

	unsigned int m_vao, m_vaoOcc;
	unsigned int m_vbo, m_vboOcc;
	unsigned int v_ibo;
	float fCubeHalfSize = 30.0f;
};