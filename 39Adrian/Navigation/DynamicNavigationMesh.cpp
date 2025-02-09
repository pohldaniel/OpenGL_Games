#include <lz4.h>
#include <DetourNavMesh.h>
#include <DetourTileCache.h>
#include <DetourNavMeshBuilder.h>
#include <DetourTileCacheBuilder.h>
#include <Recast.h>

#include "DynamicNavigationMesh.h"
#include "Obstacle.h"
#include "OffMeshConnection.h"

static const int DEFAULT_MAX_OBSTACLES = 1024;
static const int DEFAULT_MAX_LAYERS = 16;

struct DynamicNavigationMesh::TileCacheData{
	unsigned char* data;
	int dataSize;
};

struct TileCompressor : public dtTileCacheCompressor{
	virtual int maxCompressedSize(const int bufferSize) {
		return (int)(bufferSize * 1.05f);
	}

	virtual dtStatus compress(const unsigned char* buffer, const int bufferSize,
		unsigned char* compressed, const int /*maxCompressedSize*/, int* compressedSize) {
		*compressedSize = LZ4_compress_default((const char*)buffer, (char*)compressed, bufferSize, LZ4_compressBound(bufferSize));
		return DT_SUCCESS;
	}

	virtual dtStatus decompress(const unsigned char* compressed, const int compressedSize,
		unsigned char* buffer, const int maxBufferSize, int* bufferSize) {
		*bufferSize = LZ4_decompress_safe((const char*)compressed, (char*)buffer, compressedSize, maxBufferSize);
		return *bufferSize < 0 ? DT_FAILURE : DT_SUCCESS;
	}
};

struct MeshProcess : public dtTileCacheMeshProcess {
	DynamicNavigationMesh* owner_;
	std::vector<Vector3f> offMeshVertices_;
	std::vector<float> offMeshRadii_;
	std::vector<unsigned short> offMeshFlags_;
	std::vector<unsigned char> offMeshAreas_;
	std::vector<unsigned char> offMeshDir_;

	inline MeshProcess(DynamicNavigationMesh* owner) :
		owner_(owner) {
	}

	virtual void process(struct dtNavMeshCreateParams* params, unsigned char* polyAreas, unsigned short* polyFlags) {
		// Update poly flags from areas.
		// \todo Assignment of flags from areas?
		for (int i = 0; i < params->polyCount; ++i)
		{
			if (polyAreas[i] != RC_NULL_AREA)
				polyFlags[i] = RC_WALKABLE_AREA;
		}

		BoundingBox bounds;
		rcVcopy(&bounds.min[0], params->bmin);
		rcVcopy(&bounds.max[0], params->bmin);

		// collect off-mesh connections
		/*std::vector<OffMeshConnection*> offMeshConnections = owner_->CollectOffMeshConnections(bounds);
		if (offMeshConnections.size() > 0) {
			if (offMeshConnections.size() != offMeshRadii_.size()) {

				//Matrix3x4 inverse = owner_->GetNode()->GetWorldTransform().Inverse();
				Matrix4f inverse = Matrix4f::IDENTITY;
				ClearConnectionData();
				for (unsigned i = 0; i < offMeshConnections.size(); ++i) {

					OffMeshConnection* connection = offMeshConnections[i];
					Vector3f start = inverse * connection->GetNode()->GetWorldPosition();
					Vector3f end = inverse * connection->GetEndPoint()->GetWorldPosition();

					offMeshVertices_.push_back(start);
					offMeshVertices_.push_back(end);
					offMeshRadii_.push_back(connection->GetRadius());
					offMeshFlags_.push_back((unsigned short)connection->GetMask());
					offMeshAreas_.push_back((unsigned char)connection->GetAreaID());
					offMeshDir_.push_back((unsigned char)(connection->IsBidirectional() ? DT_OFFMESH_CON_BIDIR : 0));
				}
			}
			params->offMeshConCount = offMeshRadii_.size();
			params->offMeshConVerts = &offMeshVertices_[0][0];
			params->offMeshConRad = &offMeshRadii_[0];
			params->offMeshConFlags = &offMeshFlags_[0];
			params->offMeshConAreas = &offMeshAreas_[0];
			params->offMeshConDir = &offMeshDir_[0];
		}*/
	}

