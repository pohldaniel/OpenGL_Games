#pragma once
#include "engine/Vector.h"
#include "engine/Shader.h"
#include "ObjModel.h"

#include "Constants.h"
#include "Object.h"

class RenderableObject : public Object {

public:
	RenderableObject();
	//virtual ~RenderableObject();

	virtual void render(const Camera& camera);

	void setDisabled(bool disabled);
	bool isDisabled();

private:
	Shader *m_shader;
	Model *m_model;
	bool m_disabled = false;
};