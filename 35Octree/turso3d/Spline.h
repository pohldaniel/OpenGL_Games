#pragma once

#include <vector>
#include "Math/Vector3.h"

enum InterpolationMode
{
	/// Bezier interpolation.
	BEZIER_CURVE = 0,
	/// Catmull-Rom interpolation. The first and last knots control velocity and are not included on the path.
	CATMULL_ROM_CURVE,
	/// Linear interpolation.
	LINEAR_CURVE,
	/// Catmull-Rom full path interpolation. Start and end knots are duplicated or looped as necessary to move through the full path.
	CATMULL_ROM_FULL_CURVE
};

/// Spline class to get a point on it based off the interpolation mode.
class Spline
{
public:
	/// Default constructor.
	Spline();
	/// Constructor setting interpolation mode.
	Spline(InterpolationMode mode);
	/// Constructor setting knots and interpolation mode.
	Spline(const std::vector<Vector3>& knots, InterpolationMode mode = BEZIER_CURVE);
	/// Copy constructor.
	Spline(const Spline& rhs);

	/// Copy operator.
	void operator =(const Spline& rhs)
	{
		knots_ = rhs.knots_;
		interpolationMode_ = rhs.interpolationMode_;
	}

	/// Equality operator.
	bool operator ==(const Spline& rhs) const
	{
		return (knots_ == rhs.knots_ && interpolationMode_ == rhs.interpolationMode_);
	}

	/// Inequality operator.
	bool operator !=(const Spline& rhs) const
	{
		return !(*this == rhs);
	}

	/// Return the interpolation mode.
	InterpolationMode GetInterpolationMode() const { return interpolationMode_; }

	/// Return the knots of the spline.
	const std::vector<Vector3>& GetKnots() const { return knots_; }

	/// Return the knot at the specific index.
	Vector3 GetKnot(unsigned index) const { return knots_[index]; }

	/// Return the T of the point of the spline at f from 0.f - 1.f.
	Vector3 GetPoint(float f) const;

	/// Set the interpolation mode.
	void SetInterpolationMode(InterpolationMode interpolationMode) { interpolationMode_ = interpolationMode; }

	/// Set the knots of the spline.
	void SetKnots(const std::vector<Vector3>& knots) { knots_ = knots; }

	/// Set the value of an existing knot.
	void SetKnot(const Vector3& knot, unsigned index);
	/// Add a knot to the end of the spline.
	void AddKnot(const Vector3& knot);
	/// Add a knot to the spline at a specific index.
	void AddKnot(const Vector3& knot, unsigned index);

	/// Remove the last knot on the spline.
	void RemoveKnot() { knots_.pop_back(); }

	/// Remove the knot at the specific index.
	void RemoveKnot(unsigned index) { knots_.erase(knots_.begin() + index); }

	/// Clear the spline.
	void Clear() { knots_.clear(); }

private:
	/// Perform Bezier interpolation on the spline.
	Vector3 BezierInterpolation(const std::vector<Vector3>& knots, float t) const;
	/// Perform Spline interpolation on the spline.
	Vector3 CatmullRomInterpolation(const std::vector<Vector3>& knots, float t) const;
	/// Perform linear interpolation on the spline.
	Vector3 LinearInterpolation(const std::vector<Vector3>& knots, float t) const;
	/// Linear interpolation between two Variants based on underlying type.
	Vector3 LinearInterpolation(const Vector3& lhs, const Vector3& rhs, float t) const;

	/// Interpolation mode.
	InterpolationMode interpolationMode_;
	/// Knots on the spline.
	std::vector<Vector3> knots_;
};