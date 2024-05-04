#include "Spline.h"
#include "IO/Log.h"

template<class T>
void safe_insert(std::vector<T>& vec, const size_t pos, const T arg)
{
	if (vec.size() <= pos) vec.resize(pos);
	vec.insert(vec.begin() + pos, arg);
}

const char* interpolationModeNames[] =
{
	"Bezier",
	"Catmull-Rom",
	"Linear",
	"Catmull-Rom Full",
	0
};

Spline::Spline() : interpolationMode_(BEZIER_CURVE) {
}

Spline::Spline(InterpolationMode mode) : interpolationMode_(mode) {
}

Spline::Spline(const std::vector<Vector3>& knots, InterpolationMode mode) : interpolationMode_(mode), knots_(knots) {
}

Spline::Spline(const Spline& rhs) : interpolationMode_(rhs.interpolationMode_), knots_(rhs.knots_) {
}

Vector3 Spline::GetPoint(float f) const
{
	if (knots_.size() < 2)
		return knots_.size() == 1 ? knots_[0] : Vector3(0.0f, 0.0f, 0.0f);

	if (f > 1.f)
		f = 1.f;
	else if (f < 0.f)
		f = 0.f;

	switch (interpolationMode_)
	{
	case BEZIER_CURVE:
		return BezierInterpolation(knots_, f);
	case CATMULL_ROM_CURVE:
		return CatmullRomInterpolation(knots_, f);
	case LINEAR_CURVE:
		return LinearInterpolation(knots_, f);
	case CATMULL_ROM_FULL_CURVE:
	{
		/// \todo Do not allocate a new vector each time
		std::vector<Vector3> fullKnots;
		if (knots_.size() > 1)
		{
			// Non-cyclic case: duplicate start and end
			if (knots_.front() != knots_.back())
			{
				fullKnots.push_back(knots_.front());
				fullKnots.insert(fullKnots.end(), knots_.begin(), knots_.end());
				fullKnots.push_back(knots_.back());
			}
			// Cyclic case: smooth the tangents
			else
			{
				fullKnots.push_back(knots_[knots_.size() - 2]);
				fullKnots.insert(fullKnots.end(), knots_.begin(), knots_.end());
				fullKnots.push_back(knots_[1]);
			}
		}
		return CatmullRomInterpolation(fullKnots, f);
	}

	default:
		LOGERROR("Unsupported interpolation mode");
		return Vector3(0.0f, 0.0f, 0.0f);
	}
}

void Spline::SetKnot(const Vector3& knot, unsigned index) {
	if (index < knots_.size()){
		if (knots_.size() > 0)
			knots_[index] = knot;
		else if (knots_.empty())
			knots_.push_back(knot);
		else
			LOGERROR("Attempted to set a Spline's Knot value of type %s where elements are already using %s");
	}
}

void Spline::AddKnot(const Vector3& knot)
{
	if (knots_.size() > 0)
		knots_.push_back(knot);
	else if (knots_.empty())
		knots_.push_back(knot);
	else
		LOGERROR("Attempted to add Knot to Spline of type %s where elements are already using %s");
}

void Spline::AddKnot(const Vector3& knot, unsigned index)
{
	if (index > knots_.size())
		index = knots_.size();

	if (knots_.size() > 0) {
		knots_.insert(knots_.begin() + index, knot);
		//safe_insert(knots_, index, knot);
	}else if (knots_.empty())
		knots_.push_back(knot);
	else
		LOGERROR("Attempted to add Knot to Spline of type %s where elements are already using %s");
}

Vector3 Spline::BezierInterpolation(const std::vector<Vector3>& knots, float t) const
{
	if (knots.size() == 2){
		return LinearInterpolation(knots[0], knots[1], t);
	}else
	{
		/// \todo Do not allocate a new vector each time
		std::vector<Vector3> interpolatedKnots;
		for (unsigned i = 1; i < knots.size(); i++)
		{
			interpolatedKnots.push_back(LinearInterpolation(knots[i - 1], knots[i], t));
		}
		return BezierInterpolation(interpolatedKnots, t);
	}
}

Vector3 Spline::LinearInterpolation(const std::vector<Vector3>& knots, float t) const{
	if (knots.size() < 2)
		return Vector3(0.0f, 0.0f, 0.0f);
	else{
		if (t >= 1.f)
			return knots.back();

		int originIndex = Clamp((int)(t * (knots.size() - 1)), 0, (int)(knots.size() - 2));
		t = fmodf(t * (knots.size() - 1), 1.f);
		return LinearInterpolation(knots[originIndex], knots[originIndex + 1], t);
	}
}

Vector3 Spline::LinearInterpolation(const Vector3& lhs, const Vector3& rhs, float t) const{
	return lhs.Lerp(rhs, t);
}

Vector3 CalculateCatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t, float t2, float t3) {
	return Vector3(0.5f * ((2.0f * p1) + (-p0 + p2) * t +
		(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
		(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3));
}

Vector3 Spline::CatmullRomInterpolation(const std::vector<Vector3>& knots, float t) const {
	if (knots.size() < 4)
		return Vector3(0.0f, 0.0f, 0.0f);
	else{
		if (t >= 1.f)
			return knots[knots.size() - 2];

		int originIndex = static_cast<int>(t * (knots.size() - 3));
		t = fmodf(t * (knots.size() - 3), 1.f);
		float t2 = t * t;
		float t3 = t2 * t;

		return CalculateCatmullRom(knots[originIndex], knots[originIndex + 1], knots[originIndex + 2], knots[originIndex + 3], t, t2, t3);
	}
}