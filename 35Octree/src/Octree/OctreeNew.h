#pragma once

#include <vector>
#include <engine/scene/Node.h>
#include <engine/scene/ShapeNode.h>
#include <engine/BoundingBox.h>

#include "AutoPtr.h"
#include "WorkQueue.h"

static const size_t NUM_OCTANTS = 8;

class Octant{

	friend class Octree;

public:


	Octant();
	~Octant();

	void Initialize(Octant* parent, const BoundingBox& boundingBox, unsigned char level, unsigned char childIndex);
	//void OnRenderOBB(const Vector4f& color = { 1.0f, 0.0f, 0.0f, 1.0f });
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });
	const BoundingBox& getCullingBox() const;
	void markCullingBoxDirty();
	bool FitBoundingBox(const BoundingBox& box, const Vector3f& boxSize) const;
	unsigned char ChildIndex(const Vector3f& position) const;

private:

	mutable BoundingBox cullingBox;
	std::vector<ShapeNode*> drawables;
	BoundingBox fittingBox;
	Vector3f center;
	Vector3f halfSize;
	Octant* children[NUM_OCTANTS];
	Octant* parent;

	unsigned char numChildren;
	unsigned char level;
	unsigned char childIndex;
	bool m_drawDebug;

	mutable bool m_cullingBoxDirty;
	mutable bool m_sortDirty;
};

struct ReinsertDrawablesTask : public MemberFunctionTask<Octree>{
	ReinsertDrawablesTask(Octree* object_, MemberWorkFunctionPtr function_) : MemberFunctionTask<Octree>(object_, function_){
	}

	ShapeNode** start;
	ShapeNode** end;
};

class Octree : public Node {

public:
	Octree();
	~Octree();

	void Update(unsigned short frameNumber);
	void FinishUpdate();

	void Resize(const BoundingBox& boundingBox, int numLevels);
	void QueueUpdate(ShapeNode* drawable);
	void CollectDrawables(std::vector<ShapeNode*>& result, Octant* octant) const;

	//void OnRenderOBB(const Vector4f& color = { 1.0f, 0.0f, 0.0f, 1.0f });
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });
	void AddDrawable(ShapeNode* drawable, Octant* octant);
	void RemoveDrawable(ShapeNode* drawable);
	void RemoveDrawable(ShapeNode* drawable, Octant* octant);
	void RemoveDrawableFromQueue(ShapeNode* drawable, std::vector<ShapeNode*>& drawables);
	void ReinsertDrawables(std::vector<ShapeNode*>& drawables);

	Octant* CreateChildOctant(Octant* octant, unsigned char index);
	void DeleteChildOctant(Octant* octant, unsigned char index);
	void DeleteChildOctants(Octant* octant, bool deletingOctree);
	void SetThreadedUpdate(bool enable);
	bool GetThreadedUpdate() const;
	void CheckReinsertWork(TaskOct* task, unsigned threadIndex);

	volatile bool m_threadedUpdate;
	unsigned short m_frameNumber;

	Octant root;
	WorkQueue* workQueue;

	std::vector<AutoPtr<ReinsertDrawablesTask>> reinsertTasks;
	AutoArrayPtr<std::vector<ShapeNode*>> reinsertQueues;
	std::atomic<int> numPendingReinsertionTasks;
	std::vector<Octant*> sortDirtyOctants;
	std::vector<ShapeNode*> updateQueue;
};

