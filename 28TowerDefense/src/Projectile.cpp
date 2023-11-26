#include <engine/Batchrenderer.h>
#include "Projectile.h"

TextureRect Projectile::Rect;
const float Projectile::speed = 10.0f, Projectile::size = 0.2f, Projectile::distanceTraveledMax = 20.0f;

Projectile::Projectile(const Vector2f& setPos, const Vector2f& setDirectionNormal) :pos(setPos), directionNormal(setDirectionNormal) { }

void Projectile::update(float dt) {
	//Move the projectile forward.
	float distanceMove = speed * dt;
	pos += directionNormal * distanceMove;

	distanceTraveled += distanceMove;
	if (distanceTraveled >= distanceTraveledMax)
		collisionOccurred = true;
}

void Projectile::drawBatched(float tileSize) {
	Batchrenderer::Get().addQuadAA(Vector4f(pos[0] * static_cast<float>(tileSize) - static_cast<float>(Rect.width / 2), pos[1] * static_cast<float>(tileSize) - static_cast<float>(Rect.height / 2), static_cast<float>(Rect.width), static_cast<float>(Rect.height)), Vector4f(Rect.textureOffsetX, Rect.textureOffsetY, Rect.textureWidth, Rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Rect.frame);
}

bool Projectile::getCollisionOccurred() {
	return collisionOccurred;
}

void Projectile::Init(const TextureRect& textureRect) {
	Rect = textureRect;
}