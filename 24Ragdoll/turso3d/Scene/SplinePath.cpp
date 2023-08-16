#include "SplinePath.h"

//extern const char* interpolationModeNames[];
//extern const char* LOGIC_CATEGORY;

template<class T>
void safe_insert(std::vector<T>& vec, const size_t pos, const T arg)
{
	if (vec.size() <= pos) vec.resize(pos);
	vec.insert(vec.begin() + pos, arg);
}

const char* controlPointsStructureElementNames[] =
{
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
	UpdateNodeIds();
}

void SplinePath::ApplyAttributes()
{
	/*if (!dirty_)
		return;

	// Remove all old instance nodes before searching for new. Can not call RemoveAllInstances() as that would modify
	// the ID list on its own
	for (unsigned i = 0; i < controlPoints_.size(); ++i)
	{
		Node* node = controlPoints_[i];
		if (node)
			node->RemoveListener(this);
	}

	controlPoints_.clear();
	spline_.Clear();

	Scene* scene = GetScene();

	if (scene)
	{
		// The first index stores the number of IDs redundantly. This is for editing
		for (unsigned i = 1; i < controlPointIdsAttr_.size(); ++i)
		{
			Node* node = scene->GetNode(controlPointIdsAttr_[i].GetUInt());
			if (node)
			{
				WeakPtr<Node> controlPoint(node);
				node->AddListener(this);
				controlPoints_.Push(controlPoint);
				spline_.AddKnot(node->GetWorldPosition());
			}
		}

		Node* node = scene->GetNode(controlledIdAttr_);
		if (node)
		{
			WeakPtr<Node> controlled(node);
			controlledNode_ = controlled;
		}
	}

	CalculateLength();
	dirty_ = false;*/
}

void SplinePath::AddControlPoint(SpatialNode* point, unsigned index)
{
	if (!point)
		return;

	WeakPtr<SpatialNode> controlPoint(point);

	//point->AddListener(this);
	//controlPoints_.insert(controlPoints_.begin() + index, controlPoint);

	safe_insert(controlPoints_, index, controlPoint);
	spline_.AddKnot(point->WorldPosition(), index);

	UpdateNodeIds();
	CalculateLength();
}

void SplinePath::RemoveControlPoint(SpatialNode* point)
{
	if (!point)
		return;

	WeakPtr<Node> controlPoint(point);

	//point->RemoveListener(this);

	for (unsigned i = 0; i < controlPoints_.size(); ++i)
	{
		if (controlPoints_[i] == controlPoint)
		{
			controlPoints_.erase(controlPoints_.begin() + i);
			spline_.RemoveKnot(i);
			break;
		}
	}

	UpdateNodeIds();
	CalculateLength();
}

void SplinePath::ClearControlPoints()
{
	for (unsigned i = 0; i < controlPoints_.size(); ++i)
	{
		Node* node = controlPoints_[i];
		//if (node)
			//node->RemoveListener(this);
	}

	controlPoints_.clear();
	spline_.Clear();

	UpdateNodeIds();
	CalculateLength();
}

void SplinePath::SetControlledNode(SpatialNode* controlled)
{
	if (controlled)
		controlledNode_ = WeakPtr<SpatialNode>(controlled);
}

void SplinePath::SetInterpolationMode(InterpolationMode interpolationMode)
{
	spline_.SetInterpolationMode(interpolationMode);
	CalculateLength();
}

void SplinePath::SetPosition(float factor)
{
	float t = factor;

	if (t < 0.f)
		t = 0.0f;
	else if (t > 1.0f)
		t = 1.0f;

	traveled_ = t;
}

Vector3 SplinePath::GetPoint(float factor) const
{
	return spline_.GetPoint(factor);
}

void SplinePath::Move(float timeStep)
{
	if (traveled_ >= 1.0f || length_ <= 0.0f || !controlledNode_)
		return;

	elapsedTime_ += timeStep;

	// Calculate where we should be on the spline based on length, speed and time. If that is less than the set traveled_ don't move till caught up.
	float distanceCovered = elapsedTime_ * speed_;
	traveled_ = distanceCovered / length_;

	controlledNode_->SetWorldPosition(GetPoint(traveled_));
}

void SplinePath::Reset()
{
	traveled_ = 0.f;
	elapsedTime_ = 0.f;
}

void SplinePath::SetControlPointIdsAttr(const std::vector<unsigned int>& value)
{
	// Just remember the node IDs. They need to go through the SceneResolver, and we actually find the nodes during
	// ApplyAttributes()
	if (value.size())
	{
		controlPointIdsAttr_.clear();

		unsigned index = 0;
		unsigned numInstances = value[index++];
		// Prevent crash on entering negative value in the editor
		if (numInstances > M_MAX_INT)
			numInstances = 0;

		controlPointIdsAttr_.push_back(numInstances);
		while (numInstances--)
		{
			// If vector contains less IDs than should, fill the rest with zeros
			if (index < value.size())
				controlPointIdsAttr_.push_back(value[index++]);
			else
				controlPointIdsAttr_.push_back(0);
		}

		dirty_ = true;
	}
	else
	{
		controlPointIdsAttr_.clear();
		controlPointIdsAttr_.push_back(0);

		dirty_ = true;
	}
}

void SplinePath::SetControlledIdAttr(unsigned value)
{
	if (value > 0 && value < M_MAX_UNSIGNED)
		controlledIdAttr_ = value;

	dirty_ = true;
}

void SplinePath::OnMarkedDirty(SpatialNode* point)
{
	if (!point)
		return;

	WeakPtr<SpatialNode> controlPoint(point);

	for (unsigned i = 0; i < controlPoints_.size(); ++i)
	{
		if (controlPoints_[i] == controlPoint)
		{
			spline_.SetKnot(point->WorldPosition(), i);
			break;
		}
	}

	CalculateLength();
}

void SplinePath::OnNodeSetEnabled(SpatialNode* point)
{
	if (!point)
		return;

	WeakPtr<Node> controlPoint(point);

	for (unsigned i = 0; i < controlPoints_.size(); ++i)
	{
		if (controlPoints_[i] == controlPoint)
		{
			if (point->IsEnabled())
				spline_.AddKnot(point->WorldPosition(), i);
			else
				spline_.RemoveKnot(i);

			break;
		}
	}

	CalculateLength();
}

void SplinePath::UpdateNodeIds()
{
	unsigned numInstances = controlPoints_.size();

	controlPointIdsAttr_.clear();
	controlPointIdsAttr_.push_back(numInstances);

	for (unsigned i = 0; i < numInstances; ++i)
	{
		SpatialNode* node = controlPoints_[i];
		controlPointIdsAttr_.push_back(node ? node->Id() : 0);
	}
}

void SplinePath::CalculateLength()
{
	if (spline_.GetKnots().size() <= 0)
		return;

	length_ = 0.f;

	Vector3 a = spline_.GetKnot(0);
	for (float f = 0.000f; f <= 1.000f; f += 0.001f)
	{
		Vector3 b = spline_.GetPoint(f);
		length_ += Abs((a - b).Length());
		a = b;
	}
}