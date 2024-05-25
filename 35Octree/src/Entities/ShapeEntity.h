#pragma once

#include <engine/scene/ShapeNode.h>
#include <engine/Texture.h>
#include <engine/Material.h>
#include <engine/Camera.h>
#include <engine/Shader.h>

class ShapeEntity : public ShapeNode {

public:

	ShapeEntity(const Shape& shape, const Camera& camera);
	virtual ~ShapeEntity();

	virtual void drawRaw() const override;

	virtual const Material& getMaterial() const;
	virtual const Texture& getTexture() const;

	short getMaterialIndex() const;
	void setMaterialIndex(short index) const;

	short getTextureIndex() const;
	void setTextureIndex(short index) const;
	void setShader(Shader* shader);

private:

	mutable short m_materialIndex;
	mutable short m_textureIndex;
	const Camera& camera;
	Shader* shader;
};