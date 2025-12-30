#pragma once

class EntityNew {

public:

	EntityNew();
	EntityNew& operator=(const EntityNew& rhs);
	EntityNew& operator=(EntityNew&& rhs) noexcept;
	EntityNew(EntityNew const& rhs);
	EntityNew(EntityNew&& rhs) noexcept;

	virtual ~EntityNew();
	virtual void update(float dt) = 0;
};