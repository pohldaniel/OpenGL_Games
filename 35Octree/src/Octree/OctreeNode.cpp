#include "Octree.h"

OctreeNodeBase::OctreeNodeBase() : octree(nullptr), drawable(nullptr){
}

void OctreeNode::SetStatic(bool enable){
    /*if (enable != IsStatic())
    {
        drawable->SetFlag(DF_STATIC, enable);
        // Reinsert into octree so that cached shadow map invalidation is handled
        OnBoundingBoxChanged();
    }*/
	OnBoundingBoxChanged();
}

void OctreeNode::OnTransformChanged(){
	SceneNodeLC::OnTransformChanged();

	drawable->OnBoundingBoxChanged();
	drawable->OnTransformChanged();

    if (drawable->getOctant() && !drawable->m_reinsertQueued)
        octree->QueueUpdate(drawable);
}

void OctreeNode::OnBoundingBoxChanged(){
	drawable->OnBoundingBoxChanged();
    if (drawable->getOctant() && !drawable->m_reinsertQueued)
        octree->QueueUpdate(drawable);
}

void OctreeNode::RemoveFromOctree(){
    if (octree){
        octree->RemoveDrawable(drawable);
        octree = nullptr;
    }
}