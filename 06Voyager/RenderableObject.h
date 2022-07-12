#pragma once
#include "engine/Vector.h"
#include "engine/Shader.h"
#include "engine/ObjModel.h"

#include "Constants.h"
#include "Object.h"

class RenderableObject : public Object {

public:
	RenderableObject();
	//virtual ~RenderableObject();

	virtual void render(const Camera& camera);
	virtual void renderShadow(const Camera& camera);

	void setDisabled(bool disabled);
	bool isDisabled();

protected:
	Shader *m_shader;
	Model *m_model;
	Texture *m_texture;

	bool m_disabled = false;
};