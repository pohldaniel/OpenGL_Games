#pragma once

#include <engine/Texture.h>
#include <engine/Camera.h>
#include <engine/Material.h>
#include <engine/scene/ShapeNode.h>

class ShapeEntity : public ShapeNode {

public:

	ShapeEntity(const Shape& shape);
	virtual ~ShapeEntity();

	virtual void draw();
	virtual void update(const float dt);

	virtual const Material& getMaterial() const;

	short getMaterialIndex() const;
	void setMaterialIndex(short index) const;
	short getTextureIndex() const;
	void setTextureIndex(short index) const;

protected:

	mutable short m_materialIndex;
	mutable short m_textureIndex;
};