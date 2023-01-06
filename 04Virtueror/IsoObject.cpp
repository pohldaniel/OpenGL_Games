#include "IsoObject.h"
#include <iostream>

IsoObject::IsoObject(int rows, int cols) : mRows(rows), mCols(cols) {
	m_quad = new Quad();
	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("structures");
}

IsoObject::~IsoObject() {
}

void IsoObject::SetPosition(int x, int y, bool mapBuffer) {
	m_posX = x;
	m_posY = y;
	
	if(mapBuffer)
		m_quad->setPosition(Vector2f(x, y));
	else
		m_transform.translate(m_posX - m_quad->getPosition()[0], m_posY - m_quad->getPosition()[1], 0.0f);
}

void IsoObject::setSize(const Vector2f &size) {
	m_quad->setSize(size);
}

void IsoObject::setOrigin(const Vector2f &origin) {
	m_quad->setOrigin(origin);
}

void IsoObject::setPosition(const Vector2f &position) {
	m_quad->setPosition(position);
}

void IsoObject::SetPosition() {
	if (mLayer)
		mLayer->RepositionObject(this);
}

int IsoObject::GetX() const {
	return m_posX;
}

void IsoObject::SetX(int x) {
	m_posX = x;
}

int IsoObject::GetY() const {
	return m_posY;
}

void IsoObject::SetY(int y) {
	m_posY = y;
}

int IsoObject::GetWidth() const { return 0; }
int IsoObject::GetHeight() const { return m_quad->getSize()[1]; }


void IsoObject::SetAlpha(unsigned char alpha) {  }

void IsoObject::SetColor(unsigned int color) {  }

void IsoObject::Render(Matrix4f& transform) {
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", Globals::projection * transform * m_transform);
	m_shaderArray->loadInt("u_layer", m_currentFrame);
	m_quad->render(m_spriteSheet->getAtlas(), true);
	glUseProgram(0);
}

void IsoObject::OnPositionChanged() {}