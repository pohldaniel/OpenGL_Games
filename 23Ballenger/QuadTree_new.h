#pragma once
#include "engine/Vector.h"

class CPlane {

private:
	Vector3f N;
	float ND;
	int O;

public:
	CPlane();
	~CPlane();

public:
	void Set(const Vector3f& A, const Vector3f& B, const Vector3f& C);
	bool AABBBehind(const Vector3f *AABBVertices);
	float AABBDistance(const Vector3f *AABBVertices);
};

// ----------------------------------------------------------------------------------------------------------------------------

class CFrustum {
private:
	Vector3f Vertices[8];

private:
	CPlane Planes[6];

public:
	CFrustum();
	~CFrustum();

public:
	void Set(const Matrix4f& ViewProjectionMatrixInverse);
	bool AABBVisible(const Vector3f* AABBVertices);
	float AABBDistance(const Vector3f* AABBVertices);
	void Render();
};

class AABB {

public:

	AABB();
	~AABB();

	void set(const Vector3f& min, const Vector3f& max);
	bool pointInside(const Vector3f& point);
	bool visible(CFrustum &frustum);
	float distance(CFrustum &frustum);
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

class QuadTreeNew {

public:
	QuadTreeNew();
	~QuadTreeNew();

	void init(Vector3f* vertices, unsigned int* indices, unsigned int indicesCount, const Vector3f& min, const Vector3f& max, float minAABBSize = 16.0f);
	void quickSortVisibleGeometryNodes(int left, int right);
	int checkVisibility(bool sortVisibleGeometryNodes);
	void draw(bool visualizeRenderingOrder) const;
	void drawAABB(int depth);
	void destroy();
	void setFrustum(const Matrix4f& mtx);

	static CFrustum Frustum;

private:

	void setDefaults();

	TreeNode *root;
	TreeNode **visibleGeometryNodes;
	int visibleGeometryNodesCount;
};