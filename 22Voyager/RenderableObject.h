#pragma once

#include "engine/MeshObject/Shape.h"
#include "engine/Transform.h"
#include "engine/Camera.h"
#include "Object.h"

class RenderableObject : public Object {

public:

	RenderableObject(const Vector3f &position = Vector3f(0.0f, 0.0f, 0.0f), const Vector3f &scale = Vector3f(1.0f, 1.0f, 1.0f));
	RenderableObject(const Vector3f &position, const Vector3f &scale, std::string shape, std::string shader, std::string texture);
	virtual ~RenderableObject();

	virtual void draw(const Camera& camera);
	virtual void draw(const Camera& camera, const Matrix4f& model);
	

	void setDisabled(bool disabled);
	bool isDisabled();
	void setAttributes(std::string shape, std::string shader, std::string texture);

protected:

	bool m_disabled = false;
	static Transform Transform;

private:

	std::string m_shape;
	std::string m_shader;
	std::string m_texture;
};