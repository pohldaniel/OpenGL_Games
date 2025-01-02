//http://www.gotw.ca/gotw/039.htm
#pragma once

#include <engine/scene/Md2Node.h>
#include <Physics/Physics.h>
#include <Entities/Entity.h>

class Md2Entity : public Md2Node, public Entity {

public:

	Md2Entity(const Md2Model& md2Model);
	virtual ~Md2Entity();

	virtual void draw() override;
	virtual void update(const float dt) override;
	void drawRaw() const override;
	void fixedUpdate(float fdt);

	short getMaterialIndex() const override;
	void setMaterialIndex(short index) override;
	short getTextureIndex() const override;
	void setTextureIndex(short index) override;
	void setIsActive(bool active);
	
	const Vector4f& getColor() const;

	btRigidBody* m_rigidBody;

private:
	bool m_isActive;
	Vector4f m_color;
};