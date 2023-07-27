// For conditions of distribution and use, seecopyright notice in License.txt

#include "../Graphics/FrameBufferTu.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/IndexBuffer.h"
#include "../Graphics/RenderBuffer.h"
#include "../Graphics/ShaderTu.h"
#include "../Graphics/ShaderProgram.h"
#include "../Graphics/TextureTu.h"
#include "../Graphics/UniformBuffer.h"
#include "../Graphics/VertexBuffer.h"
#include "../IO/Log.h"
#include "../Math/RandomTu.h"
#include "../Resource/ResourceCache.h"
#include "../Scene/Scene.h"
#include "AnimatedModel.h"
#include "Animation.h"
#include "Batch.h"
#include "CameraTu.h"
#include "DebugRenderer.h"
#include "Light.h"
#include "LightEnvironment.h"
#include "MaterialTu.h"
#include "Model.h"
#include "Octree.h"
#include "Renderer.h"
#include "StaticModel.h"

#include <algorithm>
#include <cstring>
#include <tracy/Tracy.hpp>
#include <iostream>

static const size_t DRAWABLES_PER_BATCH_TASK = 128;
static const size_t NUM_BOX_INDICES = 36;
static const float OCCLUSION_MARGIN = 0.1f;

static inline bool CompareDrawableDistances(Drawable* lhs, Drawable* rhs)
{
    return lhs->Distance() < rhs->Distance();
}


void ThreadOctantResult::Clear()
{
    drawableAcc = 0;
    taskOctantIdx = 0;
    batchTaskIdx = 0;
    lights.clear();
    octants.clear();
    occlusionQueries.clear();
}

void ThreadBatchResult::Clear()
{
    minZ = M_MAX_FLOAT;
    maxZ = 0.0f;
    geometryBounds.Undefine();
    opaqueBatches.clear();
    alphaBatches.clear();
}

ShadowMap::ShadowMap()
{
    // Construct texture but do not define its size yet
    texture = ObjectTu::Create<TextureTu>();
    fbo = new FrameBufferTu();
}

ShadowMap::~ShadowMap()
{
}

void ShadowMap::Clear()
{
    freeQueueIdx = 0;
    freeCasterListIdx = 0;
    allocator.Reset(texture->Width(), texture->Height(), 0, 0, false);
    shadowViews.clear();
    instanceTransforms.clear();

    for (auto it = shadowBatches.begin(); it != shadowBatches.end(); ++it)
        it->Clear();
    for (auto it = shadowCasters.begin(); it != shadowCasters.end(); ++it)
        it->clear();
}

Renderer::Renderer() : workQueue(Subsystem<WorkQueue>()) {
    
    assert(workQueue);

    RegisterSubsystem(this);
    RegisterRendererLibrary();

	perViewDataBuffer = new UniformBuffer();
    perViewDataBuffer->Define(USAGE_DYNAMIC, sizeof(PerViewUniforms));

    octantResults = new ThreadOctantResult[NUM_OCTANT_TASKS];
 

    for (size_t i = 0; i < NUM_OCTANTS + 1; ++i){
        collectOctantsTasks[i] = new CollectOctantsTask(this, &Renderer::CollectOctantsWork);
        collectOctantsTasks[i]->resultIdx = i;
    }

 
}

Renderer::~Renderer()
{
    RemoveSubsystem(this);
}

void Renderer::CollectOctantsAndLights(Octant* octant, ThreadOctantResult& result, unsigned char planeMask) {
    const BoundingBox& octantBox = octant->CullingBox();

    if (planeMask){
        // If not already inside all frustum planes, do frustum test and terminate if completely outside
        //planeMask = frustum.IsInsideMasked(octantBox, planeMask);
        if (planeMask == 0xff){
            // If octant becomes frustum culled, reset its visibility for when it comes back to view, including its children
            if (false && octant->Visibility() != VIS_OUTSIDE_FRUSTUM)
                octant->SetVisibility(VIS_OUTSIDE_FRUSTUM, true);
            return;
        }
    }

    // When occlusion not in use, reset all traversed octants to visible-unknown
	octant->SetVisibility(VIS_VISIBLE_UNKNOWN, false);
    

    const std::vector<Drawable*>& drawables = octant->Drawables();

    for (auto it = drawables.begin(); it != drawables.end(); ++it) {
        Drawable* drawable = *it;

        if (!drawable->TestFlag(DF_LIGHT)) {
			result.octants.push_back(std::make_pair(octant, planeMask));
			result.drawableAcc += drawables.end() - it;
        }
        
    }

    // Root octant is handled separately. Otherwise recurse into child octants
    if (octant != octree->Root() && octant->HasChildren()){
        for (size_t i = 0; i < NUM_OCTANTS; ++i){
            if (octant->Child(i))
                CollectOctantsAndLights(octant->Child(i), result, planeMask);
        }
    }
}


void Renderer::CollectOctantsWork(Task* task_, unsigned int idx)
{
    ZoneScoped;

    CollectOctantsTask* task = static_cast<CollectOctantsTask*>(task_);

    Octant* octant = task->startOctant;
    ThreadOctantResult& result = octantResults[task->resultIdx];

    CollectOctantsAndLights(octant, result);
    numPendingBatchTasks.fetch_add(-1);
}

void RegisterRendererLibrary()
{
	static bool registered = false;
    if (registered)
        return;

    // Scene node base attributes are needed
	RegisterSceneLibrary();
    Octree::RegisterObject();
	Bone::RegisterObject();
    CameraTu::RegisterObject();
    OctreeNode::RegisterObject();
    GeometryNode::RegisterObject();
    StaticModel::RegisterObject();
    AnimatedModel::RegisterObject();
    Light::RegisterObject();
    LightEnvironment::RegisterObject();
    MaterialTu::RegisterObject();
    Model::RegisterObject();
    Animation::RegisterObject();

    registered = true;
}
