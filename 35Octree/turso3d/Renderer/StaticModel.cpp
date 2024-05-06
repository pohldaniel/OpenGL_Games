// For conditions of distribution and use, see copyright notice in License.txt

#include "../IO/Log.h"
#include "../Resource/ResourceCache.h"
#include "CameraTu.h"
#include "Model.h"
#include "OctreeTu.h"
#include "StaticModel.h"

#include <tracy/Tracy.hpp>

static Vector3 DOT_SCALE(1 / 3.0f, 1 / 3.0f, 1 / 3.0f);

static Allocator<StaticModelDrawable> drawableAllocator;

StaticModelDrawable::StaticModelDrawable() :
    lodBias(1.0f)
{
}

void StaticModelDrawable::OnWorldBoundingBoxUpdate() const
{
    if (model)
        worldBoundingBox = model->LocalBoundingBox().Transformed(WorldTransform());
    else
        Drawable::OnWorldBoundingBoxUpdate();
}

bool StaticModelDrawable::OnPrepareRender(unsigned short frameNumber, CameraTu* camera)
{
    distance = camera->Distance(WorldBoundingBox().Center());

    if (maxDistance > 0.0f && distance > maxDistance)
        return false;

    lastFrameNumber = frameNumber;

    // If model was last updated long ago, reset update framenumber to illegal
    if (frameNumber - lastUpdateFrameNumber == 0x8000)
        lastUpdateFrameNumber = 0;

    // Find out the new LOD level if model has LODs
    if (Flags() & DF_HAS_LOD_LEVELSTU)
    {
        float lodDistance = camera->LodDistance(distance, WorldScale().DotProduct(DOT_SCALE), lodBias);
        size_t numGeometries = batches.NumGeometries();

        for (size_t i = 0; i < numGeometries; ++i)
        {
            const std::vector<SharedPtr<Geometry> >& lodGeometries = model->LodGeometries(i);
            if (lodGeometries.size() > 1)
            {
                size_t j;
                for (j = 1; j < lodGeometries.size(); ++j)
                {
                    if (lodDistance <= lodGeometries[j]->lodDistance)
                        break;
                }
                if (batches.GetGeometry(i) != lodGeometries[j - 1])
                {
                    batches.SetGeometry(i, lodGeometries[j - 1]);
                    lastUpdateFrameNumber = frameNumber;
                }
            }
        }
    }

    return true;
}

StaticModel::StaticModel()
{
    drawable = drawableAllocator.Allocate();
    drawable->SetOwner(this);
}

StaticModel::~StaticModel()
{
    if (drawable)
    {
        RemoveFromOctree();
        drawableAllocator.Free(static_cast<StaticModelDrawable*>(drawable));
        drawable = nullptr;
    }
}

void StaticModel::RegisterObject()
{
    RegisterFactory<StaticModel>();
    // Copy base attributes from OctreeNode instead of GeometryNode, as the model attribute needs to be set first so that there is the correct amount of materials to assign
    CopyBaseAttributes<StaticModel, OctreeNodeTu>();
    RegisterDerivedType<StaticModel, GeometryNode>();
    RegisterMixedRefAttribute("model", &StaticModel::ModelAttr, &StaticModel::SetModelAttr, ResourceRef(Model::TypeStatic()));
    CopyBaseAttribute<StaticModel, GeometryNode>("materials");
    RegisterAttribute("lodBias", &StaticModel::LodBias, &StaticModel::SetLodBias, 1.0f);
}

void StaticModel::SetModel(Model* model)
{
    ZoneScoped;

    StaticModelDrawable* modelDrawable = static_cast<StaticModelDrawable*>(drawable);

    modelDrawable->model = model;
    modelDrawable->SetFlag(DF_HAS_LOD_LEVELSTU, false);

    if (model)
    {
        SetNumGeometries(model->NumGeometries());
        // Start at LOD level 0
        for (size_t i = 0; i < model->NumGeometries(); ++i)
        {
            SetGeometry(i, model->GetGeometry(i, 0));
            if (model->NumLodLevels(i) > 1)
                modelDrawable->SetFlag(DF_HAS_LOD_LEVELSTU, true);
        }
    }
    else
    {
        SetNumGeometries(0);
    }

    OnBoundingBoxChanged();
}

void StaticModel::SetLodBias(float bias)
{
    StaticModelDrawable* modelDrawable = static_cast<StaticModelDrawable*>(drawable);
    modelDrawable->lodBias = Max(bias, M_EPSILON);
}

Model* StaticModel::GetModel() const
{
    return static_cast<StaticModelDrawable*>(drawable)->model;
}

void StaticModel::SetModelAttr(const ResourceRef& value)
{
    ResourceCache* cache = Subsystem<ResourceCache>();
    SetModel(cache->LoadResource<Model>(value.name));
}

ResourceRef StaticModel::ModelAttr() const
{
    return ResourceRef(Model::TypeStatic(), ResourceName(GetModel()));
}
