#pragma once
#include <functional>
#include "engine/MeshObject/Shape.h"
#include "engine/Camera.h"
#include "Object.h"

class RenderableObject : public Object {

public:

	RenderableObject(const Vector3f &position = Vector3f(0.0f, 0.0f, 0.0f), const Vector3f &scale = Vector3f(1.0f, 1.0f, 1.0f));
	RenderableObject(std::string shape, std::string shader, std::string texture);
	RenderableObject(std::string shader);
	RenderableObject(RenderableObject const& rhs);
	RenderableObject& operator=(const RenderableObject& rhs);
	virtual ~RenderableObject();

	virtual void draw(const Camera& camera, bool viewIndependent = false);
	virtual void update(const float dt);

	void setDisabled(bool disabled);
	bool isDisabled();
	void setAttributes(std::string shape, std::string shader, std::string texture);
	void setUpdateFunction(std::function<void(const float dt)> fun);
	void setDrawFunction(std::function<void(const Camera& camera, bool viewIndependent)> fun);
	std::string& getShape();
	std::string& getShader();
	std::string& getTexture();

private:

	void drawDefault(const Camera& camera, bool viewIndependent = false);

	std::string m_shape;
	std::string m_shader;
	std::string m_texture;
	bool m_disabled = false;
	std::function<void(const float dt)> m_update = 0;
	std::function<void(const Camera& camera, bool viewIndependent)> m_draw = 0;
};