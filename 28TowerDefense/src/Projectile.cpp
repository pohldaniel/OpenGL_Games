#include <engine/Batchrenderer.h>
#include "Projectile.h"

TextureRect Projectile::Rect;
const float Projectile::speed = 10.0f, Projectile::size = 0.2f, Projectile::distanceTraveledMax = 20.0f;

Projectile::Projectile(const Vector2f& setPos, const Vector2f& setDirectionNormal) :pos(setPos), directionNormal(setDirectionNormal) { }

void Projectile::update(float dt, std::vector<std::shared_ptr<Unit>>& listUnits) {
	//Move the projectile forward.
	float distanceMove = speed * dt;
	pos += directionNormal * distanceMove;

	distanceTraveled += distanceMove;
	if (distanceTraveled >= distanceTraveledMax)
		collisionOccurred = true;

	checkCollisions(listUnits);
}

void Projectile::drawBatched(float tileSize) {
	Batchrenderer::Get().addQuadAA(Vector4f(pos[0] * static_cast<float>(tileSize) - static_cast<float>(Rect.width / 2), pos[1] * static_cast<float>(tileSize) - static_cast<float>(Rect.height / 2), static_cast<float>(Rect.width), static_cast<float>(Rect.height)), Vector4f(Rect.textureOffsetX, Rect.textureOffsetY, Rect.textureWidth, Rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Rect.frame);
}

bool Projectile::getCollisionOccurred() {
	return collisionOccurred;
}
void Projectile::checkCollisions(std::vector<std::shared_ptr<Unit>>& listUnits) {
	//Check for a collision with any of the units.
	if (collisionOccurred == false) {
		//Check if this overlaps any of the enemy units or not.
		for (int count = 0; count < listUnits.size() && collisionOccurred == false; count++) {
			auto& unitSelected = listUnits[count];
			if (unitSelected != nullptr && unitSelected->checkOverlap(pos, size)) {
				unitSelected->removeHealth(1);
				collisionOccurred = true;
			}
		}
	}
}

void Projectile::Init(const TextureRect& textureRect) {
	Rect = textureRect;
}