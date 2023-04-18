#pragma once
#include "engine/Vector.h"
#include "engine/Quad.h"

#include "Constants.h"
#include "Object.h"
#include "ViewEffect.h"

class RenderableObject : public Object {

public:
	RenderableObject(unsigned int category);
	//virtual ~RenderableObject();

	virtual void render();
	void setSize(const float x, const float y) override;
	void setSize(const Vector2f &size) override;

	void setDisabled(bool disabled);
	bool isDisabled();

private:
	Quad *m_quad;
	Shader *m_shader;
	std::unordered_map<std::string, unsigned int> m_sprites;
	bool m_disabled = false;
};