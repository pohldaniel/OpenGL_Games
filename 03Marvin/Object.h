#pragma once
#include "engine/Vector.h"

namespace Category {
	enum Type {
		None = 1 << 0,
		Walkable = 1 << 1,
		Player = 1 << 2,
		Enemy = 1 << 3,
		GrassBlock = 1 << 4,
		GrassPlatform = 1 << 5,
		Seeker = 1 << 6,
		Gem = 1 << 7,
		Exit = 1 << 8,
		Damager = 1 << 9,
		SoundEffect = 1 << 10
	};
}

class Object {

public:
	Object(unsigned int category);

	void setCategory(unsigned int category);		
	void setOrigin(const float x, const float y);
	void setOrigin(const Vector2f &origin);
	virtual void setSize(const float x, const float y);
	virtual void setSize(const Vector2f &size);
	virtual void setPosition(const float x, const float y);
	virtual void setPosition(const Vector2f &position);

	const Vector2f &getPosition() const;
	const Vector2f &getSize() const;
	const Vector2f &getOrigin() const;
	const unsigned int getCategory() const;

protected:
	unsigned int m_category;

	Vector2f m_position;
	Vector2f m_size;
	Vector2f m_origin;
	Matrix4f m_transform = Matrix4f::IDENTITY;
};