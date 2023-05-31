#include <iostream>
#include <GL/glew.h>
#include "QuadTree.h"
#include "StateMachine.h"

int QuadTree::MaxDepth = 0;

QuadTree::QuadTree() {
	m_parentNode = 0;
}

QuadTree::QuadTree(const QuadTree& other) { }

QuadTree::~QuadTree() { }

void QuadTree::Initialize(Terrain* terrain) {
	m_terrain = terrain;
	
	int vertexCount;
	float centerX, centerZ, width;

	// Get the number of vertices in the terrain vertex array.
	vertexCount = terrain->GetVertexCount();
	// Store the total triangle count for the vertex list.
	m_triangleCount = vertexCount / 3;

	// Calculate the center x,z and the width of the mesh.
	CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

	// Create the parent node for the quad tree.
	m_parentNode = new Node();

	if (!m_parentNode){
		return;
	}
	
	// Recursively build the quad tree based on the vertex list data and mesh dimensions.
	CreateTreeNode(m_parentNode, centerX, centerZ, width, 0);
}


void QuadTree::Shutdown() {
	// Recursively release the quad tree data.
	if (m_parentNode) {
		ReleaseNode(m_parentNode);
		delete m_parentNode;
		m_parentNode = 0;
	}

	return;
}


void QuadTree::Render(int depth) {
	// Reset the number of triangles that are drawn for this frame.
	m_drawCount = 0;

	// Render each node that is visible starting at the parent node and moving down the tree.
	RenderNode(m_parentNode, depth);

	return;
}


int QuadTree::GetDrawCount() {
	return m_drawCount;
}


void QuadTree::CalculateMeshDimensions(int _vertexCount, float& centerX, float& centerZ, float& meshWidth) {
	int i;
	float maxWidth, maxDepth, minWidth, minDepth, width, depth, maxX, maxZ;
	int vertexCount = m_terrain->m_positions.size();

	// Initialize the center position of the mesh to zero.
	centerX = 0.0f;
	centerZ = 0.0f;
	std::vector<unsigned int>& indexBuffer = m_terrain->m_indexBuffer;
	// Sum all the vertices in the mesh.
	for (i = 0; i<vertexCount; i++) {
		centerX += m_terrain->m_positions[i][0];
		centerZ += m_terrain->m_positions[i][2];
	}

	// And then divide it by the number of vertices to find the mid-point of the mesh.
	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;

	// Initialize the maximum and minimum size of the mesh.
	maxWidth = 0.0f;
	maxDepth = 0.0f;

	minWidth = fabsf(m_terrain->m_positions[i][0] - centerX);
	minDepth = fabsf(m_terrain->m_positions[i][2] - centerZ);

	// Go through all the vertices and find the maximum and minimum width and depth of the mesh.
	for (i = 0; i<vertexCount; i++) {
		width = fabsf(m_terrain->m_positions[i][0] - centerX);
		depth = fabsf(m_terrain->m_positions[i][2] - centerZ);

		if (width > maxWidth) { maxWidth = width; }
		if (depth > maxDepth) { maxDepth = depth; }
		if (width < minWidth) { minWidth = width; }
		if (depth < minDepth) { minDepth = depth; }
	}

	// Find the absolute maximum value between the min and max depth and width.
	maxX = (float)std::max(fabs(minWidth), fabs(maxWidth));
	maxZ = (float)std::max(fabs(minDepth), fabs(maxDepth));

	// Calculate the maximum diameter of the mesh.
	meshWidth = std::max(maxX, maxZ) * 2.0f;

	return;
}


