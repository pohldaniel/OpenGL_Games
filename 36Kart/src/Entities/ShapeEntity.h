#pragma once

#include <engine/Texture.h>
#include <engine/Camera.h>
#include <engine/Material.h>
#include <engine/scene/ShapeNode.h>
#include <Entities/Entity.h>

class ShapeEntity : public ShapeNode, public Entity {

public:

	ShapeEntity(const Shape& shape);
	virtual ~ShapeEntity();

	virtual void draw() override;
};