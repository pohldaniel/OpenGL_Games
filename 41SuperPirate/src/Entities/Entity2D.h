#pragma once

class Entity2D {

public:

	Entity2D();
	Entity2D& operator=(const Entity2D& rhs);
	Entity2D& operator=(Entity2D&& rhs) noexcept;
	Entity2D(Entity2D const& rhs);
	Entity2D(Entity2D&& rhs) noexcept;

	virtual ~Entity2D();
	virtual void update(float dt) = 0;
};