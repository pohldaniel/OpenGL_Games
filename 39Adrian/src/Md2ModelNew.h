#pragma once

#include <engine/BoundingBox.h>
#include "Utils/BinaryIO.h"

enum AnimationType {
	_STAND = 0,
	_RUN = 1,
	_ATTACK = 2,
	_DEATH_BACK = 3,
	_DEATH_FORWARD = 4,
	_DEATH_BACK_SLOW = 5,
	_NONE
};

class Md2Model {
public:

	Md2Model();
	~Md2Model();

	void load(const char* path);

	const void draw(short textureIndex = -1, short materialIndex = -1) const;
	const void update(float dt);

	const BoundingBox& getLocalBoundingBox() const;
	void setAnimationType(AnimationType animationType);

private:

	std::vector<Utils::MD2IO::Animation> m_animation;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<Utils::MD2IO::Vertex> m_interpolated;

	short m_activeFrameIdx;
	Utils::MD2IO::Animation* currentAnimation;
	float m_activeFrame;
	float m_speed;
	AnimationType m_animationType;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	static unsigned int StreamBufferCapacity;
};