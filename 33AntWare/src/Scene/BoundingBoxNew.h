#pragma once
#include <engine/Vector.h>
#include <engine/BoundingBox.h>

class BoundingBoxNew {

public:

	Vector3f min;
	Vector3f max;

	BoundingBoxNew() : min(Vector3f(FLT_MAX, FLT_MAX, FLT_MAX)), max(Vector3f(-FLT_MAX, -FLT_MAX, -FLT_MAX)){
	}

	BoundingBoxNew(const BoundingBoxNew& box) : min(box.min),max(box.max){
	
	}

	BoundingBoxNew(const Vector3f& min, const Vector3f& max) : min(min),max(max){
	
	}

	BoundingBoxNew(float min, float max) : min(Vector3f(min)), max(Vector3f(max)){
	
	}

	BoundingBoxNew(const BoundingBox& box) : min(box.position), max (box.position + box.size){

	}

	BoundingBoxNew transformed(const Matrix4f& transform) const;
	void define(const Vector3f& point);

private:

	void merge(const Vector3f& point);
	void merge(const Vector3f* vertices, size_t count);
	void undefine();
	void minmize(float factor);
	void maximize(float factor);
	Vector3f center() const;

	
};