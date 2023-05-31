#pragma once

#include <vector>

#include "engine/Vector.h"

#include "Terrain.h"

const int MAX_TRIANGLES = 10000;

class QuadTree{

private:

	struct Vertex{
		Vector3f position;
		Vector2f texture;
		Vector3f normal;
	};

	struct Node{
		float positionX, positionZ, width;
		int triangleCount;
		int drawCount;
		int depth;
		Node* nodes[4];
		unsigned int vao = 0;
		unsigned int vbo = 0;
		unsigned int vaoDebug = 0;
		unsigned int vboDebug = 0;
	};

public:
	QuadTree();
	QuadTree(const  QuadTree&);
	~QuadTree();

	void Initialize(Terrain* terrain);
	void Shutdown();
	void Render(int depth = 0);

	int GetDrawCount();

	static int MaxDepth;

private:

	void CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth);
	void CreateTreeNode(Node* node, float positionX, float positionZ, float width, int depth);
	int CountTriangles(float positionX, float positionZ, float width);
	bool IsTriangleContained(int index, float positionX, float positionZ, float width);
	void ReleaseNode(Node* node);
	void RenderNode(Node* node, int depth);

private:

	int m_triangleCount, m_drawCount;
	Node* m_parentNode;
	bool m_result = false;

	Terrain* m_terrain;
};
