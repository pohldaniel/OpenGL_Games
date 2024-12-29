#pragma once

#include <engine/BoundingBox.h>
#include "Utils/BinaryIO.h"

enum AnimationType {
	STAND = 0,
	RUN = 1,
	ATTACK = 2,
	DEATH_BACK = 3,
	DEATH_FORWARD = 4,
	DEATH_BACK_SLOW = 5,
	NONE
};

class Md2Model {
public:

	Md2Model();
	Md2Model(Md2Model const& rhs);
	Md2Model(Md2Model&& rhs);
	Md2Model& operator=(const Md2Model& rhs);
	Md2Model& operator=(Md2Model&& rhs);
	~Md2Model();

	void load(const char* path);
	void loadCpu(const char* path);
	void loadGpu();

	const void draw(short textureIndex = -1, short materialIndex = -1) const;
	const void update(float dt) const;
	const void updateBuffer(const std::vector<Utils::MD2IO::Vertex>& interpolated) const;

	const BoundingBox& getLocalBoundingBox() const;
	void setAnimationType(AnimationType animationType);
	const std::vector<Utils::MD2IO::Animation>& getAnimations() const;
	const unsigned int getNumVertices() const;

	void cleanup();
	void markForDelete();

private:

	std::vector<Utils::MD2IO::Animation> m_animations;
	mutable std::vector<Utils::MD2IO::Vertex> m_interpolated;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_texels;

	mutable short m_activeFrameIdx;
	mutable float m_activeFrame;

	Utils::MD2IO::Animation* currentAnimation;
	AnimationType m_animationType;
	float m_speed;
	unsigned int m_numVertices;

	unsigned int m_vao;
	unsigned int m_vbo[2];
	unsigned int m_ibo;
	
	bool m_markForDelete;

	static unsigned int StreamBufferCapacity;
};