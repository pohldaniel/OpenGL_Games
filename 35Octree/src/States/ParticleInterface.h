#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/scene/SceneNodeLC.h>
#include <engine/Camera.h>
#include <engine/AssimpModel.h>
#include <engine/ObjModel.h>
#include <engine/Framebuffer.h>
#include <States/StateMachine.h>
#include "Skybox.h"

#define MAXPARTICLES 500

struct LinearSpline {
	std::vector<std::array<float, 2>> m_points;

	void addPoint(const float t, const float p) {
		m_points.push_back({ t, p });
	}

	float get(float t) {
		int index1 = 0;

		for (int i = 0; i < m_points.size(); i++) {
			if (m_points[i][0] >= t) {
				break;
			}
			index1 = i;
		}

		unsigned int index2 = std::min(static_cast<int>(m_points.size()) - 1, index1 + 1);
		if (index1 == index2) {
			return m_points[index1][1];
		}

		float _t = (t - m_points[index1][0]) / (m_points[index2][0] - m_points[index1][0]);
		return Math::Lerp(m_points[index1][1], m_points[index2][1], _t);
	}
};

struct ColorSpline {
	std::vector< std::tuple<float, Vector4f>> m_points;

	void addPoint(const float t, const Vector4f& p) {
		m_points.push_back({ t, p });
	}

	Vector4f get(float t) {
		int index1 = 0;

		for (int i = 0; i < m_points.size(); i++) {
			if (std::get<0>(m_points[i]) >= t) {
				break;
			}
			index1 = i;
		}

		unsigned int index2 = std::min(static_cast<int>(m_points.size()) - 1, index1 + 1);
		if (index1 == index2) {
			return std::get<1>(m_points[index1]);
		}

		float _t = (t - std::get<0>(m_points[index1])) / (std::get<0>(m_points[index2]) - std::get<0>(m_points[index1]));
		Vector4f res = Math::Lerp(std::get<1>(m_points[index1]), std::get<1>(m_points[index2]), _t);
		res[3] = _t;
		return res;
	}
};

struct ParticleVertex {
	Vector3f position;
	float size;
	Vector4f color;
	float angle;
};

struct ParticleNew {	
	Vector3f position;
	Vector3f velocity;
	Vector4f color;
	float size;
	float currentSize;
	float life;
	float maxLife;
	float angle;
	float alpha;
	float cameraDistance;
};

class ParticleInterface : public State, public MouseEventListener, public KeyboardEventListener {

public:

	ParticleInterface(StateMachine& machine);
	~ParticleInterface();

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
	void addParticles(float dt, int count = 0);
	void updateParticles(float dt);
	void updateGeometry();

private:

	void renderUi();
	float getRand();

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_addParticle = true;

	Camera m_camera;
	AssimpModel m_rocket;	
	Skybox m_skybox;

	LinearSpline m_alphaSpline, m_sizeSpline;
	ColorSpline m_colorSpline;

	unsigned int m_vao, m_vbo;

	float gdfsghk = 0.0f;

	std::vector<ParticleNew> m_particles;
	ParticleVertex* particleBatch;
	ParticleVertex* particleBatchPtr;
	uint32_t m_particleCount = 0;
};