#pragma once
#include <engine/Material.h>
#include <engine/Shader.h>

class Entity {

public:

	Entity();
	virtual ~Entity() = default;

	virtual void draw() = 0;
	virtual void update(const float dt);

	const Material& getMaterial() const;
	short getMaterialIndex() const;
	void setMaterialIndex(short index) const;
	short getTextureIndex() const;
	void setTextureIndex(short index) const;
	void setShader(Shader* shader);

protected:

	mutable short m_materialIndex;
	mutable short m_textureIndex;
	Shader* shader;
};