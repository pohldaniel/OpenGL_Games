#pragma once

#include <engine/Camera.h>
#include <Scene/Object.h>

class Skybox : public Object {

public:
	Skybox();

	void draw(const Camera& camera);

	short getTextureIndex() const;
	void setTextureIndex(short index) const;

private:

	Matrix4f transform;
	mutable short m_textureIndex;
};