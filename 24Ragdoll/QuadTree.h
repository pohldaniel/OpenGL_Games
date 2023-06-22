#pragma once
#include "engine/Vector.h"
#include "engine/Frustum.h"

class AABB {

public:

	AABB();
	~AABB();

	void set(const Vector3f& min, const Vector3f& max);
	bool pointInside(const Vector3f& point);
	bool visible(Frustum &frustum);
	float distance(Frustum &frustum);
	void draw();

private:
	Vector3f vertices[8];
};

// ----------------------------------------------------------------------------------------------------------------------------

class TreeNode {

public:

	TreeNode();
	~TreeNode();

	void initAABB(const Vector3f& min, const Vector3f& max, int depth, float minAABBSize);
	bool checkTriangle(Vector3f* vertices, unsigned int* indices, unsigned int a, unsigned int b, unsigned int c);
	void allocateMemory();
	bool addTriangle(Vector3f* vertices, unsigned int* indices, unsigned int a, unsigned int b, unsigned int c);
	void resetAABB(Vector3f* vertices);
	int initIndexBufferObject();
	int checkVisibility(TreeNode** visibleGeometryNodes, int& visibleGeometryNodesCount);
	float getDistance();
	void draw();
	void drawAABB(int depth);
	void destroy();

private:

	void setDefaults();

	Vector3f min, max;
	int depth;
	AABB aabb;
	bool visible;
	float distance;
	int *indices;
	int indicesCount;
	unsigned int indexBufferObject;
	TreeNode *children[2];
};

// ----------------------------------------------------------------------------------------------------------------------------

class QuadTree {

	friend class TreeNode;

public:
	QuadTree();
	~QuadTree();

	void init(Vector3f* vertices, unsigned int* indices, unsigned int indicesCount, const Vector3f& min, const Vector3f& max, float minAABBSize = 16.0f);
	void quickSortVisibleGeometryNodes(int left, int right);
	int checkVisibility(bool sortVisibleGeometryNodes = false);
	void draw(bool visualizeRenderingOrder = false) const;
	void drawAABB(int depth);
	void destroy();

	static Frustum Frustum;

private:

	void setDefaults();

	TreeNode *root;
	TreeNode **visibleGeometryNodes;
	int visibleGeometryNodesCount;
};