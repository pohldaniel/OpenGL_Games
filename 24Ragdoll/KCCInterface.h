#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/MeshObject/Shape.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "StateMachine.h"
#include "KinematicCharacterController.h"
#include "Character.h"
#include "MovingPlatform.h"
#include "SplinePlatform.h"
#include "ShapeDrawer.h"
#include "KinematicObject.h"
#include "StaticTrigger.h"
#include "KinematicTrigger.h"
#include "SolidIO.h"
#include "Lift.h"

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
#include "Renderer/AnimationController.h"
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
#include "Scene/SplinePath.h"
#include "Time/TimerTu.h"
#include "Time/Profiler.h"
#include "Thread/ThreadUtils.h"




class KCCInterface : public State, public MouseEventListener, public KeyboardEventListener, public ObjectTu {

	struct CollectOctantsTask : public MemberFunctionTask<KCCInterface> {

		CollectOctantsTask(KCCInterface* object_, MemberWorkFunctionPtr function_) : MemberFunctionTask<KCCInterface>(object_, function_) { }

		Octant* startOctant;
		size_t resultIdx;
	};

	struct CollectBatchesTask : public MemberFunctionTask<KCCInterface>{

		CollectBatchesTask(KCCInterface* object_, MemberWorkFunctionPtr function_) : MemberFunctionTask<KCCInterface>(object_, function_) { }

		std::vector<std::pair<Octant*, unsigned char>> octants;
	};

	struct ThreadOctantResult {

		void Clear();
		size_t drawableAcc;
		size_t taskOctantIdx;
		size_t batchTaskIdx;
		std::vector<std::pair<Octant*, unsigned char>> octants;
		std::vector<AutoPtr<CollectBatchesTask>> collectBatchesTasks;
		std::vector<Octant*> occlusionQueries;
	};

	struct ThreadBatchResult{

		void Clear();
		float minZ;
		float maxZ;
		BoundingBoxTu geometryBounds;
		std::vector<Batch> opaqueBatches;
	};


	OBJECT(KCCInterface);

public:

	KCCInterface(StateMachine& machine);
	~KCCInterface();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void renderDirect();
	void updateOctree();


	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void CreateScene(Scene* scene, CameraTu* camera, int preset);

	void RegisterRendererLibrary();
	void createPhysics();

private:

	void renderUi();

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_drawDebug = false;
	bool m_debugPhysic = false;
	bool m_useCulling = true;

	Octree* m_octree;
	AutoPtr<WorkQueue> workQueue;
	AutoPtr<Log> log;
	AutoPtr<ResourceCache> cache;
	AutoPtr<Graphics> graphics;
	AutoPtr<DebugRenderer> debugRenderer;

	SharedPtr<Scene> scene;
	SharedPtr<CameraTu> camera;

	float angle = 0.0f;
	float m_prevFraction = 1.0f;
	bool animate = true;
	float m_offsetDistance = 10.0f;
	float m_rotationSpeed = 0.1f;

	std::vector<StaticModel*> rotatingObjects;
	std::vector<AnimatedModel*> animatingObjects;
	AnimatedModel* beta = nullptr;

	std::vector<Octant*> rootLevelOctants;
	AutoArrayPtr<KCCInterface::ThreadOctantResult> octantResults;
	AutoArrayPtr<KCCInterface::ThreadBatchResult> batchResults;

	BatchQueue opaqueBatches;
	std::atomic<int> numPendingBatchTasks;

	AutoPtr<KCCInterface::CollectOctantsTask> collectOctantsTasks[NUM_OCTANT_TASKS];
	AutoPtr<UniformBuffer> perViewDataBuffer;
	PerViewUniforms perViewData;

	FrustumTu frustum;
	unsigned viewMask;
	unsigned short frameNumber = 0;
	bool hasInstancing = false;
	std::vector<Matrix3x4> instanceTransforms;

	AutoPtr<VertexBuffer> instanceVertexBuffer;
	std::vector<VertexElement> instanceVertexElements;
	SATData frustumSATData;


	static const size_t DRAWABLES_PER_BATCH_TASK = 128;

	void CollectOctantsWork(Task* task, unsigned threadIndex);
	void CollectBatchesWork(Task* task, unsigned threadIndex);
	void CollectOctants(Octant* octant, ThreadOctantResult& result, unsigned char planeMask = 0x3f);
	void SortMainBatches();
	void UpdateInstanceTransforms(const std::vector<Matrix3x4>& transforms);
	
	void RenderBatches(CameraTu* camera_, const BatchQueue& queue);
	void preStep(btScalar timeStep);
	void postStep(btScalar timeStep);

	BoundingBoxTu geometryBounds;

	WeakPtr<AnimationController> animController;
	WeakPtr<KinematicCharacterController> kinematicCharacter;

	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;
	
	Shape m_diskShape;
	Shape m_liftShape;
	Shape m_baseShape;
	Shape m_liftExteriorShape;
	Shape m_upperFloorShape;
	Shape m_rampShape;
	Shape m_ramp2Shape;
	Shape m_ramp3Shape;
	Shape m_cylinderShape;

	Utils::MdlIO mdlConverter;
	Character* m_character;
	MovingPlatform* m_movingPlatform;
	SplinePlatform* m_splinePlatform;
	Lift* m_lift;

	StaticModel* m_disk;
	StaticModel* m_cylinder;

	KinematicObject* m_kinematicPlatform1;
	KinematicObject* m_kinematicPlatform2;
	KinematicObject* m_kinematicLift;
	KinematicTrigger* m_liftButtonTrigger;

	SplinePath* m_splinePath;

	static void PreTickCallback(btDynamicsWorld* world, btScalar timeStep);
	static void PostTickCallback(btDynamicsWorld* world, btScalar timeStep);
};

