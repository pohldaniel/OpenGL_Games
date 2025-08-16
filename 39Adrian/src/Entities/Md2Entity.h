//http://www.gotw.ca/gotw/039.htm
#pragma once

#include <engine/scene/Md2Node.h>
#include <Entities/Entity.h>

class Md2Entity : public Md2Node, public Entity {

public:

	Md2Entity(const Md2Model& md2Model);
	virtual ~Md2Entity();

	virtual void draw() override;
	virtual void update(const float dt) override;
	virtual void fixedUpdate(float fdt);

	short getMaterialIndex() const override;
	void setMaterialIndex(short index) override;
	short getTextureIndex() const override;
	void setTextureIndex(short index) override;
	void setIsActive(bool active);
	bool isActive();
	
private:

	bool m_isActive;
};