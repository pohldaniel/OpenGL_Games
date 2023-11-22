#pragma once
#include "engine/collision/Collision.h"
#include "engine/Texture.h"
#include "engine/Quad.h"
#include "engine/Shader.h"
#include "Constants.h"

class Entity {
public:
	Entity(const float& dt, const float& fdt);
	virtual ~Entity();

	virtual void fixedUpdate() = 0;
	virtual void update() = 0;
	virtual void render() = 0;

	Collision getCollider();

	void setSize(const float x, const float y);
	void setSize(const Vector2f &size);
	void setPosition(const float x, const float y);
	void setPosition(const Vector2f &position);

	void setPosition2(const float x, const float y);
	void setPosition2(const Vector2f &position);

	void setOrigin(const float x, const float y);
	void setOrigin(const Vector2f &origin);

	void setOrigin2(const float x, const float y);
	void setOrigin2(const Vector2f &origin);

	const Vector2f &getPosition() const;
	const Vector2f &getSize() const;
	const Vector2f &getOrigin() const;

protected:
	Collider m_collider;

	unsigned int *m_textureAtlas;
	unsigned int *m_currentFrame;

	Vector2f m_position;
	Vector2f m_size;
	Vector2f m_origin;
	Matrix4f m_transform = Matrix4f::IDENTITY;

	const float& m_fdt;
	const float& m_dt;	
};