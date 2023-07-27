// For conditions of distribution and use, see copyright notice in License.txt

#pragma once

#include "../Graphics/GraphicsDefs.h"
#include "../Math/Color.h"
#include "../Math/FrustumTu.h"
#include "../Object/AutoPtr.h"
#include "../Resource/Image.h"
#include "../Thread/WorkQueue.h"
#include "Batch.h"

#include <atomic>

class CameraTu;
class FrameBufferTu;
class GeometryDrawable;
class Graphics;
class LightDrawable;
class LightEnvironment;
class MaterialTu;
class Octant;
class Octree;
class RenderBuffer;
class Scene;
class ShaderProgram;
class TextureTu;
class UniformBuffer;
class VertexBuffer;
struct CollectOctantsTask;
struct CollectBatchesTask;
struct CollectShadowBatchesTask;
struct CollectShadowCastersTask;
struct CullLightsTask;
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
struct ThreadOctantResult
{
    /// Clear for the next frame.
    void Clear();

    /// Drawable accumulator. When full, queue the next batch collection task.
    size_t drawableAcc;
    /// Starting octant index for current task.
    size_t taskOctantIdx;
    /// Batch collection task index.
    size_t batchTaskIdx;
    /// Intermediate octant list.
    std::vector<std::pair<Octant*, unsigned char> > octants;
    /// Intermediate light drawable list.
    std::vector<LightDrawable*> lights;
    /// Tasks for main view batches collection, queued by the octant collection task when it finishes.
    std::vector<AutoPtr<CollectBatchesTask> > collectBatchesTasks;
    /// New occlusion queries to be issued.
    std::vector<Octant*> occlusionQueries;
};

/// Per-thread results for batch collection.
struct ThreadBatchResult
{
    /// Clear for the next frame.
    void Clear();

    /// Minimum geometry Z value.
    float minZ;
    /// Maximum geometry Z value.
    float maxZ;
    /// Combined bounding box of the visible geometries.
    BoundingBox geometryBounds;
    /// Initial opaque batches.
    std::vector<Batch> opaqueBatches;
    /// Initial alpha batches.
    std::vector<Batch> alphaBatches;
};

/// Shadow map data structure. May be shared by several lights.
struct ShadowMap
{
    /// Default-construct.
    ShadowMap();
    /// Destruct.
    ~ShadowMap();

    /// Clear for the next frame.
    void Clear();

    /// Next free batch queue.
    size_t freeQueueIdx;
    /// Next free shadowcaster list index.
    size_t freeCasterListIdx;
    /// Rectangle allocator.
    AreaAllocator allocator;
    /// Shadow map texture.
    SharedPtr<TextureTu> texture;
    /// Shadow map framebuffer.
    SharedPtr<FrameBufferTu> fbo;
    /// Shadow views that use this shadow map.
    std::vector<ShadowView*> shadowViews;
    /// Shadow batch queues used by the shadow views.
    std::vector<BatchQueue> shadowBatches;
    /// Intermediate shadowcaster lists for processing.
    std::vector<std::vector<Drawable*> > shadowCasters;
    /// Instancing transforms for shadowcasters.
    std::vector<Matrix3x4> instanceTransforms;
};

/// Per-view uniform buffer data.
struct PerViewUniforms
{
    /// Current camera's view matrix
    Matrix3x4 viewMatrix;
    /// Current camera's projection matrix.
    Matrix4 projectionMatrix;
    /// Current camera's combined view and projection matrix.
    Matrix4 viewProjMatrix;
    /// Current camera's depth parameters.
    Vector4 depthParameters;
    /// Current camera's world position.
    Vector4 cameraPosition;
    /// Current scene's ambient color.
    Color ambientColor;
    /// Current scene's fog color.
    Color fogColor;
    /// Current scene's fog start and end parameters.
    Vector4 fogParameters;
    /// Directional light direction.
    Vector4 dirLightDirection;
    /// Directional light color.
    Color dirLightColor;
    /// Directional light shadow split parameters.
    Vector4 dirLightShadowSplits;
    /// Directional light shadow parameters.
    Vector4 dirLightShadowParameters;
    /// Directional light shadow matrices.
    Matrix4 dirLightShadowMatrices[2];
};

/// Per-light data for cluster light shader.
struct LightData
{
    /// %Light position.
    Vector4 position;
    /// %Light direction.
    Vector4 direction;
    /// %Light attenuation parameters.
    Vector4 attenuation;
    /// %Light color.
    Color color;
    /// Shadow parameters.
    Vector4 shadowParameters;
    /// Shadow matrix. For point lights, contains extra parameters.
    Matrix4 shadowMatrix;
};

/// Per-cluster data for culling lights.
struct ClusterCullData
{
    /// Cluster frustum.
    FrustumTu frustum;
    /// Cluster bounding box.
    BoundingBox boundingBox;
    /// Number of lights already in cluster.
    unsigned char numLights;
};

/// High-level rendering subsystem. Performs rendering of 3D scenes.
class Renderer : public ObjectTu
{
    OBJECT(Renderer);

public:
    /// Construct. Register subsystem and objects. Graphics and WorkQueue subsystems must have been initialized.
    Renderer();
    /// Destruct.
    ~Renderer();

    void CollectOctantsAndLights(Octant* octant, ThreadOctantResult& result, unsigned char planeMask = 0x3f);
    void CollectOctantsWork(Task* task, unsigned threadIndex);
  
    Octree* octree;

    WorkQueue* workQueue;
    
    std::vector<Octant*> rootLevelOctants;
    AutoArrayPtr<ThreadOctantResult> octantResults;
    BatchQueue opaqueBatches;
	std::atomic<int> numPendingBatchTasks;

    AutoPtr<CollectOctantsTask> collectOctantsTasks[NUM_OCTANT_TASKS];
    AutoPtr<UniformBuffer> perViewDataBuffer;
	PerViewUniforms perViewData;
   
};

/// Register Renderer related object factories and attributes.
void RegisterRendererLibrary();

/// %Task for collecting octants.
struct CollectOctantsTask : public MemberFunctionTask<Renderer> {

	CollectOctantsTask(Renderer* object_, MemberWorkFunctionPtr function_) : MemberFunctionTask<Renderer>(object_, function_){
	}

	Octant* startOctant;

	size_t resultIdx;
};

struct CollectBatchesTask : public MemberFunctionTask<Renderer>{
	/// Construct.
	CollectBatchesTask(Renderer* object_, MemberWorkFunctionPtr function_) : MemberFunctionTask<Renderer>(object_, function_) {
	}

	/// %Octant list with plane masks.
	std::vector<std::pair<Octant*, unsigned char > > octants;
};