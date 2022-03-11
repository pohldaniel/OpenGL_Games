#pragma once

#include "Constants.h"
#include "Shader.h"
#include "Quad.h"

class Light {
public:
	Light() = default;
	Light(Light const& rhs);
	Light(const Vector2f &position, float radius);
	~Light();
	void render() const;
	void setPosition(const Vector2f &position);
	void setOrigin(const Vector2f &origin);

	Shader& getShader() const;
private:
	Vector2f m_position;
	Vector2f m_origin;

	Shader* m_shader;
	Quad *m_quad;

	Matrix4f m_transform = Matrix4f::IDENTITY;

	float m_move = 0.0f;	
};