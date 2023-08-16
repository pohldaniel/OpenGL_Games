#pragma once
#include "SpatialNode.h"
#include "../Spline.h"


/// Spline for creating smooth movement based on Speed along a set of Control Points modified by the Interpolation Mode.
class SplinePath : public SpatialNode {
	OBJECT(SplinePath);

public:
	/// Construct an Empty SplinePath.
	SplinePath();

	/// Destructor.
	virtual ~SplinePath() { };

	/// Apply Attributes to the SplinePath.
	virtual void ApplyAttributes();
	/// Add a Node to the SplinePath as a Control Point.
	void AddControlPoint(SpatialNode* point, unsigned index = M_MAX_UNSIGNED);
	/// Remove a Node Control Point from the SplinePath.
	void RemoveControlPoint(SpatialNode* point);
	/// Clear the Control Points from the SplinePath.
	void ClearControlPoints();

	/// Set the Interpolation Mode.
	void SetInterpolationMode(InterpolationMode interpolationMode);

	/// Set the movement Speed.
	void SetSpeed(float speed) { speed_ = speed; }

	/// Set the controlled Node's position on the SplinePath.
	void SetPosition(float factor);
	/// Set the Node to be moved along the SplinePath.
	void SetControlledNode(SpatialNode* controlled);

	/// Get the Interpolation Mode.
	InterpolationMode GetInterpolationMode() const { return spline_.GetInterpolationMode(); }

	/// Get the movement Speed.
	float GetSpeed() const { return speed_; }

	/// Get the length of SplinePath;
	float GetLength() const { return length_; }
	/// Get the traveled length of SplinePath;
	float GetTraveled() const { return traveled_; }

	/// Get the parent Node's last position on the spline.
	Vector3 GetPosition() const { return GetPoint(traveled_); }

	/// Get the controlled Node.
	SpatialNode* GetControlledNode() const { return controlledNode_; }

	/// Get a point on the SplinePath from 0.f to 1.f where 0 is the start and 1 is the end.
	Vector3 GetPoint(float factor) const;

	/// Move the controlled Node to the next position along the SplinePath based off the Speed value.
	void Move(float timeStep);
	/// Reset movement along the path.
	void Reset();

	/// Returns whether the movement along the SplinePath is complete.
	bool IsFinished() const { return traveled_ >= 1.0f; }

	/// Set Control Point Node IDs attribute.
	void SetControlPointIdsAttr(const std::vector<unsigned int>& value);

	/// Return Control Point Node IDs attribute.
	const std::vector<unsigned int>& GetControlPointIdsAttr() const { return controlPointIdsAttr_; }

	/// Set Controlled Node ID attribute.
	void SetControlledIdAttr(unsigned value);

	/// Get Controlled Node ID attribute.
	unsigned GetControlledIdAttr() const { return controlledIdAttr_; }

protected:

	//void OnTransformChanged() override;

	/// Listener to manage Control Point movement.
	void OnMarkedDirty(SpatialNode* point);
	/// Listener to manage Control Point enabling.
	void OnNodeSetEnabled(SpatialNode* point);

private:
	/// Update the Node IDs of the Control Points.
	void UpdateNodeIds();
	/// Calculate the length of the SplinePath. Used for movement calculations.
	void CalculateLength();

	/// The Control Points of the Spline.
	Spline spline_;
	/// The Speed of movement along the Spline.
	float speed_;
	/// Amount of time that has elapsed while moving.
	float elapsedTime_;
	/// The fraction of the SplinePath covered.
	float traveled_;
	/// The length of the SplinePath.
	float length_;
	/// Whether the Control Point IDs are dirty.
	bool dirty_;
	/// Node to be moved along the SplinePath.
	WeakPtr<SpatialNode> controlledNode_;
	/// Control Points for the SplinePath.
	std::vector<WeakPtr<SpatialNode> > controlPoints_;
	/// Control Point ID's for the SplinePath.
	mutable std::vector<unsigned int> controlPointIdsAttr_;
	/// Controlled ID for the SplinePath.
	mutable unsigned controlledIdAttr_;
};