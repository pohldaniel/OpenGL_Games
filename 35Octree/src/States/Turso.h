#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "StateMachine.h"

#include <turso3d/Graphics/FrameBufferTu.h>
#include <turso3d/Graphics/Graphics.h>
#include <turso3d/Graphics/TextureTu.h>
#include <turso3d/Graphics/UniformBuffer.h>
#include <turso3d/Graphics/IndexBuffer.h>
#include <turso3d/Graphics/VertexBuffer.h>
#include <turso3d/Input/InputTu.h>
#include <turso3d/IO/Arguments.h>
#include <turso3d/IO/FileSystem.h>
#include <turso3d/IO/Log.h>
#include <engine/utils/StringUtils.h>
#include <turso3d/Math/Math.h>
#include <turso3d/Math/RandomTu.h>
#include <turso3d/Renderer/AnimatedModelTu.h>
#include <turso3d/Renderer/AnimationTu.h>
#include <turso3d/Renderer/AnimationStateTu.h>
#include <turso3d/Renderer/CameraTu.h>
#include <turso3d/Renderer/DebugRendererTu.h>
#include <turso3d/Renderer/Light.h>
#include <turso3d/Renderer/LightEnvironment.h>
#include <turso3d/Renderer/MaterialTu.h>
#include <turso3d/Renderer/Model.h>
#include <turso3d/Renderer/OctreeTu.h>
#include <turso3d/Renderer/Renderer.h>
#include <turso3d/Resource/ResourceCache.h>
#include <turso3d/Renderer/StaticModel.h>
#include <turso3d/Scene/Scene.h>
#include <turso3d/Time/TimerTu.h>
#include <turso3d/Time/Profiler.h>
#include <Octree/ThreadUtils.h>

class Turso : public State, public MouseEventListener, public KeyboardEventListener, public ObjectTu {

	OBJECT(Turso);

public:

	Turso(StateMachine& machine);
	~Turso();

	void fixedUpdate() override;
	void update() override;
	void render() override;

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;

	void CreateScene(Scene* scene, CameraTu* camera, int preset);
	void HandleUpdate(EventTu& eventType);
private:

	void renderUi();

	//Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;

	bool m_initUi = true;
	bool m_drawUi = true;


	AutoPtr<WorkQueueTu> workQueue;
	AutoPtr<Profiler> profiler;
	AutoPtr<Log> log;
	AutoPtr<ResourceCache> cache;
	AutoPtr<Graphics> graphics;

	AutoPtr<Renderer> renderer;
	AutoPtr<DebugRendererTu> debugRenderer;


	AutoPtr<FrameBufferTu> viewFbo;
	AutoPtr<FrameBufferTu> viewMRTFbo;
	AutoPtr<FrameBufferTu> ssaoFbo;
	AutoPtr<TextureTu> colorBuffer;
	AutoPtr<TextureTu> normalBuffer;
	AutoPtr<TextureTu> depthStencilBuffer;
	AutoPtr<TextureTu> ssaoTexture;
	AutoPtr<TextureTu> occlusionDebugTexture;

	AutoPtr<TextureTu> noiseTexture;

	SharedPtr<Scene> scene;
	SharedPtr<CameraTu> camera;

	float yaw = 0.0f, pitch = 0.0f;
	HiresTimer frameTimer;
	TimerTu profilerTimer;
	float angle = 0.0f;
	int shadowMode = 1;

	bool useOcclusion = true;
	bool animate = true;
	bool drawDebug = true;
	bool drawShadowDebug = false;
	bool drawOcclusionDebug = false;

	std::string profilerOutput;

	std::vector<StaticModel*> rotatingObjects;
	std::vector<AnimatedModelTu*> animatingObjects;

	EventTu eventTu;
	SceneNodeLC* m_root;
	std::vector<ShapeNode*> m_entities;
	OctreeTu* m_octree;
};
