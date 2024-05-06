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
#include "AnimatedModelTu.h"
#include "AnimationTu.h"
#include "Batch.h"
#include "CameraTu.h"
#include "DebugRendererTu.h"
#include "Light.h"
#include "LightEnvironment.h"
#include "MaterialTu.h"
#include "Model.h"
#include "OctreeTu.h"
#include "Renderer.h"
#include "StaticModel.h"

#include <algorithm>
#include <cstring>
#include <tracy/Tracy.hpp>
#include <iostream>
#include <engine/Shader.h>
#include "Globals.h"

static const size_t DRAWABLES_PER_BATCH_TASK = 128;
static const size_t NUM_BOX_INDICES = 36;
static const float OCCLUSION_MARGIN = 0.1f;

static inline bool CompareDrawableDistances(Drawable* lhs, Drawable* rhs)
{
    return lhs->Distance() < rhs->Distance();
}

/// %Task for collecting octants.
struct CollectOctantsTaskTu : public MemberFunctionTaskTu<Renderer>
{
    /// Construct.
    CollectOctantsTaskTu(Renderer* object_, MemberWorkFunctionPtr function_) :
        MemberFunctionTaskTu<Renderer>(object_, function_)
    {
    }

    /// Starting point octant.
    OctantTu* startOctant;
    /// Result structure index.
    size_t resultIdx;
};

/// %Task for collecting geometry batches from octants.
struct CollectBatchesTaskTu : public MemberFunctionTaskTu<Renderer>
{
    /// Construct.
    CollectBatchesTaskTu(Renderer* object_, MemberWorkFunctionPtr function_) :
        MemberFunctionTaskTu<Renderer>(object_, function_)
    {
    }

    /// %Octant list with plane masks.
    std::vector<std::pair<OctantTu*, unsigned char > > octants;
};

/// %Task for collecting shadowcasters of a specific light.
struct CollectShadowCastersTaskTu : public MemberFunctionTaskTu<Renderer>
{
    /// Construct.
    CollectShadowCastersTaskTu(Renderer* object_, MemberWorkFunctionPtr function_) :
        MemberFunctionTaskTu<Renderer>(object_, function_)
    {
    }

    /// %Light.
    LightDrawable* light;
};

/// %Task for collecting shadow batches of a specific shadow view.
struct CollectShadowBatchesTaskTu : public MemberFunctionTaskTu<Renderer>
{
    /// Construct.
    CollectShadowBatchesTaskTu(Renderer* object_, MemberWorkFunctionPtr function_) :
        MemberFunctionTaskTu<Renderer>(object_, function_)
    {
    }

    /// Shadow map index.
    size_t shadowMapIdx;
    /// Shadow view index within shadow map.
    size_t viewIdx;
};

/// %Task for culling lights to a specific Z-slice of the frustum grid.
struct CullLightsTaskTu : public MemberFunctionTaskTu<Renderer>
{
    /// Construct.
    CullLightsTaskTu(Renderer* object_, MemberWorkFunctionPtr function_) :
        MemberFunctionTaskTu<Renderer>(object_, function_)
    {
    }

    /// Z-slice.
    size_t z;
};

void ThreadOctantResult::Clear()
{
    drawableAcc = 0;
    taskOctantIdx = 0;
    batchTaskIdx = 0;
    octants.clear();
    occlusionQueries.clear();
}


Renderer::Renderer(const Frustum& frustum, const Camera& camera, OctreeTu* octree) : m_octree(octree), m_frustum(frustum), m_camera(camera),graphics(Subsystem<Graphics>()),workQueue(Subsystem<WorkQueueTu>()){
    RegisterSubsystem(this);
    RegisterRendererLibrary();

    octantResults = new ThreadOctantResult[NUM_OCTANT_TASKS];

    for (size_t i = 0; i < NUM_OCTANTSTU + 1; ++i){
        collectOctantsTasks[i] = new CollectOctantsTaskTu(this, &Renderer::CollectOctantsWork);
        collectOctantsTasks[i]->resultIdx = i;
    }  
}

Renderer::~Renderer()
{
    RemoveSubsystem(this);
}

