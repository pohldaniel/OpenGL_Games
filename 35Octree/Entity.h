#pragma once

#include <engine/Texture.h>
#include <engine/Camera.h>
#include <engine/Material.h>
#include <engine/scene/MeshNode.h>
#include <engine/AssimpModel.h>

class Entity : public MeshNode {

public:

	Entity(const AssimpModel& model);
	virtual ~Entity();

	virtual void draw();
	virtual void update(const float dt);

	virtual const Material& getMaterial() const;


	short getMaterialIndex() const;
	void setMaterialIndex(short index) const;

protected:

	mutable short m_materialIndex;
};