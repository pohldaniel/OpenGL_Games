#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpData.h>
#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpTexture.h>

#define WATER_WIDTH 256
#define WATER_HEIGHT 256
#define CAUSTICS_SIZE 1024
#define SPHERE_RADIUS 0.25f
#define SPHERE_DETAIL 10
#define WATER_SURFACE_DETAIL 200
#define SPHERE_DENSITY  1.1f

class WaterSimulation : public State, public MouseEventListener, public KeyboardEventListener {
	struct PendingDrop{
		float x;
		float z;
		float radius;
		float strength;
		bool pending;
	};
public:

	WaterSimulation(StateMachine& machine);
	~WaterSimulation();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;
	void resize(int deltaW, int deltaH) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsDrop();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsUpdate();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsNormal();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsSphereMove();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsCaustics();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsPool();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsWater();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsSphere();

	void createBindGroupPool();
	void createBindGroupCaustic();
	void createBindGroupWater();
	void createBindGroupSphere();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void run_simulation_pass(WGPURenderPipeline pipeline, WGPUBuffer uniform_buffer, uint32_t uniform_size);
	void water_add_drop(float x, float y, float radius, float strength);
	float random_float_min_max(float min, float max);
	float random_float();
	bool getWorldPosition(int xPos, int yPos, const Vector3f& planeNormal, Vector3f& outIntersection);
	Vector3f getRay(int xPos, int yPos);
	void water_move_sphere(const Vector3f& oldCenter, const Vector3f& newCenter, float radius);
	bool intersectDragPlane(int mouseX, int mouseY, const Vector3f& planePoint, const Vector3f& planeNormal, Vector3f& outHit);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_useA = true;

	Camera m_camera;
	Uniforms m_uniforms;
	PendingDrop m_pendingDrop;
	TrackBall m_trackball;

	WgpBuffer m_uniformBuffer, m_lightBuffer, m_sphereBuffer, m_sphereMoveBuffer, m_shadowBuffer, m_dropBuffer, m_updateBuffer;
	WgpBuffer m_poolVertex, m_poolIndex, m_waterVertex, m_waterIndex, m_sphereVertex, m_sphereIndex;
	WgpTexture m_wgpTextureCube, m_wgpTextureTiles, m_textureA, m_textureB, m_textureCaustics;
	WGPUBindGroup m_bindGoupPoolA, m_bindGoupPoolB, m_bindGroupCausticA, m_bindGroupCausticB, m_bindGroupWaterA, m_bindGroupWaterB, m_bindGroupSphereA, m_bindGroupSphereB;

	bool m_isDraggingSphere = false;
	Vector3f m_prevHit;
	Vector3f m_dragPlaneNormal;
	Vector3f m_sphereCenter = Vector3f(-0.4f, -0.75f, 0.2f);
	Vector3f m_oldSphereCenter = Vector3f(-0.4f, -0.75f, 0.2f);
	Vector3f m_sphereVelocity = Vector3f(0.0f, 0.0f, 0.0f);
	bool m_wasHorizontalDrag = false;

	static uint16_t floatToHalf(float value);
	static bool hit_test_sphere(const Vector3f& origin, const Vector3f& ray, const Vector3f& center, float radius, float* t_out, Vector3f& hit_out);
};