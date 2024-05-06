#pragma once

#include "../scene/SceneNode.h"
#include "../scene/SceneNodeLC.h"
#include "../MeshObject/Shape.h"
#include "../BoundingBox.h"


class Octant;
class OctantTu;
class ShapeNode : public SceneNodeLC {

public:

	ShapeNode(const Shape& shape);
	~ShapeNode();

	void OnTransformChanged() override;

	void OnWorldBoundingBoxUpdate() const;
	void OnOctreeUpdate() const;
	void OnBoundingBoxChanged() const;
	void OnRenderOBB(const Vector4f& color = { 1.0f, 0.0f, 0.0f, 1.0f });
	void OnRenderAABB(const Vector4f& color = { 0.0f, 1.0f, 0.0f, 1.0f });

	using SceneNodeLC::addChild;
	void addChild(ShapeNode* node, bool drawDebug);

	const BoundingBox& getWorldBoundingBox() const;
	const BoundingBox& getLocalBoundingBox() const;	
	const Shape& getShape() const;
	void setDrawDebug(bool drawDebug);
	Octant* getOctant() const;
	OctantTu* getOctantTu() const;

	Octant* m_octant;
	OctantTu* m_octantTu;
	mutable bool m_octreeUpdate;
	mutable bool m_reinsertQueued;

protected:

	const Shape& shape;

private:

	mutable BoundingBox m_worldBoundingBox;
	mutable bool m_worldBoundingBoxDirty;
	bool m_drawDebug;

	const BoundingBox& localBoundingBox;

};