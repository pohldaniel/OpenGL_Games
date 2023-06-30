#pragma once
#include <map>
#include "engine/Vector.h"
#include "Physics.h"

class TriangleCollector : public btTriangleCallback {

public:

	struct Comparer {
		bool operator()(const btVector3& lhs, const btVector3& rhs) const {
			return memcmp(lhs, rhs, sizeof(btVector3)) < 0;
		}
	};


	std::vector<unsigned int>* indexBufferOut;
	std::vector <btVector3>* vertexBufferOut;

	std::map<btVector3, unsigned int, Comparer> vertexCache;

	TriangleCollector() {
		vertexBufferOut = nullptr;
		indexBufferOut = nullptr;
	}

	bool getSimilarVertexIndex(btVector3& packed, std::map<btVector3, unsigned int, Comparer>& VertexToOutIndex, unsigned int & result) {

		std::map<btVector3, unsigned int>::iterator it = VertexToOutIndex.find(packed);
		if (it == VertexToOutIndex.end()) {
			return false;
		}
		else {
			result = it->second;
			return true;
		}
	}

	virtual void processTriangle(btVector3* triVert, int partId, int triangleIndex) {

		//for (int k = 0; k < 3; k++) {			
		//	indexBufferOut->push_back(indexBufferOut->size());
		//	vertexBufferOut->push_back(btVector3(triVert[k].getX(), triVert[k].getY(), triVert[k].getZ()));			
		//}

		unsigned int index;
		bool found;
		for (int k = 0; k < 3; k++) {

			found = getSimilarVertexIndex(triVert[k], vertexCache, index);
			if (found) {
				(*indexBufferOut).push_back(index);
			}
			else {
				(*vertexBufferOut).push_back(triVert[0]);

				unsigned int newindex = (unsigned int)(*vertexBufferOut).size() - 1;
				(*indexBufferOut).push_back(newindex);
				vertexCache[triVert[k]] = newindex;
			}
		}
	}
};

struct DrawHelper {
	std::vector<btVector3>* m_positions;
	std::vector<unsigned int>* m_indices;
	btVector3 m_localScaling;
	Vector4f m_color;
};

class HeightField {

public:

	HeightField();
	~HeightField();

	void create(char* heightfieldData, unsigned int dimX, unsigned int dimZ, const btTransform& transform, const btVector3& localScaling = btVector3(1.0f, 1.0f, 1.0f), int collisionFilterGroup = 1, int collisionFilterMask = -1);
	void processAllTriangles();

protected:

	std::vector<btVector3> m_positions;
	std::vector<unsigned int> m_indices;

	btHeightfieldTerrainShape* m_heightFieldShape;
	btRigidBody* m_rigidBody;
	btVector3 m_localScaling;
	DrawHelper m_drawHelper;
};