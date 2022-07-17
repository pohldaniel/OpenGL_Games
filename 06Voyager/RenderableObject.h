#pragma once
#include "engine/Vector.h"
#include "engine/Shader.h"
#include "engine/ObjModel.h"

#include "Constants.h"

class RenderableObject {

public:
	RenderableObject();
	//virtual ~RenderableObject();

	virtual void draw(const Camera& camera);
	virtual void drawShadow(const Camera& camera);
	virtual void drawRaw();

	void setDisabled(bool disabled);
	bool isDisabled();

	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);
	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix() const;

protected:
	Shader *m_shader;
	Model *m_model;
	Texture *m_texture;
	ModelMatrix m_modelMatrix;

	bool m_disabled = false;
};