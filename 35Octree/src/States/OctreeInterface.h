#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/MeshObject/Shape.h>
#include <engine/Camera.h>
#include <engine/TrackBall.h>
#include <States/StateMachine.h>

#include "Octree/Octree.h"
#include "Octree/ThreadUtils.h"

class OctreeInterface : public State, public MouseEventListener, public KeyboardEventListener {

	static const size_t NUM_OCTANT_TASKS = 9;
	struct CollectOctantsTask : public MemberFunctionTask<OctreeInterface> {

		CollectOctantsTask(OctreeInterface* object_, MemberWorkFunctionPtr function_) : MemberFunctionTask<OctreeInterface>(object_, function_) { }

		Octant* startOctant;
		size_t resultIdx;
	};

	struct CollectBatchesTask : public MemberFunctionTask<OctreeInterface> {

		CollectBatchesTask(OctreeInterface* object_, MemberWorkFunctionPtr function_) : MemberFunctionTask<OctreeInterface>(object_, function_) { }

		std::vector<std::pair<Octant*, unsigned char>> octants;
	};

	struct ThreadOctantResult {

		void Clear();
		size_t drawableAcc;
		size_t taskOctantIdx;
		size_t batchTaskIdx;
		std::vector<std::pair<Octant*, unsigned char>> octants;
		std::vector<AutoPtr<CollectBatchesTask>> collectBatchesTasks;
	};


public:

	OctreeInterface(StateMachine& machine);
	~OctreeInterface();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void updateOctree();


	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;

private:

	void renderUi();

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;

	bool m_initUi = true;
	bool m_drawUi = true;
	
	Octree* m_octree;
	AutoPtr<WorkQueue> workQueue;

	std::vector<Octant*> rootLevelOctants;
	AutoArrayPtr<OctreeInterface::ThreadOctantResult> octantResults;
	std::atomic<int> numPendingBatchTasks;

	AutoPtr<OctreeInterface::CollectOctantsTask> collectOctantsTasks[NUM_OCTANT_TASKS];
	void CollectOctantsWork(Task* task, unsigned threadIndex);
	void CollectOctants(Octant* octant, ThreadOctantResult& result, unsigned char planeMask = 0x3f);

	ShapeNode *m_cube;
};
