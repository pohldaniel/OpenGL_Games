#include "IsoObject.h"
#include <iostream>

IsoObject::IsoObject(int rows, int cols) : mRows(rows), mCols(cols) {
	Vector2f m_size = Vector2f(288.0f, 163.0f);
	m_quad = new Quad(false, 0.0f, 1.0f, 0.0f, 1.0f, m_size[0], m_size[1], 0.0f, 0.0f, 1.0f, 1.0f, 0, 0);

	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_shader = Globals::shaderManager.getAssetPointer("quad");


	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("structures");
	m_texture = &Globals::textureManager.get("structures");
}

IsoObject::~IsoObject() {
}

void IsoObject::SetPosition(int x, int y) {
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
int IsoObject::GetHeight() const { return 0; }


void IsoObject::SetAlpha(unsigned char alpha) {  }

void IsoObject::SetColor(unsigned int color) {  }

void IsoObject::Render(Matrix4f& transform) {

	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", transform * Globals::projection);
	m_shaderArray->loadInt("u_layer", 1);
	m_quad->render(m_spriteSheet->getAtlas(), true);
	glUseProgram(0);
}

void IsoObject::OnPositionChanged() {}