#pragma once

#include <vulkan/vulkan.h>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>
#include <engine/AssimpModel.h>
#include <engine/TrackBall.h>
#include <States/StateMachine.h>

#include <Vulkan/Data.h>
#include <Vulkan/VlkTexture.h>
#include <Vulkan/VlkBuffer.h>
#include <Vulkan/VlkMesh.h>

enum Model {
	DRAGON,
	VIKING_ROOM
};

class Default : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Default(StateMachine& machine);
	~Default();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const VkCommandBuffer& vkCommandbuffer);

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	void renderUi();
	void applyTransformation(TrackBall& arc);
	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_drawWirframe = false;

	Camera m_camera;
	UniformBufferObject m_ubo;
	ObjModel m_model;
	AssimpModel m_modelAssimp;

	std::list<VlkTexture> m_textures;
	std::list<VlkBuffer> m_vertexBuffer;
	std::list<VlkBuffer> m_indexBuffer;
	std::list<VlkMesh> m_dragon;
	std::list<VlkMesh> m_vikingRoom;

	Model model = Model::DRAGON;
	TrackBall m_trackball;
	glm::mat4 m_modelMtx;

	VkBuffer vkBufferUniform;
	VkDeviceMemory vkDeviceMemoryUniform;
	void* uniformMappingMVP;
};