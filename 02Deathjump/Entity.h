#pragma once
#include "Constants.h"
#include "Collision.h"
#include "Texture.h"
#include "Quad.h"
#include "Shader.h"

class Entity {
public:
	Entity(const float& dt, const float& fdt);
	virtual ~Entity();

	virtual void fixedUpdate() = 0;
	virtual void update() = 0;
	virtual void render() const = 0;


	Collision GetCollider();

	void setSize(float x, float y);
	void setSize(const Vector2f &size);
	void setPosition(float x, float y);
	void setPosition(const Vector2f &position);

	const Vector2f &Entity::getPosition() const;
	const Vector2f &Entity::getSize() const;

protected:
	Collider i_collider;

	unsigned int *m_textureAtlas;
	unsigned int *m_currentFrame;

	Vector2f m_position;
	Vector2f m_size;
	Matrix4f m_transform = Matrix4f::IDENTITY;

	const float& i_fdt;
	const float& i_dt;

	const float xTrans = 2.0f / (float)(WIDTH);
	const float yTrans = 2.0f / (float)(HEIGHT);

	float xScale;
	float yScale;
};