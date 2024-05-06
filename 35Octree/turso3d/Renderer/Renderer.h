// For conditions of distribution and use, see copyright notice in License.txt

#pragma once

#include "../Graphics/GraphicsDefs.h"
#include "../Math/Color.h"
#include "../Math/FrustumTu.h"
#include <Octree/AutoPtr.h>
#include "../Resource/Image.h"
#include "../Thread/WorkQueueTu.h"
#include "Batch.h"
#include <engine/Frustum.h>
#include <engine/Camera.h>
#include <atomic>

class CameraTu;
class FrameBufferTu;
class GeometryDrawable;
class Graphics;
class LightDrawable;
class LightEnvironment;
class MaterialTu;
class OctantTu;
class OctreeTu;
class RenderBuffer;
class Scene;
class ShaderProgram;
class TextureTu;
class UniformBuffer;
class VertexBuffer;
struct CollectOctantsTaskTu;
struct CollectBatchesTaskTu;
struct CollectShadowBatchesTaskTu;
struct CollectShadowCastersTaskTu;
struct CullLightsTaskTu;
struct ShadowView;
struct ThreadOctantResult;

static const size_t NUM_CLUSTER_X = 16;
static const size_t NUM_CLUSTER_Y = 8;
static const size_t NUM_CLUSTER_Z = 8;
static const size_t MAX_LIGHTS = 255;
static const size_t MAX_LIGHTS_CLUSTER = 16;
static const size_t NUM_OCTANT_TASKS = 9;
static const size_t NUM_SHADOW_MAPS = 2; // One for directional lights and another for the rest

// Texture units with built-in meanings.
static const size_t TU_DIRLIGHTSHADOW = 8;
static const size_t TU_SHADOWATLAS = 9;
static const size_t TU_FACESELECTION1 = 10;
static const size_t TU_FACESELECTION2 = 11;
static const size_t TU_LIGHTCLUSTERDATA = 12;

/// Per-thread results for octant collection.
struct ThreadOctantResult {
	void Clear();
	size_t drawableAcc;
	size_t taskOctantIdx;
	size_t batchTaskIdx;
	std::vector<std::pair<OctantTu*, unsigned char>> octants;
	std::vector<OctantTu*> occlusionQueries;
};

/// High-level rendering subsystem. Performs rendering of 3D scenes.
class Renderer : public ObjectTu
{
    OBJECT(Renderer);

public:
    /// Construct. Register subsystem and objects. Graphics and WorkQueue subsystems must have been initialized.
    Renderer(const Frustum& frustum, const Camera& camera, OctreeTu* octree);
    /// Destruct.
    ~Renderer();  
    /// Prepare view for rendering. This will utilize worker threads.
    void PrepareView(bool useOcclusion, float dt);   
    /// Collect octants and lights from the octree recursively. Queue batch collection tasks while ongoing.
    void CollectOctantsAndLights(OctantTu* octant, ThreadOctantResult& result, unsigned char planeMask = 0x3f);
    /// Add an occlusion query for the octant if applicable.
    void AddOcclusionQuery(OctantTu* octant, ThreadOctantResult& result, unsigned char planeMask);
    /// Check occlusion query results and propagate visibility hierarchically.
    void CheckOcclusionQueries();
    /// Render occlusion queries for octants.
    void RenderOcclusionQueries();
    /// Work function to collect octants.
    void CollectOctantsWork(TaskTu* task, unsigned threadIndex);
   

    /// Cached graphics subsystem.
    Graphics* graphics;
    /// Cached work queue subsystem.
    WorkQueueTu* workQueue;
    /// Occlusion use flag.
    bool useOcclusion;
    /// Previous frame camera position for occlusion culling bounding box elongation.
    Vector3f previousCameraPosition;
    /// Root-level octants, used as a starting point for octant and batch collection. The root octant is included if it also contains drawables.
    std::vector<OctantTu*> rootLevelOctants;
    /// Counter for batch collection tasks remaining. When zero, main batch sorting can begin while other tasks go on.
    std::atomic<int> numPendingBatchTasks;
    /// Per-octree branch octant collection results.
    AutoArrayPtr<ThreadOctantResult> octantResults;
    /// Tasks for octant collection.
    AutoPtr<CollectOctantsTaskTu> collectOctantsTasks[NUM_OCTANT_TASKS];

	float m_dt;
	const Frustum& m_frustum;
	const Camera& m_camera;
	/// Current scene octree.
	OctreeTu* m_octree;
};

/// Register Renderer related object factories and attributes.
void RegisterRendererLibrary();
