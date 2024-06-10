#pragma once

#include <engine/Texture.h>
#include <engine/Camera.h>
#include <engine/Material.h>
#include <engine/scene/MeshNode.h>
#include <engine/AssimpModel.h>
#include <Entities/Entity.h>

class MeshEntity : public MeshNode, public Entity {

public:

	MeshEntity(const AssimpModel& model);
	virtual ~MeshEntity();

	virtual void draw() override;
};