void Renderer::PrepareView(bool useOcclusion_, float dt){
    
	m_dt = dt;  
    useOcclusion = useOcclusion_;

    // Clear results from last frame
    rootLevelOctants.clear();
    
    for (size_t i = 0; i < NUM_OCTANT_TASKS; ++i)
        octantResults[i].Clear();
    
    // Process moved / animated objects' octree reinsertions
    m_octree->Update();

    // Check arrived occlusion query results while octree update goes on, then finish octree update
    CheckOcclusionQueries();
	m_octree->FinishUpdate();

    // Enable threaded update during geometry / light gathering in case nodes' OnPrepareRender() causes further reinsertion queuing
	m_octree->SetThreadedUpdate(workQueue->NumThreads() > 1);

    // Find the starting points for octree traversal. Include the root if it contains drawables that didn't fit elsewhere
    OctantTu* rootOctant = m_octree->Root();
    if (rootOctant->Drawables().size())
        rootLevelOctants.push_back(rootOctant);

    for (size_t i = 0; i < NUM_OCTANTSTU; ++i){
        if (rootOctant->Child(i))
            rootLevelOctants.push_back(rootOctant->Child(i));
    }

    // Keep track of both batch + octant task progress before main batches can be sorted (batch tasks will add to the counter when queued)
    numPendingBatchTasks.store((int)rootLevelOctants.size());

    // Find octants in view and their plane masks for node frustum culling. At the same time, find lights and process them
    // When octant collection tasks complete, they queue tasks for collecting batches from those octants.
    for (size_t i = 0; i < rootLevelOctants.size(); ++i){
        collectOctantsTasks[i]->startOctant = rootLevelOctants[i];
    }

    workQueue->QueueTasks(rootLevelOctants.size(), reinterpret_cast<TaskTu**>(&collectOctantsTasks[0]));

    // Execute tasks until can sort the main batches. Perform that in the main thread to potentially run faster
    while (numPendingBatchTasks.load() > 0)
        workQueue->TryComplete();

    // Finish remaining view preparation tasks (shadowcaster batches, light culling to frustum grid)
    workQueue->Complete();

    // No more threaded reinsertion will take place
	m_octree->SetThreadedUpdate(false);
}

void Renderer::CollectOctantsAndLights(OctantTu* octant, ThreadOctantResult& result, unsigned char planeMask)
{
    const BoundingBox& octantBox = octant->CullingBox();

    if (planeMask)
    {
        // If not already inside all frustum planes, do frustum test and terminate if completely outside
        planeMask = m_frustum.isInsideMasked(octantBox, planeMask);
        if (planeMask == 0xff)
        {
            // If octant becomes frustum culled, reset its visibility for when it comes back to view, including its children
            if (useOcclusion && octant->Visibility() != VIS_OUTSIDE_FRUSTUMTU)
                octant->SetVisibility(VIS_OUTSIDE_FRUSTUMTU, true);
            return;
        }
    }

    // Process occlusion now before going further
    if (useOcclusion)
    {
        // If was previously outside frustum, reset to visible-unknown
        if (octant->Visibility() == VIS_OUTSIDE_FRUSTUMTU)
            octant->SetVisibility(VIS_VISIBLE_UNKNOWNTU, false);

        switch (octant->Visibility())
        {
            // If octant is occluded, issue query if not pending, and do not process further this frame
        case VIS_OCCLUDEDTU:
            AddOcclusionQuery(octant, result, planeMask);
            return;

            // If octant was occluded previously, but its parent came into view, issue tests along the hierarchy but do not render on this frame
        case VIS_OCCLUDED_UNKNOWNTU:
            AddOcclusionQuery(octant, result, planeMask);
            if (octant != m_octree->Root() && octant->HasChildren())
            {
                for (size_t i = 0; i < NUM_OCTANTSTU; ++i)
                {
                    if (octant->Child(i))
                        CollectOctantsAndLights(octant->Child(i), result, planeMask);
                }
            }
            return;

            // If octant has unknown visibility, issue query if not pending, but collect child octants and drawables
        case VIS_VISIBLE_UNKNOWNTU:
            AddOcclusionQuery(octant, result, planeMask);
            break;

            // If the octant's parent is already visible too, only test the octant if it is a "leaf octant" with drawables
            // Note: visible octants will also add a time-based staggering to reduce queries
        case VIS_VISIBLETU:
            OctantTu* parent = octant->Parent();
            if (octant->Drawables().size() > 0 || (parent && parent->Visibility() != VIS_VISIBLETU))
                AddOcclusionQuery(octant, result, planeMask);
            break;
        }
    }else{
        // When occlusion not in use, reset all traversed octants to visible-unknown
        octant->SetVisibility(VIS_VISIBLE_UNKNOWNTU, false);
    }

    const std::vector<ShapeNode*>& drawables = octant->Drawables();

    for (auto it = drawables.begin(); it != drawables.end(); ++it){
		ShapeNode* drawable = *it;
        result.octants.push_back(std::make_pair(octant, planeMask));
        result.drawableAcc += drawables.end() - it;
        break;
    }

    // Root octant is handled separately. Otherwise recurse into child octants
    if (octant != m_octree->Root() && octant->HasChildren()){
        for (size_t i = 0; i < NUM_OCTANTSTU; ++i){
            if (octant->Child(i))
                CollectOctantsAndLights(octant->Child(i), result, planeMask);
        }
    }
}

