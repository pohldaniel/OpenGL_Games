#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/TrackBall.h>
#include <engine/Clock.h>
#include <engine/Pixelbuffer.h>
#include <engine/Framebuffer.h>
#include <engine/Spritesheet.h>
#include <engine/ZoomableQuad.h>
#include <engine/ObjModel.h>
#include <engine/SlicedCube.h>

#include <States/StateMachine.h>
#include "Background.h"

#define NUM_INSTANCES 16
#define NUM_SDFS 16

struct Instance {
	// Mesh
	ObjModel			mesh;
	Vector3f			color;

	// SDF
	Texture				sdf;
	std::array<int, 3>	volume_size;
	Vector3f			grid_origin;
	float				grid_step_size;
	Vector3f			min_extents;
	Vector3f			max_extents;

	// Transform
	bool	 animate = false;
	float	 rotation = 0.0f;
	Vector3f position = Vector3f(0.0f);
	Matrix4f transform = Matrix4f::IDENTITY;
};

struct GlobalUniforms {
	Matrix4f view_proj;
	Vector4f cam_pos;
	int32_t num_instances;
};

struct SamplerPack {
	std::uint64_t value;
	std::uint8_t padding[8];
};

struct InstanceUniforms {
	Matrix4f inverse_transform;
	Vector4f half_extents;
	Vector4f os_center;
	Vector4f ws_center;
	Vector4f ws_axis[3];
	std::array<int, 4> sdf_idx;
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

	void applyTransformation(TrackBall& arc);
	void renderUi();
	void bake_sdf(Instance& instance, float grid_step_size, int padding);
	void updateUbo();
	void update_transforms();

	bool m_initUi = true;
	bool m_drawUi = true;
	
	Background m_background;
	TrackBall m_trackball;
	Transform m_transform;
	Camera m_camera;

	std::vector<Instance> m_instances;
	SlicedCube m_slicedCube;
	std::vector<InstanceUniforms> m_instanceUniforms;
	std::vector<uint64_t> m_textureUniforms;
	GlobalUniforms m_globalUniforms;
	ObjModel m_ground;

	unsigned int m_globalUbo = 0;
	unsigned int m_instanceUbo = 0;
	unsigned int m_sdfUbo = 0;


	// Light
	float     m_light_pitch = -0.9f;
	Vector3f  m_light_pos = Vector3f(0.0f, 30.0f, 35.0f);
	float     m_light_inner_cutoff = 3.4f;
	float     m_light_outer_cutoff = 8.7f;
	float     m_light_range = 100.0f;

	// AO
	bool  m_ao = true;
	float m_ao_strength = 0.16f;
	float m_ao_step_size = 0.15f;
	int   m_ao_num_steps = 8;

	// SDF
	float m_t_min = 0.2f;
	float m_t_max = 100.0f;
	bool  m_soft_shadows = true;
	float m_soft_shadows_k = 5.7f;
	bool  m_draw_bounding_boxes = false;

	void renderMesh(const ObjModel* mesh, const Matrix4f& model, const Vector3f& color);
};