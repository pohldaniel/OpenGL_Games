#pragma once
#include <engine/Material.h>
#include <engine/Shader.h>

class Entity {

public:

	Entity();
	virtual ~Entity() = default;

	virtual void draw();
	virtual void update(const float dt);

	const Material& getMaterial() const;
	void setShader(Shader* shader);

	virtual short getMaterialIndex() const;
	virtual void setMaterialIndex(short index);
	virtual short getTextureIndex() const;
	virtual void setTextureIndex(short index);
	
protected:

	short m_materialIndex;
	short m_textureIndex;
	Shader* shader;
};