void Renderer::AddOcclusionQuery(OctantTu* octant, ThreadOctantResult& result, unsigned char planeMask)
{
    // No-op if previous query still ongoing. Also If the octant intersects the frustum, verify with SAT test that it actually covers some screen area
    // Otherwise the occlusion test will produce a false negative
	if (octant->CheckNewOcclusionQuery(m_dt) && (!planeMask || m_frustum.isInsideSAT(octant->CullingBox(), m_frustum.m_frustumSATData))) {
		result.occlusionQueries.push_back(octant);
	}
}

void Renderer::CheckOcclusionQueries(){
    static std::vector<OcclusionQueryResult> results;
    results.clear();
    graphics->CheckOcclusionQueryResults(results);

    for (auto it = results.begin(); it != results.end(); ++it){
        OctantTu* octant = static_cast<OctantTu*>(it->object);
        octant->OnOcclusionQueryResult(it->visible);
    }   
}

void Renderer::RenderOcclusionQueries()
{
	Matrix4f boxMatrix(Matrix4f::IDENTITY);
	float nearClip = m_camera.getNear();

	// Use camera's motion since last frame to enlarge the bounding boxes. Use multiplied movement speed to account for latency in query results
	Vector3f cameraPosition = m_camera.getPosition();
	Vector3f cameraMove = cameraPosition - previousCameraPosition;
	Vector3f enlargement = (OCCLUSION_MARGIN + 4.0f * cameraMove.length()) * Vector3f::ONE;

	glDisable(GL_BLEND);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	auto shader = Globals::shaderManager.getAssetPointer("boundingBox");
	shader->use();
	shader->loadMatrix("u_vp", m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix());

	for (size_t i = 0; i < NUM_OCTANT_TASKS; ++i) {
		for (auto it = octantResults[i].occlusionQueries.begin(); it != octantResults[i].occlusionQueries.end(); ++it) {
			OctantTu* octant = *it;

			const BoundingBox& octantBox = octant->CullingBox();
			BoundingBox box(octantBox.min - enlargement, octantBox.max + enlargement);

			// If bounding box could be clipped by near plane, assume visible without performing query
			if (box.distance(cameraPosition) < 2.0f * nearClip) {
				octant->OnOcclusionQueryResult(true);
				continue;
			}

			Vector3f size = box.getHalfSize();
			Vector3f center = box.getCenter();

			boxMatrix[0][0] = size[0];
			boxMatrix[1][1] = size[1];
			boxMatrix[2][2] = size[2];
			boxMatrix[3][0] = center[0];
			boxMatrix[3][1] = center[1];
			boxMatrix[3][2] = center[2];

			shader->loadMatrix("u_model", boxMatrix);
			unsigned queryId = graphics->BeginOcclusionQuery(octant);
			Globals::shapeManager.get("boundingBox").drawRaw();
			graphics->EndOcclusionQuery();

			// Store query to octant to make sure we don't re-test it until result arrives
			octant->OnOcclusionQuery(queryId);
		}
	}
	shader->unuse();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	previousCameraPosition = cameraPosition;
}

void Renderer::CollectOctantsWork(TaskTu* task_, unsigned)
{
    ZoneScoped;

    CollectOctantsTaskTu* task = static_cast<CollectOctantsTaskTu*>(task_);

    // Go through octants in this task's octree branch
    OctantTu* octant = task->startOctant;
    ThreadOctantResult& result = octantResults[task->resultIdx];

    CollectOctantsAndLights(octant, result);

    // Queue final batch task for leftover nodes if needed
    /*if (result.drawableAcc)
    {
        if (result.collectBatchesTasks.size() <= result.batchTaskIdx)
            result.collectBatchesTasks.push_back(new CollectBatchesTaskTu(this, &Renderer::CollectBatchesWork));
            
        CollectBatchesTaskTu* batchTask = result.collectBatchesTasks[result.batchTaskIdx];
        batchTask->octants.clear();
        batchTask->octants.insert(batchTask->octants.end(), result.octants.begin() + result.taskOctantIdx, result.octants.end());
        numPendingBatchTasks.fetch_add(1);
        workQueue->QueueTask(batchTask);
    }*/

    numPendingBatchTasks.fetch_add(-1);
}

