//http://www.gotw.ca/gotw/039.htm
#pragma once

#include <engine/scene/Md2Node.h>
#include <Physics/Physics.h>
#include <Entities/Entity.h>

#define		MAP_MODEL_HEIGHT_Y	25.0f

class Md2Entity : public Md2Node, public Entity {

public:

	Md2Entity(const Md2Model& md2Model);
	virtual ~Md2Entity();

	virtual void draw() override;
	virtual void update(const float dt) override;
	
	short getMaterialIndex() const override;
	void setMaterialIndex(short index) override;
	short getTextureIndex() const override;
	void setTextureIndex(short index) override;
	void setIsActive(bool active);
	bool isActive();
	
	const Vector4f& getColor() const;
	void move(float x, float z);

private:

	bool m_isActive;
	Vector4f m_color;
};