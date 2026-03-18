#pragma once

#include "../Md2Model.h"
#include "../BoundingBox.h"
#include "OctreeNode.h"

class Md2Node : public OctreeNode {

public:

	Md2Node(const Md2Model& md2Model);

	virtual void drawRaw() const override;
	virtual void drawShadow() const override;
	virtual void update(const float dt);

	using OctreeNode::addChild;
	void addChild(Md2Node* node, bool drawDebug);
	const Md2Model& getMd2Model() const;
	void setAnimationType(AnimationType animationType, AnimationType animationTypeN = AnimationType::STAND);

	short getMaterialIndex() const;
	short getTextureIndex() const;	
	void setMaterialIndex(short index);
	void setTextureIndex(short index);
	void setShader(Shader* shader);
	const BoundingBox& getLocalBoundingBox() const;
	void setSpeed(float speed);
	void setLoopAnimation(bool loopAnimation);
	void setColor(const Vector4f& color);
	void setOnAnimationEnd(std::function<void()> fun);
	void setDisabled(bool disabled);

protected:

	short m_materialIndex;
	short m_textureIndex;

private:

	void OnWorldBoundingBoxUpdate() const;	
	void OnAnimationChanged();
	void updateAnimation();
	
	std::vector<Utils::MD2IO::Vertex> m_interpolated;
	bool m_animationDirty, m_disabled;

	const Utils::MD2IO::Animation* currentAnimation;
	AnimationType m_animationType;
	float m_speed;
	short m_activeFrameIdx;
	float m_activeFrame;
	Shader* m_shader;
	bool m_loopAnimation;
	Vector4f m_color;
	const Md2Model& md2Model;

	std::function<void()> OnAnimationEnd;
};
