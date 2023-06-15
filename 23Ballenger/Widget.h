#pragma once

#include "engine/input/Event.h"
#include "engine/Vector.h"

class Widget {

public:

	Widget();
	Widget(Widget const& rhs);
	Widget& operator=(const Widget& rhs);
	~Widget();

	virtual void draw() = 0;
	virtual void processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button = Event::MouseButtonEvent::MouseButton::NONE) = 0;

	virtual void setPosition(const float x, const float y);
	virtual void setPosition(const Vector2f& position);
	virtual void setSize(const float sx, const float sy);
	virtual void setSize(const Vector2f& size);

	const Vector2f& getPosition() const;
	const Vector2f& getSize() const;

	static void Resize(unsigned int width, unsigned int height);

protected:

	Matrix4f m_transform;
	Vector2f m_position;
	Vector2f m_size;

	static Matrix4f Orthographic;
};