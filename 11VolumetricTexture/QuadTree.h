#pragma once

#include <vector>

#include "engine/Vector.h"
#include "engine/Extension.h"
#include "Terrain2.h"

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
		Node* nodes[4];
		unsigned int vao = 0;
		unsigned int vbo = 0;
	};

public:
	QuadTree();
	QuadTree(const  QuadTree&);
	~QuadTree();

	void Initialize(TerrainClass* terrain);
	void Shutdown();
	void Render();

	int GetDrawCount();

private:

	void CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth);
	void CreateTreeNode(Node* node, float positionX, float positionZ, float width);
	int CountTriangles(float positionX, float positionZ, float width);
	bool IsTriangleContained(int index, float positionX, float positionZ, float width);
	void ReleaseNode(Node* node);
	void RenderNode(Node* node);

private:
	int m_triangleCount, m_drawCount;
	Vertex* m_vertexList;
	Node* m_parentNode;
};