	void ClearConnectionData() {
		offMeshVertices_.clear();
		offMeshRadii_.clear();
		offMeshFlags_.clear();
		offMeshAreas_.clear();
		offMeshDir_.clear();
	}
};


// From the Detour/Recast Sample_TempObstacles.cpp
struct LinearAllocator : public dtTileCacheAlloc {
	unsigned char* buffer;
	int capacity;
	int top;
	int high;

	LinearAllocator(const int cap) :
		buffer(0), capacity(0), top(0), high(0) {
		resize(cap);
	}

	~LinearAllocator() {
		dtFree(buffer);
	}

	void resize(const int cap) {
		if (buffer)
			dtFree(buffer);
		buffer = (unsigned char*)dtAlloc(cap, DT_ALLOC_PERM);
		capacity = cap;
	}

	virtual void reset() {
		high = std::max(high, top);
		top = 0;
	}

	virtual void* alloc(const int size) {
		if (!buffer)
			return 0;
		if (top + size > capacity)
			return 0;
		unsigned char* mem = &buffer[top];
		top += size;
		return mem;
	}

	virtual void free(void*) {
	}
};


DynamicNavigationMesh::DynamicNavigationMesh() : 
	NavigationMesh(), 
	drawObstacles_(true), 
	tileCache_(nullptr),
	maxObstacles_(DEFAULT_MAX_OBSTACLES),
	maxLayers_(DEFAULT_MAX_LAYERS) {
	tileSize_ = 64;
	partitionType_ = NAVMESH_PARTITION_MONOTONE;
	allocator_ = new LinearAllocator(32000); //32kb to start
	compressor_ = new TileCompressor();
	meshProcessor_ = new MeshProcess(this);
}

DynamicNavigationMesh::~DynamicNavigationMesh() {

}

bool DynamicNavigationMesh::Allocate(const BoundingBox& boundingBox, unsigned maxTiles)
{
	// Release existing navigation data and zero the bounding box
	ReleaseNavigationMesh();

	//if (!node_)
		//return false;

	//if (!node_->GetWorldScale().Equals(Vector3::ONE))
		//URHO3D_LOGWARNING("Navigation mesh root node has scaling. Agent parameters may not work as intended");

	//boundingBox_ = boundingBox.Transformed(node_->GetWorldTransform().Inverse());
	maxTiles = NextPowerOfTwo(maxTiles);

	// Calculate number of tiles
	int gridW = 0, gridH = 0;
	float tileEdgeLength = (float)tileSize_ * cellSize_;
	rcCalcGridSize(&boundingBox_.min[0], &boundingBox_.max[0], cellSize_, &gridW, &gridH);
	numTilesX_ = (gridW + tileSize_ - 1) / tileSize_;
	numTilesZ_ = (gridH + tileSize_ - 1) / tileSize_;

	// Calculate max number of polygons, 22 bits available to identify both tile & polygon within tile
	unsigned tileBits = LogBaseTwo(maxTiles);
	unsigned maxPolys = (unsigned)(1 << (22 - tileBits));

	dtNavMeshParams params;
	rcVcopy(params.orig, &boundingBox_.min[0]);
	params.tileWidth = tileEdgeLength;
	params.tileHeight = tileEdgeLength;
	params.maxTiles = maxTiles;
	params.maxPolys = maxPolys;

	navMesh_ = dtAllocNavMesh();
	if (!navMesh_){
		std::cout << "Could not allocate navigation mesh" << std::endl;
		return false;
	}

	if (dtStatusFailed(navMesh_->init(&params))){
		std::cout << "Could not initialize navigation mesh" << std::endl;
		ReleaseNavigationMesh();
		return false;
	}

	dtTileCacheParams tileCacheParams;
	memset(&tileCacheParams, 0, sizeof(tileCacheParams));
	rcVcopy(tileCacheParams.orig, &boundingBox_.min[0]);
	tileCacheParams.ch = cellHeight_;
	tileCacheParams.cs = cellSize_;
	tileCacheParams.width = tileSize_;
	tileCacheParams.height = tileSize_;
	tileCacheParams.maxSimplificationError = edgeMaxError_;
	tileCacheParams.maxTiles = maxTiles * maxLayers_;
	tileCacheParams.maxObstacles = maxObstacles_;
	// Settings from NavigationMesh
	tileCacheParams.walkableClimb = agentMaxClimb_;
	tileCacheParams.walkableHeight = agentHeight_;
	tileCacheParams.walkableRadius = agentRadius_;

	tileCache_ = dtAllocTileCache();
	if (!tileCache_){
		std:: cout << "Could not allocate tile cache" << std::endl;
		ReleaseNavigationMesh();
		return false;
	}

	if (dtStatusFailed(tileCache_->init(&tileCacheParams, allocator_, compressor_, meshProcessor_))){
		std::cout << "Could not initialize tile cache" << std::endl;;
		ReleaseNavigationMesh();
		return false;
	}

	std::cout << "Allocated empty navigation mesh with max " + std::to_string(maxTiles) + " tiles" << std::endl;

	// Scan for obstacles to insert into us
	std::vector<SceneNodeLC*> obstacles;
	//GetScene()->GetChildrenWithComponent<Obstacle>(obstacles, true);
	for (unsigned i = 0; i < m_obstacles.size(); ++i){
		Obstacle* obs = m_obstacles[i];
		if (obs && obs->isEnabled_)
			AddObstacle(obs);
	}

	return true;
}

