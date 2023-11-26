#include <engine/Batchrenderer.h>
#include "Turret.h"

TextureRect Turret::Rect;
TextureRect Turret::ShadowRect;

const float Turret::speedAngular = MathAddon::angleDegToRad(180.0f);
const float Turret::weaponRange = 5.0f;

Turret::Turret(const Vector2f& setPos) :
	pos(setPos), angle(0.0f), timerWeapon(1.0f) {
	
}

void Turret::update(float dt, std::vector<std::shared_ptr<Unit>>& listUnits, std::vector<Projectile>& listProjectiles) {
	//Update timer.
	timerWeapon.countDown(dt);

	//Check if a target has been found but is no longer alive or is out of weapon range.
	if (auto unitTargetSP = unitTarget.lock()) {
		if (unitTargetSP->getIsAlive() == false ||
			(unitTargetSP->getPos() - pos).length() > weaponRange) {
			//Then reset it.
			unitTarget.reset();
		}
	}

	//Find a target if needed.
	if (unitTarget.expired())
		unitTarget = findEnemyUnit(listUnits);

	//Update the angle and shoot a projectile if needed.
	if (updateAngle(dt))
		shootProjectile(listProjectiles);
}

bool Turret::updateAngle(float dT) {
	//Rotate towards the target unit if needed and output if it's pointing towards it or not.
	if (auto unitTargetSP = unitTarget.lock()) {
		//Determine the direction normal to the target.
		Vector2f directionNormalTarget = (unitTargetSP->getPos() - pos).normalize();

		//Determine the angle to the target.
		float angleToTarget = directionNormalTarget.angleBetween(Vector2f(cosf(angle), sinf(angle)));

		//Update the angle as required.
		//Determine the angle to move this frame.
		float angleMove = -copysign(speedAngular * dT, angleToTarget);
		if (abs(angleMove) > abs(angleToTarget)) {
			//It will point directly at it's target this frame.
			angle = directionNormalTarget.angle();
			return true;
		}
		else {
			//It won't reach it's target this frame.
			angle += angleMove;
		}
	}

	return false;
}


void Turret::shootProjectile(std::vector<Projectile>& listProjectiles) {
	//Shoot a projectile towards the target unit if the weapon timer is ready.
	if (timerWeapon.timeSIsZero()) {
		listProjectiles.push_back(Projectile(pos, Vector2f(cosf(angle), sinf(angle))));

		timerWeapon.resetToMax();
	}
}

void Turret::drawBatched(float tileSize) {
	drawTextureWithOffset(ShadowRect, tileSize, 5.0f);
	drawTextureWithOffset(Rect, tileSize, 0.0f);
}

void Turret::drawTextureWithOffset(const TextureRect& rect, float tileSize, float offset) {
	Batchrenderer::Get().addRotatedQuadRH(Vector4f(pos[0] * static_cast<float>(tileSize) - static_cast<float>(rect.width / 2) + offset, pos[1] * static_cast<float>(tileSize) - static_cast<float>(rect.height / 2) - offset, static_cast<float>(rect.width), static_cast<float>(rect.height)),
		MathAddon::angleRadToDeg(angle),
		static_cast<float>(rect.width / 2),
		static_cast<float>(rect.height / 2),
		Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight),
		Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
}

bool Turret::checkIfOnTile(int x, int y) {
	return ((int)pos[0] == x && (int)pos[1] == y);
}

std::weak_ptr<Unit> Turret::findEnemyUnit(std::vector<std::shared_ptr<Unit>>& listUnits) {
	//Find the closest enemy unit to this turret.
	std::weak_ptr<Unit> closestUnit;
	float distanceClosest = 0.0f;

	//Loop through the entire list of units.
	for (auto& unitSelected : listUnits) {
		//Ensure that the selected unit exists.
		if (unitSelected != nullptr) {
			//Calculate the distance to the selected unit.
			float distanceCurrent = (pos - unitSelected->getPos()).length();
			//Check if the unit is within range, and no closest unit has been found or the 
			//selected unit is closer than the previous closest unit.
			if (distanceCurrent <= weaponRange &&
				(closestUnit.expired() || distanceCurrent < distanceClosest)) {
				//Then set the closest unit to the selected unit.
				closestUnit = unitSelected;
				distanceClosest = distanceCurrent;
			}
		}
	}

	return closestUnit;
}

void Turret::Init(std::vector<TextureRect>& textureRects) {
	Rect = textureRects[0];
	ShadowRect = textureRects[1];
}