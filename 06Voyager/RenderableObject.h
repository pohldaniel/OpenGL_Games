#pragma once
#include "engine/Vector.h"
#include "engine/Shader.h"
#include "engine/ObjModel.h"

#include "Constants.h"

class RenderableObject {

public:
	RenderableObject();
	//virtual ~RenderableObject();
	virtual void drawRaw();
	virtual void draw(const Camera& camera);
	virtual void drawShadow(const Camera& camera);	
	virtual void drawAABB(const Camera& camera);
	virtual void drawSphere(const Camera& camera);
	virtual void drawHull(const Camera& camera);

	virtual BoundingBox& getAABB();
	virtual BoundingSphere& getBoundingSphere();
	virtual ConvexHull& getConvexHull();

	void setDisabled(bool disabled);
	bool isDisabled();

	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);

	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix();
	const ObjModel* getModel() const;
	unsigned int getId() const;

	Transform m_transform;

protected:
	Shader *m_shader;
	ObjModel *m_model;
	Texture *m_texture;
	
	Vector4f m_pickColor;
	Shader* m_colorShader;

	bool m_disabled = false;
	unsigned int m_id;

	static unsigned int s_id;
};