/*void Renderer::ProcessLightsWork(TaskTu*, unsigned)
{
    ZoneScoped;

    // Merge the light collection results
    for (size_t i = 0; i < rootLevelOctants.size(); ++i)
        lights.insert(lights.end(), octantResults[i].lights.begin(), octantResults[i].lights.end());

    // Find the directional light if any
    for (auto it = lights.begin(); it != lights.end(); )
    {
        LightDrawable* light = *it;
        if (light->GetLightType() == LIGHT_DIRECTIONAL)
        {
            if (!dirLight || light->GetColor().Average() > dirLight->GetColor().Average())
                dirLight = light;
            it = lights.erase(it);
        }
        else
            ++it;
    }

    // Sort localized lights by increasing distance
    std::sort(lights.begin(), lights.end(), CompareDrawableDistances);

    // Clamp to maximum supported
    if (lights.size() > MAX_LIGHTS)
        lights.resize(MAX_LIGHTS);

    // Pre-step for shadow map caching: reallocate all lights' shadow map rectangles which are non-zero at this point.
    // If shadow maps were dirtied (size or bias change) reset all allocations instead
    for (auto it = lights.begin(); it != lights.end(); ++it)
    {
        LightDrawable* light = *it;
        if (shadowMapsDirty)
            light->SetShadowMap(nullptr);
        else if (drawShadows && light->ShadowStrength() < 1.0f && light->ShadowRect() != IntRect::ZERO)
            AllocateShadowMap(light);
    }

    // Check if directional light needs shadows
    if (dirLight)
    {
        if (shadowMapsDirty)
            dirLight->SetShadowMap(nullptr);

        if (!drawShadows || dirLight->ShadowStrength() >= 1.0f || !AllocateShadowMap(dirLight))
            dirLight->SetShadowMap(nullptr);
    }

    shadowMapsDirty = false;

    size_t lightTaskIdx = 0;

    // Go through lights and setup shadowcaster collection tasks
    for (size_t i = 0; i < lights.size(); ++i)
    {
        LightDrawable* light = lights[i];
        float cutoff = light->GetLightType() == LIGHT_SPOT ? cosf(light->Fov() * 0.5f * M_DEGTORAD) : 0.0f;

        lightData[i].position = Vector4(light->WorldPosition(), 1.0f);
        lightData[i].direction = Vector4(-light->WorldDirection(), 0.0f);
        lightData[i].attenuation = Vector4(1.0f / Max(light->Range(), M_EPSILON), cutoff, 1.0f / (1.0f - cutoff), 1.0f);
        lightData[i].color = light->EffectiveColor();
        lightData[i].shadowParameters = Vector4::ONE; // Assume unshadowed

        // Check if not shadowcasting or beyond shadow range
        if (!drawShadows || light->ShadowStrength() >= 1.0f)
        {
            light->SetShadowMap(nullptr);
            continue;
        }

        // Now retry shadow map allocation if necessary. If it's a new allocation, must rerender the shadow map
        if (!light->ShadowMap())
        {
            if (!AllocateShadowMap(light))
                continue;
        }

        light->InitShadowViews();
        std::vector<ShadowView>& shadowViews = light->ShadowViews();

        // Preallocate shadowcaster list
        ShadowMap& shadowMap = shadowMaps[1];
        size_t casterListIdx = shadowMap.freeCasterListIdx++;
        if (shadowMap.shadowCasters.size() < shadowMap.freeCasterListIdx)
            shadowMap.shadowCasters.resize(shadowMap.freeCasterListIdx);

        for (size_t j = 0; j < shadowViews.size(); ++j)
        {
            ShadowView& view = shadowViews[j];

            // Preallocate shadow batch queues
            view.casterListIdx = casterListIdx;

            if (light->IsStatic())
            {
                view.staticQueueIdx = shadowMap.freeQueueIdx++;
                view.dynamicQueueIdx = shadowMap.freeQueueIdx++;
            }
            else
                view.dynamicQueueIdx = shadowMap.freeQueueIdx++;

            if (shadowMap.shadowBatches.size() < shadowMap.freeQueueIdx)
                shadowMap.shadowBatches.resize(shadowMap.freeQueueIdx);

            shadowMap.shadowViews.push_back(&view);
        }

        if (collectShadowCastersTasks.size() <= lightTaskIdx)
            collectShadowCastersTasks.push_back(new CollectShadowCastersTaskTu(this, &Renderer::CollectShadowCastersWork));

        collectShadowCastersTasks[lightTaskIdx]->light = light;
        workQueue->AddDependency(processShadowCastersTask, collectShadowCastersTasks[lightTaskIdx]);
        ++lightTaskIdx;
    }

    if (dirLight && dirLight->ShadowMap())
    {
        ShadowMap& shadowMap = shadowMaps[0];

        dirLight->InitShadowViews();
        std::vector<ShadowView>& shadowViews = dirLight->ShadowViews();

        for (size_t i = 0; i < shadowViews.size(); ++i)
        {
            ShadowView& view = shadowViews[i];

            // Directional light needs a new frustum query for each split, as the shadow cameras are typically far outside the main view
            // But queries are only performed later when the shadow map can be focused to visible scene
            view.casterListIdx = shadowMap.freeCasterListIdx++;
            if (shadowMap.shadowCasters.size() < shadowMap.freeCasterListIdx)
                shadowMap.shadowCasters.resize(shadowMap.freeCasterListIdx);

            view.dynamicQueueIdx = shadowMap.freeQueueIdx++;
            if (shadowMap.shadowBatches.size() < shadowMap.freeQueueIdx)
                shadowMap.shadowBatches.resize(shadowMap.freeQueueIdx);

            shadowMap.shadowViews.push_back(&view);
        }
    }

    // Now queue all shadowcaster collection tasks
    if (lightTaskIdx > 0)
        workQueue->QueueTasks(lightTaskIdx, reinterpret_cast<TaskTu**>(&collectShadowCastersTasks[0]));
}*/

