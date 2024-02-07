#pragma once

#include <glm/gtc/quaternion.hpp>
#include <engine/Texture.h>
#include <engine/Camera.h>
#include <engine/AssimpModel.h>
#include <engine/Material.h>
#include <Scene/MeshNode.h>
#include <Scene/SequenceNode.h>
#include "Rigidbody.h"

class Entity : public SequenceNode {

public:

	Entity(const MeshSequence& meshSequence, int meshIndex);
	virtual ~Entity() = default;

	virtual void start();
	virtual void draw();
	virtual void update(const float dt);
	virtual void fixedUpdate(float fdt);

	virtual const Material& getMaterial() const;

	void markForDelete();
	bool isMarkForDelete();
	void setRigidbody(const Rigidbody& rigidbody);
	void setIsStatic(bool isStatic);

	short getMaterialIndex() const;
	virtual void setMaterialIndex(short index) const;
	short getTextureIndex() const;
	void setTextureIndex(short index) const;
	
protected:

	Rigidbody m_rigidbody;
	mutable short m_materialIndex;
	mutable short m_textureIndex;

private:

	bool m_disabled;
	bool m_isStatic;
	bool m_markForDelete;	
};