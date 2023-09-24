#pragma once

#include "Texture.h"
#include "Spritesheet.h"
#include "Framebuffer.h"
#include "Shader.h"

class CubeBuffer {

public:

	CubeBuffer(unsigned int internalFormat, int size);
	~CubeBuffer();

	void setFiltering(unsigned int minFilter);
	void setWrapMode(unsigned int mode);
	void setShader(Shader* shader);
	void draw();
	void drawRaw();
	void updateViewMatrix(unsigned int face);
	void updateAllViewMatrices();
	void attachTexture(unsigned int face);
	void attachLayerd();
	void unbind();
	void bindVao();
	void unbindVao();

	void resize(int size);
	void setInnerDrawFunction(std::function<void()> fun);
	void setDrawFunction(std::function<void()> fun);
	void setPosition(const Vector3f& position);

	const Matrix4f& getPerspectiveMatrix();
	const Matrix4f& getViewMatrix();
	const Matrix4f& getInvPerspectiveMatrix();
	const Matrix4f& getInvViewMatrix();
	const Vector3f& getViewDirection();
	const Vector3f& getPosition();
	const std::vector<Matrix4f>& getViewMatrices();

	int getSize();

	Framebuffer& getFramebuffer();
	Shader* getShader();
	unsigned int& getVao();
	unsigned int& getTexture();

private:

	void createBuffer();

	Shader* m_shader;
	Framebuffer m_fbo;

	unsigned int m_texture, m_depthTexture;
	unsigned int m_internalFormat;

	unsigned int m_minFilter;
	unsigned int m_magFilter;
	unsigned int m_mode;

	int m_size;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;

	Vector3f m_targetVectors[6];
	Vector3f m_upVectors[6];
	Vector3f m_position;
	Vector3f m_viewDirection;

	Matrix4f m_persMatrix;
	Matrix4f m_viewMatrix;
	Matrix4f m_invPersMatrix;
	Matrix4f m_invViewMatrix;

	float m_nearPlane = 1.0f;
	float m_farPlane = 100.0f;

	std::function<void()> m_innerDraw = 0;
	std::function<void()> m_draw = 0;

	std::vector<Matrix4f> m_viewMatrices;
};