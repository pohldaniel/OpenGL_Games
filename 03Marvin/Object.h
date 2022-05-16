#pragma once
#include "engine/Vector.h"

namespace Category {
	enum Type {
		None = 0,
		Walkable = 1 << 0,
		Player = 1 << 1,
		Enemy = 1 << 2,
		GrassBlock = 1 << 3,
		GrassPlatform = 1 << 4,
		Seeker = 1 << 5,
		Gem = 1 << 6,
		Exit = 1 << 7,
		Damager = 1 << 8,
		SoundEffect = 1 << 9
	};
}

class Object {

public:
	Object(unsigned int category);

	void setCategory(unsigned int category);
	void setSize(const float x, const float y);
	void setSize(const Vector2f &size);
	void setPosition(const float x, const float y);
	void setPosition(const Vector2f &position);
	void setOrigin(const float x, const float y);
	void setOrigin(const Vector2f &origin);

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