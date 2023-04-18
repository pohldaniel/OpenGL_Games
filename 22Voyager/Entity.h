#pragma once
#include <functional>
#include "engine/Cubemap.h"
#include "engine/Shader.h"
#include "RenderableObject.h"

class Entity : public RenderableObject {

public:

	Entity(const Shape& shape, const Shader* shader, const Cubemap& texture);
	virtual ~Entity();


	virtual void draw(const Camera& camera) override;
	virtual void draw(const Camera& camera, const Matrix4f& model) override;

	virtual void fixedUpdate();
	virtual void update(const float dt);
	void setUpdateFunction(std::function<void(const float dt)> fun);

private:

	const Shape& m_shape;
	const Shader* m_shader;	
	const Cubemap& m_texture;

	std::function<void(const float dt)> m_update = 0;

};