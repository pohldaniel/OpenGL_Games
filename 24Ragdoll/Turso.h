#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "StateMachine.h"

#include "Graphics/FrameBufferTu.h"
#include "Graphics/Graphics.h"
#include "Graphics/TextureTu.h"
#include "Graphics/UniformBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/VertexBuffer.h"
#include "Input/InputTu.h"
#include "IO/Arguments.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"
#include "IO/StringUtils.h"
#include "Math/Math.h"
#include "Math/RandomTu.h"
#include "Renderer/AnimatedModel.h"
#include "Renderer/Animation.h"
#include "Renderer/AnimationState.h"
#include "Renderer/CameraTu.h"
#include "Renderer/DebugRenderer.h"
#include "Renderer/Light.h"
#include "Renderer/LightEnvironment.h"
#include "Renderer/MaterialTu.h"
#include "Renderer/Model.h"
#include "Renderer/Octree.h"
#include "Renderer/Renderer.h"
#include "Resource/ResourceCache.h"
#include "Renderer/StaticModel.h"
#include "Scene/Scene.h"
#include "Time/TimerTu.h"
#include "Time/Profiler.h"
#include "Thread/ThreadUtils.h"

class TursoInterface : public State, public MouseEventListener, public KeyboardEventListener, public ObjectTu {

	OBJECT(TursoInterface);

public:

	TursoInterface(StateMachine& machine);
	~TursoInterface();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void renderDirect();
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

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;

	bool m_initUi = true;
	bool m_drawUi = true;


	AutoPtr<WorkQueue> workQueue;
	AutoPtr<Profiler> profiler;
	AutoPtr<Log> log;
	AutoPtr<ResourceCache> cache;
	AutoPtr<Graphics> graphics;

	//AutoPtr<InputTu> input;
	AutoPtr<Renderer> renderer;
	AutoPtr<DebugRenderer> debugRenderer;


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

	float yaw = 0.0f, pitch = 20.0f;
	HiresTimer frameTimer;
	TimerTu profilerTimer;
	//float dt = 0.0f;
	float angle = 0.0f;
	int shadowMode = 1;
	bool drawSSAO = false;
	bool useOcclusion = true;
	bool animate = true;
	bool drawDebug = false;
	bool drawShadowDebug = false;
	bool drawOcclusionDebug = false;

	std::string profilerOutput;

	std::vector<StaticModel*> rotatingObjects;
	std::vector<AnimatedModel*> animatingObjects;

	EventTu eventTu;
};
