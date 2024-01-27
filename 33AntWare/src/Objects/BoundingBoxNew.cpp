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