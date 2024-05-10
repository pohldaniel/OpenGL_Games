#pragma once
#include <engine/scene/SceneNodeLC.h>
#include "Spline.h"

class SplinePath : public SceneNodeLC {


public:

	SplinePath();
	virtual ~SplinePath() { };


	virtual void ApplyAttributes();
	void AddControlPoint(SceneNodeLC* point, unsigned index = UINT_MAX);
	void RemoveControlPoint(SceneNodeLC* point);
	void ClearControlPoints();


	void SetInterpolationMode(InterpolationMode interpolationMode);
	void SetSpeed(float speed) { speed_ = speed; }
	void SetPosition(float factor);
	void SetControlledNode(SceneNodeLC* controlled);


	InterpolationMode GetInterpolationMode() const { return spline_.GetInterpolationMode(); }
	float GetSpeed() const { return speed_; }


	float GetLength() const { return length_; }
	float GetTraveled() const { return traveled_; }


	Vector3f GetPosition() const { return GetPoint(traveled_); }
	SceneNodeLC* GetControlledNode() const { return controlledNode_; }

	Vector3f GetPoint(float factor) const;

	void Move(float timeStep);
	void Reset();
	bool IsFinished() const { return traveled_ >= 1.0f; }
	void SetControlPointIdsAttr(const std::vector<unsigned int>& value);
	const std::vector<unsigned int>& GetControlPointIdsAttr() const { return controlPointIdsAttr_; }
	void SetControlledIdAttr(unsigned value);
	unsigned GetControlledIdAttr() const { return controlledIdAttr_; }

protected:

	void OnMarkedDirty(SceneNodeLC* point);
	void OnNodeSetEnabled(SceneNodeLC* point);

private:

	//void UpdateNodeIds();
	void CalculateLength();

	Spline spline_;
	float speed_;
	float elapsedTime_;
	float traveled_;
	float length_;
	bool dirty_;
	SceneNodeLC* controlledNode_;
	std::vector<SceneNodeLC*> controlPoints_;
	mutable std::vector<unsigned int> controlPointIdsAttr_;
	mutable unsigned controlledIdAttr_;
};