#pragma once

#include "../scene/SceneNodeLC.h"
#include "../BoundingBox.h"

class Octant;
class OctreeNode : public SceneNodeLC {

	friend class Octree;

public:

	OctreeNode();

	virtual void OnTransformChanged() override;
	virtual void OnOctreeSet(Octree* octree);

	virtual void drawRaw() const;
	virtual void OnRenderDebug() const;

	void OnRenderOBB(const Vector4f& color = { 1.0f, 0.0f, 0.0f, 1.0f });
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });

	void removeFromOctree();

	const BoundingBox& getWorldBoundingBox() const;
	virtual const BoundingBox& getLocalBoundingBox() const = 0;

	Octant* getOctant() const;
	Octree* getOctree() const;
	void setDrawDebug(bool drawDebug);
	void setLastFrameNumber(unsigned short lastFrameNumber);
	bool wasInView(unsigned short frameNumber) const;
	void setSortKey(int sortKey);
	const int getSortKey() const;

protected:

	virtual void OnBoundingBoxChanged();
	virtual void OnWorldBoundingBoxUpdate() const;
	virtual void OnOctreeUpdate();

	mutable BoundingBox m_worldBoundingBox;
	mutable bool m_worldBoundingBoxDirty;
	bool m_drawDebug;
	mutable bool m_octreeUpdate;
	mutable bool m_reinsertQueued;
	unsigned short* frameNumber;
	unsigned short m_lastFrameNumber;
	Octant* m_octant;
	Octree* m_octree;

private:

	//const BoundingBox& localBoundingBox;
	int m_sortKey;
};