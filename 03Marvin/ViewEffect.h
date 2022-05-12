#pragma once
#include "engine/Vector.h"
#include "Constants.h"

class ViewEffect {
public:
	static ViewEffect& get();

	const Matrix4f& getView();

	void update();
	void init();
	void setPosition(const Vector2f &position);

	Vector3f m_position;
	Vector3f m_target;
	Vector3f m_origin = Vector3f(WIDTH * 0.25f, HEIGHT * 0.25f, 0.0f);
private:
	ViewEffect() { }
	~ViewEffect() {}

	Matrix4f m_view = Matrix4f::IDENTITY;
	

	static ViewEffect s_instance;

	void applyEffect();

};