#include <engine/Batchrenderer.h>
#include "Turret.h"

TextureRect Turret::Rect;
TextureRect Turret::ShadowRect;

const float Turret::speedAngular = MathAddon::angleDegToRad(180.0f);

Turret::Turret(const Vector2f& setPos) :
	pos(setPos), angle(0.0f) {
	
}

void Turret::update(float dT) {
	angle += speedAngular * dT;
	if (angle > MathAddon::angleDegToRad(360.0f))
		angle -= MathAddon::angleDegToRad(360.0f);
}

void Turret::drawBatched(float tileSize) {
	drawTextureWithOffset(ShadowRect, tileSize, 5.0f);
	drawTextureWithOffset(Rect, tileSize, 0.0f);
}

void Turret::drawTextureWithOffset(const TextureRect& rect, float tileSize, float offset) {
	Batchrenderer::Get().addRotatedQuadLH(Vector4f(pos[0] * static_cast<float>(tileSize) - static_cast<float>(rect.width / 2) + offset, pos[1] * static_cast<float>(tileSize) - static_cast<float>(rect.height / 2) - offset, static_cast<float>(rect.width), static_cast<float>(rect.height)),
		MathAddon::angleRadToDeg(angle),
		static_cast<float>(rect.width / 2),
		static_cast<float>(rect.height / 2),
		Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight),
		Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
}

bool Turret::checkIfOnTile(int x, int y) {
	return ((int)pos[0] == x && (int)pos[1] == y);
}

void Turret::Init(std::vector<TextureRect>& textureRects) {
	Rect = textureRects[0];
	ShadowRect = textureRects[1];
}