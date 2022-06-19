#include "IsoObject.h"
#include <iostream>

IsoObject::IsoObject(int rows, int cols) : mRows(rows), mCols(cols) {
	m_quad = new Quad(m_position, m_size);
	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("structures");
}

IsoObject::~IsoObject() {
}

void IsoObject::SetPosition(int x, int y) {
	m_quad->setPosition(Vector2f(x, y));
}

void IsoObject::setSize(const Vector2f &size) {
	m_quad->setSize(size);
}

void IsoObject::setOrigin(const Vector2f &origin) {
	m_quad->setOrigin(origin);
}

void IsoObject::SetPosition() {
	if (mLayer)
		mLayer->RepositionObject(this);
}

int IsoObject::GetX() const { return 0; }

void IsoObject::SetX(int x) {
	//mImg->SetX(x);
	OnPositionChanged();
}

int IsoObject::GetY() const { return 0; }

void IsoObject::SetY(int y) {

}

int IsoObject::GetWidth() const { return 0; }
int IsoObject::GetHeight() const { return m_size[1]; }


void IsoObject::SetAlpha(unsigned char alpha) {  }

void IsoObject::SetColor(unsigned int color) {  }

void IsoObject::Render(Matrix4f& transform) {
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", transform * Globals::projection);
	m_shaderArray->loadInt("u_layer", m_currentFrame);
	m_quad->render(m_spriteSheet->getAtlas(), true);
	glUseProgram(0);
}

void IsoObject::OnPositionChanged() {}