void QuadTree::CreateTreeNode(Node* node, float positionX, float positionZ, float width, int depth) {
	int numTriangles, i, count, vertexCount, indexCount, vertexIndex;
	float offsetX, offsetZ;
	Vertex* vertices;
	unsigned int* indices;
	bool result;
	float widthHalf = width * 0.5f;
	MaxDepth = std::max(MaxDepth, depth);

	float data[] = {
		positionX - widthHalf,  0.0f, positionZ + widthHalf,
		positionX + widthHalf,  0.0f, positionZ + widthHalf,
		positionX + widthHalf, 50.0f, positionZ + widthHalf,
		positionX - widthHalf, 50.0f, positionZ + widthHalf,

		positionX - widthHalf,  0.0f, positionZ - widthHalf,
		positionX + widthHalf,  0.0f, positionZ - widthHalf,
		positionX + widthHalf, 50.0f, positionZ - widthHalf,
		positionX - widthHalf, 50.0f, positionZ - widthHalf
	};

	const unsigned short indicesDebug[] = {
		// positive X
		1, 5, 6,
		6, 2, 1,
		// negative X
		4, 0, 3,
		3, 7, 4,
		// positive Y
		3, 2, 6,
		6, 7, 3,
		// negative Y
		4, 5, 1,
		1, 0, 4,
		// positive Z
		0, 1, 2,
		2, 3, 0,
		// negative Z
		7, 6, 5,
		5, 4, 7
	};

	unsigned int iboDebug;
	glGenBuffers(1, &iboDebug);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboDebug);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned short), indicesDebug, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &node->vboDebug);
	glBindBuffer(GL_ARRAY_BUFFER, node->vboDebug);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenVertexArrays(1, &node->vaoDebug);
	glBindVertexArray(node->vaoDebug);

	glBindBuffer(GL_ARRAY_BUFFER, node->vboDebug);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboDebug);
	

	glBindVertexArray(0);
	glDeleteBuffers(1, &iboDebug);


	// Store the node position and size.
	node->positionX = positionX;
	node->positionZ = positionZ;
	node->width = width;
	node->depth = depth;
	// Initialize the triangle count to zero for the node.
	node->triangleCount = 0;

	// Initialize the children nodes of this node to null.
	node->nodes[0] = 0;
	node->nodes[1] = 0;
	node->nodes[2] = 0;
	node->nodes[3] = 0;

	// Count the number of triangles that are inside this node.
	numTriangles = CountTriangles(positionX, positionZ, width);

	// Case 1: If there are no triangles in this node then return as it is empty and requires no processing.
	if (numTriangles == 0) {
		return;
	}

	// Case 2: If there are too many triangles in this node then split it into four equal sized smaller tree nodes.
	if (numTriangles > MAX_TRIANGLES) {
		for (i = 0; i<4; i++) {
			// Calculate the position offsets for the new child node.
			offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
			offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

			// See if there are any triangles in the new node.
			count = CountTriangles((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			if (count > 0) {
				// If there are triangles inside where this new node would be then create the child node.
				node->nodes[i] = new Node();

				// Extend the tree starting from this new child node now.
				CreateTreeNode(node->nodes[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f), depth + 1);
			}
		}

		return;
	}

	// Case 3: If this node is not empty and the triangle count for it is less than the max then 
	// this node is at the bottom of the tree so create the list of triangles to store in it.
	node->triangleCount = numTriangles;

	// Calculate the number of vertices.
	vertexCount = numTriangles * 3;

	// Create the vertex array.
	vertices = new Vertex[vertexCount];

	// Create the index array.
	indices = new unsigned int[vertexCount];

	// Initialize the index for this new vertex and index array.
	indexCount = 0;

	std::vector<unsigned int>& indexBuffer = m_terrain->m_indexBuffer;

	// Go through all the triangles in the vertex list.
	for (i = 0; i < m_triangleCount; i++) {
		// If the triangle is inside this node then add it to the vertex array.
		result = IsTriangleContained(i, positionX, positionZ, width);
		if (result == true) {
			// Calculate the index into the terrain vertex list.
			vertexIndex = i * 3;

			// Get the three vertices of this triangle from the vertex list.
			vertices[indexCount].position = m_terrain->m_positions[indexBuffer[vertexIndex]];
			vertices[indexCount].texture  = m_terrain->m_texels[indexBuffer[vertexIndex]];
			vertices[indexCount].normal   = m_terrain->m_normals[indexBuffer[vertexIndex]];
			indices[indexCount] = indexCount;
			indexCount++;

			vertexIndex++;
			vertices[indexCount].position = m_terrain->m_positions[indexBuffer[vertexIndex]];
			vertices[indexCount].texture  = m_terrain->m_texels[indexBuffer[vertexIndex]];
			vertices[indexCount].normal   = m_terrain->m_normals[indexBuffer[vertexIndex]];
			indices[indexCount] = indexCount;
			indexCount++;

			vertexIndex++;
			vertices[indexCount].position = m_terrain->m_positions[indexBuffer[vertexIndex]];
			vertices[indexCount].texture  = m_terrain->m_texels[indexBuffer[vertexIndex]];
			vertices[indexCount].normal   = m_terrain->m_normals[indexBuffer[vertexIndex]];
			indices[indexCount] = indexCount;
			indexCount++;
		}
	}

	short stride = 8;
	
	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &node->vbo);

	glGenVertexArrays(1, &node->vao);
	glBindVertexArray(node->vao);

	glBindBuffer(GL_ARRAY_BUFFER, node->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//Texture Coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));

	//Normal Coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(5 * sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);


	// Release the vertex and index arrays now that the data is stored in the buffers in the node.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	node->drawCount = indexCount;
	return;
}


int QuadTree::CountTriangles(float positionX, float positionZ, float width) {
	int count, i;
	bool result;


	// Initialize the count to zero.
	count = 0;

	// Go through all the triangles in the entire mesh and check which ones should be inside this node.
	for (i = 0; i<m_triangleCount; i++) {
		// If the triangle is inside the node then increment the count by one.
		result = IsTriangleContained(i, positionX, positionZ, width);
		if (result == true) {
			count++;
		}
	}

	return count;
}


bool QuadTree::IsTriangleContained(int index, float positionX, float positionZ, float width) {
	std::vector<unsigned int>& indexBuffer = m_terrain->m_indexBuffer;
	float radius;
	int vertexIndex;
	float x1, z1, x2, z2, x3, z3;
	float minimumX, maximumX, minimumZ, maximumZ;


	// Calculate the radius of this node.
	radius = width / 2.0f;

	// Get the index into the vertex list.
	vertexIndex = index * 3;

	// Get the three vertices of this triangle from the vertex list.
	x1 = m_terrain->m_positions[indexBuffer[vertexIndex]][0];
	z1 = m_terrain->m_positions[indexBuffer[vertexIndex]][2];
	vertexIndex++;

	x2 = m_terrain->m_positions[indexBuffer[vertexIndex]][0];
	z2 = m_terrain->m_positions[indexBuffer[vertexIndex]][2];
	vertexIndex++;

	x3 = m_terrain->m_positions[indexBuffer[vertexIndex]][0];
	z3 = m_terrain->m_positions[indexBuffer[vertexIndex]][2];

	// Check to see if the minimum of the x coordinates of the triangle is inside the node.
	minimumX = std::min(x1, std::min(x2, x3));
	if (minimumX > (positionX + radius)) {
		return false;
	}

	// Check to see if the maximum of the x coordinates of the triangle is inside the node.
	maximumX = std::max(x1, std::max(x2, x3));
	if (maximumX < (positionX - radius)) {
		return false;
	}

	// Check to see if the minimum of the z coordinates of the triangle is inside the node.
	minimumZ = std::min(z1, std::min(z2, z3));
	if (minimumZ >(positionZ + radius)) {
		return false;
	}

	// Check to see if the maximum of the z coordinates of the triangle is inside the node.
	maximumZ = std::max(z1, std::max(z2, z3));
	if (maximumZ < (positionZ - radius)) {
		return false;
	}

	return true;
}


void QuadTree::ReleaseNode(Node* node) {
	int i;


	// Recursively go down the tree and release the bottom nodes first.
	for (i = 0; i<4; i++) {
		if (node->nodes[i] != 0) {
			ReleaseNode(node->nodes[i]);
		}
	}

	// Release the four child nodes.
	for (i = 0; i<4; i++) {
		if (node->nodes[i]) {
			delete node->nodes[i];
			node->nodes[i] = 0;
		}
	}

	return;
}


void QuadTree::RenderNode(Node* node, int depth) {
	
	if (depth == node->depth) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBindVertexArray(node->vaoDebug);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);

		glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
	}
	
	bool result = true;

	// Check to see if the node can be viewed, height doesn't matter in a quad tree.
	//result = frustum->CheckCube(node->positionX, 0.0f, node->positionZ, (node->width / 2.0f));

	// If it can't be seen then none of its children can either so don't continue down the tree, this is where the speed is gained.
	if (!result) {
		return;
	}
	
	// If it can be seen then check all four child nodes to see if they can also be seen.
	int count = 0;
	for (int i = 0; i < 4; i++) {
		if (node->nodes[i] != 0) {
			count++;
			RenderNode(node->nodes[i], depth);
		}
	}

	// If there were any children nodes then there is no need to continue as parent nodes won't contain any triangles to render.
	if (count != 0) {
		return;
	}
	
	/*m_result = !m_result;
	if (!m_result) {
		return;
	}*/
	// Otherwise if this node can be seen and has triangles in it then render these triangles.
	glBindVertexArray(node->vao);
	glDrawElements(GL_TRIANGLES, node->drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Increase the count of the number of polygons that have been rendered during this frame.
	m_drawCount += node->triangleCount;
}