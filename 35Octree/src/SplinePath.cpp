#include <engine/scene/OctreeNode.h>
#include <engine/octree/Octree.h>
#include <engine/DebugRenderer.h>
#include "SplinePath.h"


template<class T>
void safe_insert(std::vector<T>& vec, const size_t pos, const T arg){
	if (vec.size() <= pos) vec.resize(pos);
	vec.insert(vec.begin() + pos, arg);
}

const char* controlPointsStructureElementNames[] ={
	"Control Point Count",
	"   NodeID",
	0
};

SplinePath::SplinePath() :
	spline_(BEZIER_CURVE),
	speed_(1.f),
	elapsedTime_(0.f),
	traveled_(0.f),
	length_(0.f),
	dirty_(false),
	controlledIdAttr_(0)
{
	//UpdateNodeIds();
}

void SplinePath::ApplyAttributes(){
	
}

void SplinePath::AddControlPoint(SceneNodeLC* point, unsigned index){
	if (!point)
		return;

	safe_insert(controlPoints_, index, point);
	spline_.AddKnot(point->getWorldPosition(), index);

	//UpdateNodeIds();
	CalculateLength();
}

void SplinePath::RemoveControlPoint(SceneNodeLC* point){
	if (!point)
		return;

	for (unsigned i = 0; i < controlPoints_.size(); ++i){
		if (controlPoints_[i] == point){
			controlPoints_.erase(controlPoints_.begin() + i);
			spline_.RemoveKnot(i);
			break;
		}
	}

	//UpdateNodeIds();
	CalculateLength();
}

void SplinePath::ClearControlPoints(){
	for (unsigned i = 0; i < controlPoints_.size(); ++i){
		Node* node = controlPoints_[i];
	}

	controlPoints_.clear();
	spline_.Clear();

	//UpdateNodeIds();
	CalculateLength();
}

void SplinePath::SetControlledNode(SceneNodeLC* controlled){
	if (controlled)
		controlledNode_ = controlled;
}

void SplinePath::SetInterpolationMode(InterpolationMode interpolationMode){
	spline_.SetInterpolationMode(interpolationMode);
	CalculateLength();
}

void SplinePath::SetPosition(float factor){
	float t = factor;

	if (t < 0.f)
		t = 0.0f;
	else if (t > 1.0f)
		t = 1.0f;

	traveled_ = t;
}

Vector3f SplinePath::GetPoint(float factor) const{
	return spline_.GetPoint(factor);
}

void SplinePath::Move(float timeStep){
	if (traveled_ >= 1.0f || length_ <= 0.0f || !controlledNode_)
		return;

	elapsedTime_ += timeStep;

	// Calculate where we should be on the spline based on length, speed and time. If that is less than the set traveled_ don't move till caught up.
	float distanceCovered = elapsedTime_ * speed_;
	traveled_ = distanceCovered / length_;

	controlledNode_->setPosition(GetPoint(traveled_));
}

void SplinePath::Reset(){
	traveled_ = 0.f;
	elapsedTime_ = 0.f;
}

void SplinePath::SetControlPointIdsAttr(const std::vector<unsigned int>& value){
	// Just remember the node IDs. They need to go through the SceneResolver, and we actually find the nodes during
	// ApplyAttributes()
	if (value.size()){
		controlPointIdsAttr_.clear();

		unsigned index = 0;
		unsigned numInstances = value[index++];
		// Prevent crash on entering negative value in the editor
		if (numInstances > INT_MAX)
			numInstances = 0;

		controlPointIdsAttr_.push_back(numInstances);
		while (numInstances--){
			// If vector contains less IDs than should, fill the rest with zeros
			if (index < value.size())
				controlPointIdsAttr_.push_back(value[index++]);
			else
				controlPointIdsAttr_.push_back(0);
		}

		dirty_ = true;
	}else{
		controlPointIdsAttr_.clear();
		controlPointIdsAttr_.push_back(0);

		dirty_ = true;
	}
}

void SplinePath::SetControlledIdAttr(unsigned value){
	if (value > 0 && value < UINT_MAX)
		controlledIdAttr_ = value;

	dirty_ = true;
}

void SplinePath::OnMarkedDirty(SceneNodeLC* point){
	if (!point)
		return;

	for (unsigned i = 0; i < controlPoints_.size(); ++i){
		if (controlPoints_[i] == point){
			spline_.SetKnot(point->getWorldPosition(), i);
			break;
		}
	}

	CalculateLength();
}

void SplinePath::OnNodeSetEnabled(SceneNodeLC* point){
	if (!point)
		return;

	for (unsigned i = 0; i < controlPoints_.size(); ++i){
		if (controlPoints_[i] == point){
			if (true)
				spline_.AddKnot(point->getWorldPosition(), i);
			else
				spline_.RemoveKnot(i);

			break;
		}
	}

	CalculateLength();
}

/*void SplinePath::UpdateNodeIds(){
	unsigned numInstances = controlPoints_.size();

	controlPointIdsAttr_.clear();
	controlPointIdsAttr_.push_back(numInstances);

	for (unsigned i = 0; i < numInstances; ++i){
		SceneNodeLC* node = controlPoints_[i];
		controlPointIdsAttr_.push_back(node ? node->Id() : 0);
	}
}*/

void SplinePath::CalculateLength(){
	if (spline_.GetKnots().size() <= 0)
		return;

	length_ = 0.f;

	Vector3f a = spline_.GetKnot(0);
	for (float f = 0.000f; f <= 1.000f; f += 0.001f){
		Vector3f b = spline_.GetPoint(f);
		length_ += fabs((a - b).length());
		a = b;
	}
}

void SplinePath::OnRenderDebug(const Vector4f& color) {

	if (spline_.GetKnots().size() > 1){
		Vector3f a = spline_.GetPoint(0.0f);
		for (float f = 0.01f; f <= 1.0f; f = f + 0.01f){
			Vector3f b = spline_.GetPoint(f);
			DebugRenderer::Get().AddLine(a, b, color);
			a = b;
		}
	}

	for (std::vector<SceneNodeLC*>::const_iterator i = controlPoints_.begin(); i != controlPoints_.end(); ++i)
		DebugRenderer::Get().AddNode(*i);

	if (controlledNode_)
		DebugRenderer::Get().AddNode(controlledNode_);

}