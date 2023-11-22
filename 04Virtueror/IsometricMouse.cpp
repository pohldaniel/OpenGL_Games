#include "IsometricMouse.h"

IsometricMouse::IsometricMouse() {

	m_quad = new Quad(m_position, m_size);
	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("tiles");
}

IsometricMouse::~IsometricMouse() {}

void IsometricMouse::Render(float x, float y) {
	const int rendW = 1600;
	const int rendH = 900;

	float pointX = x * (float)(TILE_WIDTH) * 0.5f;
	float pointY = y * (float)(TILE_WIDTH) * 0.5f;
	float pointXTrans = (pointX - pointY);
	float pointYTrans = (pointX + pointY) * 0.5f;

	m_transform.translate(x + (rendW * 0.5f), y + (rendH *  0.5f), 0.0f);

	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", Globals::projection * m_transform);
	m_shaderArray->loadInt("u_layer", 13);
	m_quad->render(m_spriteSheet->getAtlas(), true);
	glUseProgram(0);
	
}