/*void Renderer::CollectBatchesWork(TaskTu* task_, unsigned threadIndex)
{
    ZoneScoped;

    CollectBatchesTaskTu* task = static_cast<CollectBatchesTaskTu*>(task_);
    ThreadBatchResult& result = batchResults[threadIndex];
    bool threaded = workQueue->NumThreads() > 1;

    std::vector<std::pair<OctantTu*, unsigned char> >& octants = task->octants;
    std::vector<Batch>& opaqueQueue = threaded ? result.opaqueBatches : opaqueBatches.batches;
    std::vector<Batch>& alphaQueue = threaded ? result.alphaBatches : alphaBatches.batches;

    const Matrix3x4& viewMatrix = camera->ViewMatrix();
    Vector3 viewZ = Vector3(viewMatrix.m20, viewMatrix.m21, viewMatrix.m22);
    Vector3 absViewZ = viewZ.Abs();
    float farClipMul = 32767.0f / camera->FarClip();

    // Scan octants for geometries
    for (auto it = octants.begin(); it != octants.end(); ++it)
    {
        OctantTu* octant = it->first;
        unsigned char planeMask = it->second;
        const std::vector<Drawable*>& drawables = octant->Drawables();

        for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt)
        {
            Drawable* drawable = *dIt;

            if (drawable->TestFlag(DF_GEOMETRYTU) && (drawable->LayerMask() & viewMask))
            {
                const BoundingBoxTu& geometryBox = drawable->WorldBoundingBox();

                // Note: to strike a balance between performance and occlusion accuracy, per-geometry occlusion tests are skipped for now,
                // as octants are already tested with combined actual drawable bounds
                if ((!planeMask || frustum.IsInsideMaskedFast(geometryBox, planeMask)) && drawable->OnPrepareRender(frameNumber, camera))
                {
                    result.geometryBounds.Merge(geometryBox);

                    Vector3 center = geometryBox.Center();
                    Vector3 edge = geometryBox.Size() * 0.5f;

                    float viewCenterZ = viewZ.DotProduct(center) + viewMatrix.m23;
                    float viewEdgeZ = absViewZ.DotProduct(edge);
                    result.minZ = Min(result.minZ, viewCenterZ - viewEdgeZ);
                    result.maxZ = Max(result.maxZ, viewCenterZ + viewEdgeZ);
 
                    Batch newBatch;

                    unsigned short distance = (unsigned short)(drawable->Distance() * farClipMul);
                    const SourceBatches& batches = static_cast<GeometryDrawable*>(drawable)->Batches();
                    size_t numGeometries = batches.NumGeometries();
        
                    for (size_t j = 0; j < numGeometries; ++j)
                    {
                        MaterialTu* material = batches.GetMaterial(j);

                        // Assume opaque first
                        newBatch.pass = material->GetPass(PASS_OPAQUE);
                        newBatch.geometry = batches.GetGeometry(j);
                        newBatch.programBits = (unsigned char)(drawable->Flags() & DF_GEOMETRY_TYPE_BITSTU);
                        newBatch.geomIndex = (unsigned char)j;

                        if (!newBatch.programBits)
                            newBatch.worldTransform = &drawable->WorldTransform();
                        else
                            newBatch.drawable = static_cast<GeometryDrawable*>(drawable);

                        if (newBatch.pass)
                        {
                            // Perform distance sort in addition to state sort
                            if (newBatch.pass->lastSortKey.first != frameNumber || newBatch.pass->lastSortKey.second > distance)
                            {
                                newBatch.pass->lastSortKey.first = frameNumber;
                                newBatch.pass->lastSortKey.second = distance;
                            }
                            if (newBatch.geometry->lastSortKey.first != frameNumber || newBatch.geometry->lastSortKey.second > distance + (unsigned short)j)
                            {
                                newBatch.geometry->lastSortKey.first = frameNumber;
                                newBatch.geometry->lastSortKey.second = distance + (unsigned short)j;
                            }

                            opaqueQueue.push_back(newBatch);
                        }
                        else
                        {
                            // If not opaque, try transparent
                            newBatch.pass = material->GetPass(PASS_ALPHA);
                            if (!newBatch.pass)
                                continue;

                            newBatch.distance = drawable->Distance();
                            alphaQueue.push_back(newBatch);
                        }
                    }
                }
            }
        }
    }

    numPendingBatchTasks.fetch_add(-1);
}*/

