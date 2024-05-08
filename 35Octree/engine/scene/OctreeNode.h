#pragma once

#include "../scene/SceneNodeLC.h"
#include "../BoundingBox.h"

class Octant;
class OctreeNode : public SceneNodeLC {

	friend class Octree;

public:

	OctreeNode(const BoundingBox& localBoundingBox);
	~OctreeNode();

	void OnTransformChanged() override;

	virtual void OnBoundingBoxChanged();
	virtual void OnWorldBoundingBoxUpdate() const;	
	virtual void OnOctreeUpdate();
	virtual void OnPrepareRender(float dt, unsigned short frameNumber);
	virtual void OnOctreeSet(Octree* octree);

	void OnRenderOBB(const Vector4f& color = { 1.0f, 0.0f, 0.0f, 1.0f });
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });
	
	void removeFromOctree();

	const BoundingBox& getWorldBoundingBox() const;
	const BoundingBox& getLocalBoundingBox() const;

	Octant* getOctant() const;
	Octree* getOctree() const;
	void setDrawDebug(bool drawDebug);
	bool wasInView(unsigned short frameNumber) const;

protected:

	mutable BoundingBox m_worldBoundingBox;
	mutable bool m_worldBoundingBoxDirty;
	bool m_drawDebug;
	mutable bool m_octreeUpdate;
	mutable bool m_reinsertQueued;

	Octant* m_octant;
	Octree* m_octree;

private:

	const BoundingBox& localBoundingBox;
	unsigned short m_lastFrameNumber;
};