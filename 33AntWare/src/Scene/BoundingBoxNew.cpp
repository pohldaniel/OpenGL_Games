#include "BoundingBoxNew.h"


void BoundingBoxNew::merge(const Vector3f* vertices, size_t count){
	while (count--)
		merge(*vertices++);
}

void BoundingBoxNew::merge(const Vector3f& point){
	
	if (point[0] < min[0])
		min[0] = point[0];
	if (point[1] < min[1])
		min[1] = point[1];
	if (point[2] < min[2])
		min[2] = point[2];
	if (point[0] > max[0])
		max[0] = point[0];
	if (point[1] > max[1])
		max[1] = point[1];
	if (point[2] > max[2])
		max[2] = point[2];
}

void  BoundingBoxNew::define(const Vector3f& point){
	min = max = point;
}

void BoundingBoxNew::undefine(){
	min = Vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
	max = -min;
}

void BoundingBoxNew::minmize(float factor) {
	min += Vector3f(factor * 0.5f);
	max -= Vector3f(factor * 0.5f);
}

void BoundingBoxNew::maximize(float factor) {
	min -= Vector3f(factor * 0.5f);
	max += Vector3f(factor * 0.5f);
}

Vector3f BoundingBoxNew::center() const { 
	return (max + min) * 0.5f; 
}

BoundingBoxNew BoundingBoxNew::transformed(const Matrix4f& transform) const {
	Vector3f oldCenter = center();
	Vector3f oldEdge = max - oldCenter;
	Vector3f newCenter = transform * oldCenter;
	Vector3f newEdge(
		std::fabs(transform[0][0]) * oldEdge[0] + std::fabs(transform[0][1]) * oldEdge[1] + std::fabs(transform[0][2]) * oldEdge[2],
		std::fabs(transform[1][0]) * oldEdge[0] + std::fabs(transform[1][1]) * oldEdge[1] + std::fabs(transform[1][2]) * oldEdge[2],
		std::fabs(transform[2][0]) * oldEdge[0] + std::fabs(transform[2][1]) * oldEdge[1] + std::fabs(transform[2][2]) * oldEdge[2]
	);

	return BoundingBoxNew(newCenter - newEdge, newCenter + newEdge);
}