/*void Renderer::CollectShadowCastersWork(TaskTu* task, unsigned)
{
    ZoneScoped;

    LightDrawable* light = static_cast<CollectShadowCastersTaskTu*>(task)->light;
    LightType lightType = light->GetLightType();
    std::vector<ShadowView>& shadowViews = light->ShadowViews();

    // Directional lights perform queries later, here only point & spot lights (in shadow atlas) are considered
    ShadowMap& shadowMap = shadowMaps[1];

    if (lightType == LIGHT_POINT)
    {
        // Point light: perform only one sphere query, then check which of the point light sides are visible
        for (size_t i = 0; i < shadowViews.size(); ++i)
        {
            // Check if each of the sides is in view. Do not process if isn't. Rendering will be no-op this frame, but cached contents are discarded once comes into view again
            light->SetupShadowView(i, camera);
            ShadowView& view = shadowViews[i];

            if (!frustum.IsInsideFast(BoundingBoxTu(view.shadowFrustum)))
            {
                view.renderMode = RENDER_STATIC_LIGHT_CACHED;
                view.viewport = IntRect::ZERO;
                view.lastViewport = IntRect::ZERO;
            }
        }

        std::vector<Drawable*>& shadowCasters = shadowMap.shadowCasters[shadowViews[0].casterListIdx];
        octree->FindDrawables(shadowCasters, light->WorldSphere(), DF_GEOMETRYTU | DF_CAST_SHADOWSTU);
    }
    else if (lightType == LIGHT_SPOT)
    {
        // Spot light: perform query for the spot frustum
        light->SetupShadowView(0, camera);
        ShadowView& view = shadowViews[0];

        std::vector<Drawable*>& shadowCasters = shadowMap.shadowCasters[view.casterListIdx];
        octree->FindDrawablesMasked(shadowCasters, view.shadowFrustum, DF_GEOMETRYTU | DF_CAST_SHADOWSTU);
    }
}*/

/*void Renderer::ProcessShadowCastersWork(TaskTu*, unsigned)
{
    ZoneScoped;

    // Queue shadow batch collection tasks. These will also perform shadow batch sorting tasks when done
    if (drawShadows)
    {
        size_t shadowTaskIdx = 0;
        LightDrawable* lastLight = nullptr;

        for (size_t i = 0; i < NUM_SHADOW_MAPS; ++i)
        {
            ShadowMap& shadowMap = shadowMaps[i];
            for (size_t j = 0; j < shadowMap.shadowViews.size(); ++j)
            {
                LightDrawable* light = shadowMap.shadowViews[j]->light;
                // For a point light, make only one task that will handle all of the views and skip rest
                if (light->GetLightType() == LIGHT_POINT && light == lastLight)
                    continue;

                lastLight = light;

                if (collectShadowBatchesTasks.size() <= shadowTaskIdx)
                    collectShadowBatchesTasks.push_back(new CollectShadowBatchesTaskTu(this, &Renderer::CollectShadowBatchesWork));
                collectShadowBatchesTasks[shadowTaskIdx]->shadowMapIdx = i;
                collectShadowBatchesTasks[shadowTaskIdx]->viewIdx = j;
                numPendingShadowViews[i].fetch_add(1);
                ++shadowTaskIdx;
            }
        }

        if (shadowTaskIdx > 0)
            workQueue->QueueTasks(shadowTaskIdx, reinterpret_cast<TaskTu**>(&collectShadowBatchesTasks[0]));
    }

    // Clear per-cluster light data from previous frame, update cluster frustums and bounding boxes if camera changed, then queue light culling tasks for the needed scene range
    DefineClusterFrustums();
    memset(clusterData, 0, MAX_LIGHTS_CLUSTER * NUM_CLUSTER_X * NUM_CLUSTER_Y * NUM_CLUSTER_Z);
    for (size_t z = 0; z < NUM_CLUSTER_Z; ++z)
    {
        size_t idx = z * NUM_CLUSTER_X * NUM_CLUSTER_Y;
        const FrustumTu& clusterFrustum = clusterCullData[idx].frustum;
        if (minZ > clusterFrustum.vertices[4].z || maxZ < clusterFrustum.vertices[0].z)
            continue;
        workQueue->QueueTask(cullLightsTasks[z]);
    }

    // Finally copy correct shadow matrices for the localized light data
    // Note: directional light shadow matrices may still be pending, but they are not included here
    for (size_t i = 0; i < lights.size(); ++i)
    {
        LightDrawable* light = lights[i];

        if (light->ShadowMap())
        {
            lightData[i].shadowParameters = light->ShadowParameters();
            lightData[i].shadowMatrix = light->ShadowViews()[0].shadowMatrix;
        }
    }
}*/