void DynamicNavigationMesh::ReleaseNavigationMesh() {
	NavigationMesh::ReleaseNavigationMesh();
	ReleaseTileCache();
}

void DynamicNavigationMesh::ReleaseTileCache() {
	dtFreeTileCache(tileCache_);
	tileCache_ = 0;
}

void DynamicNavigationMesh::OnRenderDebug() {
	NavigationMesh::OnRenderDebug();

	if (drawObstacles_) {
		for (unsigned i = 0; i < m_obstacles.size(); ++i) {
			Obstacle* obstacle = m_obstacles[i];
			if (obstacle && obstacle->isEnabled_)
				obstacle->OnRenderDebug();
		}
	}
}

void DynamicNavigationMesh::AddObstacle(Obstacle* obstacle, bool silent) {

	if (tileCache_){
		float pos[3];
		const Vector3f& obsPos = obstacle->m_node->getWorldPosition();
		rcVcopy(pos, obsPos.getVec());
		dtObstacleRef refHolder;

		while (tileCache_->isObstacleQueueFull())
			tileCache_->update(1, navMesh_);

		if (dtStatusFailed(tileCache_->addObstacle(pos, obstacle->GetRadius(), obstacle->GetHeight(), &refHolder))){
			std::cout << "Failed to add obstacle" << std::endl;
			return;
		}
		obstacle->obstacleId_ = refHolder;
	}
}

void DynamicNavigationMesh::RemoveObstacle(Obstacle* obstacle, bool silent) {
	
	if (tileCache_ && obstacle->obstacleId_ > 0) {
		while (tileCache_->isObstacleQueueFull())
			tileCache_->update(1, navMesh_);

		if (dtStatusFailed(tileCache_->removeObstacle(obstacle->obstacleId_))){
			std::cout << "Failed to remove obstacle" << std::endl;
			return;
		}
		obstacle->obstacleId_ = 0;
	}
}

void DynamicNavigationMesh::ObstacleChanged(Obstacle* obstacle) {
	if (tileCache_) {
		RemoveObstacle(obstacle, true);
		AddObstacle(obstacle, true);
	}
}