/*void Renderer::CollectShadowBatchesWork(TaskTu* task_, unsigned)
{
    ZoneScoped;

    CollectShadowBatchesTaskTu* task = static_cast<CollectShadowBatchesTaskTu*>(task_);
    ShadowMap& shadowMap = shadowMaps[task->shadowMapIdx];
    size_t viewIdx = task->viewIdx;

    for (;;)
    {
        ShadowView& view = *shadowMap.shadowViews[viewIdx];

        LightDrawable* light = view.light;
        LightType lightType = light->GetLightType();

        float splitMinZ = minZ, splitMaxZ = maxZ;

        // Focus directional light shadow camera to the visible geometry combined bounds, and query for shadowcasters late
        if (lightType == LIGHT_DIRECTIONAL)
        {
            if (!light->SetupShadowView(viewIdx, camera, &geometryBounds))
                view.viewport = IntRect::ZERO;
            else
            {
                splitMinZ = Max(splitMinZ, view.splitMinZ);
                splitMaxZ = Min(splitMaxZ, view.splitMaxZ);

                // Before querying (which is potentially expensive), check for degenerate depth range or frustum outside split
                if (splitMinZ >= splitMaxZ || splitMinZ > view.splitMaxZ || splitMaxZ < view.splitMinZ)
                    view.viewport = IntRect::ZERO;
                else
                    octree->FindDrawablesMasked(shadowMap.shadowCasters[view.casterListIdx], view.shadowFrustum, DF_GEOMETRYTU | DF_CAST_SHADOWSTU);
            }
        }

        // Skip view? (no geometry, out of range or point light face not in view)
        if (view.viewport == IntRect::ZERO)
        {
            view.renderMode = RENDER_STATIC_LIGHT_CACHED;
            view.lastViewport = IntRect::ZERO;
        }
        else
        {
            const FrustumTu& shadowFrustum = view.shadowFrustum;
            const Matrix3x4& lightView = view.shadowCamera->ViewMatrix();
            const std::vector<Drawable*>& initialShadowCasters = shadowMap.shadowCasters[view.casterListIdx];

            bool dynamicOrDirLight = lightType == LIGHT_DIRECTIONAL || !light->IsStatic();
            bool dynamicCastersMoved = false;
            bool staticCastersMoved = false;

            size_t totalShadowCasters = 0;
            size_t staticShadowCasters = 0;

            FrustumTu lightViewFrustum = camera->WorldSplitFrustum(splitMinZ, splitMaxZ).Transformed(lightView);
            BoundingBoxTu lightViewFrustumBox(lightViewFrustum);

            BatchQueue* destStatic = !dynamicOrDirLight ? &shadowMap.shadowBatches[view.staticQueueIdx] : nullptr;
            BatchQueue* destDynamic = &shadowMap.shadowBatches[view.dynamicQueueIdx];

            for (auto it = initialShadowCasters.begin(); it != initialShadowCasters.end(); ++it)
            {
                Drawable* drawable = *it;
                const BoundingBoxTu& geometryBox = drawable->WorldBoundingBox();

                bool inView = drawable->InView(frameNumber);
                bool staticNode = drawable->IsStatic();

                // Check shadowcaster frustum visibility for point lights; may be visible in view, but not in each cube map face
                if (lightType == LIGHT_POINT && !shadowFrustum.IsInsideFast(geometryBox))
                    continue;

                // Furthermore, check by bounding box extrusion if out-of-view or directional light shadowcaster actually contributes to visible geometry shadowing or if it can be skipped
                // This is done only for dynamic objects or dynamic lights' shadows; cached static shadowmap needs to render everything
                if ((!staticNode || dynamicOrDirLight) && !inView)
                {
                    BoundingBoxTu lightViewBox = geometryBox.Transformed(lightView);

                    if (lightType == LIGHT_DIRECTIONAL)
                    {
                        lightViewBox.max.z = Max(lightViewBox.max.z, lightViewFrustumBox.max.z);
                        if (!lightViewFrustum.IsInsideFast(lightViewBox))
                            continue;
                    }
                    else
                    {
                        // For perspective lights, extrusion direction depends on the position of the shadow caster
                        Vector3 center = lightViewBox.Center();
                        Ray extrusionRay(center, center);

                        float extrusionDistance = view.shadowCamera->FarClip();
                        float originalDistance = Clamp(center.Length(), M_EPSILON, extrusionDistance);

                        // Because of the perspective, the bounding box must also grow when it is extruded to the distance
                        float sizeFactor = extrusionDistance / originalDistance;

                        // Calculate the endpoint box and merge it to the original. Because it's axis-aligned, it will be larger
                        // than necessary, so the test will be conservative
                        Vector3 newCenter = extrusionDistance * extrusionRay.direction;
                        Vector3 newHalfSize = lightViewBox.Size() * sizeFactor * 0.5f;
                        BoundingBoxTu extrudedBox(newCenter - newHalfSize, newCenter + newHalfSize);
                        lightViewBox.Merge(extrudedBox);

                        if (!lightViewFrustum.IsInsideFast(lightViewBox))
                            continue;
                    }
                }

                // If not in view, let the node prepare itself for render now
                if (!inView)
                {
                    if (!drawable->OnPrepareRender(frameNumber, camera))
                        continue;
                }

                ++totalShadowCasters;

                if (staticNode)
                {
                    ++staticShadowCasters;
                    if (drawable->LastUpdateFrameNumber() == frameNumber)
                        staticCastersMoved = true;
                }
                else
                {
                    if (drawable->LastUpdateFrameNumber() == frameNumber)
                        dynamicCastersMoved = true;
                }

                // If did not allocate a static queue, just put everything to dynamic
                BatchQueue& dest = destStatic ? (staticNode ? *destStatic : *destDynamic) : *destDynamic;
                const SourceBatches& batches = static_cast<GeometryDrawable*>(drawable)->Batches();
                size_t numGeometries = batches.NumGeometries();

                Batch newBatch;

                for (size_t j = 0; j < numGeometries; ++j)
                {
                    MaterialTu* material = batches.GetMaterial(j);
                    newBatch.pass = material->GetPass(PASS_SHADOW);
                    if (!newBatch.pass)
                        continue;

                    newBatch.geometry = batches.GetGeometry(j);
                    newBatch.programBits = (unsigned char)(drawable->Flags() & DF_GEOMETRY_TYPE_BITSTU);
                    newBatch.geomIndex = (unsigned char)j;

                    if (!newBatch.programBits)
                        newBatch.worldTransform = &drawable->WorldTransform();
                    else
                        newBatch.drawable = static_cast<GeometryDrawable*>(drawable);

                    dest.batches.push_back(newBatch);
                }
            }

            // Now determine which kind of caching can be used for the shadow map
            // Dynamic or directional lights
            if (dynamicOrDirLight)
            {
                // If light atlas allocation changed, light moved, or amount of objects in view changed, render an optimized shadow map
                if (view.lastViewport != view.viewport || !view.lastShadowMatrix.Equals(view.shadowMatrix, 0.0001f) || view.lastNumGeometries != totalShadowCasters || dynamicCastersMoved || staticCastersMoved)
                    view.renderMode = RENDER_DYNAMIC_LIGHT;
                else
                    view.renderMode = RENDER_STATIC_LIGHT_CACHED;
            }
            // Static lights
            else
            {
                // If light atlas allocation has changed, or the static light changed, render a full shadow map now that can be cached next frame
                if (view.lastViewport != view.viewport || !view.lastShadowMatrix.Equals(view.shadowMatrix, 0.0001f))
                    view.renderMode = RENDER_STATIC_LIGHT_STORE_STATIC;
                else
                {
                    view.renderMode = RENDER_STATIC_LIGHT_CACHED;

                    // If static shadowcasters updated themselves (e.g. LOD change), render shadow map fully
                    // If dynamic casters moved, need to restore shadowmap and rerender
                    if (staticCastersMoved)
                        view.renderMode = RENDER_STATIC_LIGHT_STORE_STATIC;
                    else
                    {
                        if (dynamicCastersMoved || view.lastNumGeometries != totalShadowCasters)
                            view.renderMode = staticShadowCasters > 0 ? RENDER_STATIC_LIGHT_RESTORE_STATIC : RENDER_DYNAMIC_LIGHT;
                    }
                }
            }

            // If no rendering to be done, use the last rendered shadow projection matrix to avoid artifacts when rotating camera
            if (view.renderMode == RENDER_STATIC_LIGHT_CACHED)
                view.shadowMatrix = view.lastShadowMatrix;
            else
            {
                view.lastViewport = view.viewport;
                view.lastNumGeometries = totalShadowCasters;
                view.lastShadowMatrix = view.shadowMatrix;

                // Clear static batch queue if not needed
                if (destStatic && view.renderMode != RENDER_STATIC_LIGHT_STORE_STATIC)
                    destStatic->Clear();
            }
        }

        // For a point light, process all its views in the same task
        if (lightType == LIGHT_POINT && viewIdx < shadowMap.shadowViews.size() - 1 && shadowMap.shadowViews[viewIdx + 1]->light == light)
            ++viewIdx;
        else
            break;
    }

    // Sort shadow batches if was the last
    if (numPendingShadowViews[task->shadowMapIdx].fetch_add(-1) == 1)
        SortShadowBatches(shadowMap);
}*/

void RegisterRendererLibrary(){
	static bool registered = false;
    if (registered)
        return;

    // Scene node base attributes are needed
	RegisterSceneLibrary();
	BoneTu::RegisterObject();
    CameraTu::RegisterObject();
    OctreeNodeTu::RegisterObject();
    GeometryNode::RegisterObject();
    StaticModel::RegisterObject();
    AnimatedModelTu::RegisterObject();
	LightTu::RegisterObject();
    LightEnvironment::RegisterObject();
    MaterialTu::RegisterObject();
    Model::RegisterObject();
    AnimationTu::RegisterObject();

    registered = true;
}
