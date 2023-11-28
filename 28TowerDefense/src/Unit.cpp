#include <engine/Batchrenderer.h>
#include "TileSet.h"
#include "Unit.h"

const float Unit::speed = 1.5f;
const float Unit::size = 0.48f;
TextureRect Unit::Rect;

Unit::Unit(const Vector2f& setPos) : pos(setPos), timerJustHurt(0.25f) {
}

void Unit::update(float dT, Level& level, std::vector<std::shared_ptr<Unit>>& listUnits) {
	timerJustHurt.countDown(dT);

	//Determine the distance to the target from the unit's current position.
	float distanceToTarget = (level.getTargetPos() - pos).length();

	if (distanceToTarget < 0.5f) {
		healthCurrent = 0;
	}else {
		//Determine the distance to move this frame.
		float distanceMove = speed * dT;
		if (distanceMove > distanceToTarget)
			distanceMove = distanceToTarget;

		//Find the normal from the flow field.
		Vector2f directionNormal(level.getFlowNormal((int)pos[0], (int)pos[1]));
		//If this reached the target tile, then modify directionNormal to point to the target tile.
		if ((int)pos[0] == (int)level.getTargetPos()[0] && (int)pos[1] == (int)level.getTargetPos()[1])
			directionNormal = (level.getTargetPos() - pos).normalize();

		Vector2f posAdd = directionNormal * distanceMove;

		//Check if the new position would overlap any other units or not.
		bool moveOk = true;
		for (int count = 0; count < listUnits.size() && moveOk; count++) {
			auto& unitSelected = listUnits[count];
			if (unitSelected != nullptr && unitSelected.get() != this &&
				unitSelected->checkOverlap(pos, size)) {
				//They overlap so check and see if this unit is moving towards or away from the unit it overlaps.
				Vector2f directionToOther = (unitSelected->pos - pos);
				//Ensure that they're not directly on top of each other.
				if (directionToOther.length() > 0.01f) {
					//Check the angle between the units positions and the direction that this unit is traveling.
					//Ensure that this unit isn't moving directly towards the other unit (by checking the angle between).
					Vector2f normalToOther(directionToOther.normalize());
					float angleBtw = abs(normalToOther.angleBetween(directionNormal));
					if (angleBtw < 3.14159265359f / 4.0f)
						//Don't allow the move.
						moveOk = false;
				}
			}
		}

		if (moveOk) {
			//Check if it needs to move in the x direction.  If so then check if the new x position, plus an amount of spacing 
			//(to keep from moving too close to the wall) is within a wall or not and update the position as required.
			const float spacing = 0.35f;
			int x = (int)(pos[0] + posAdd[0] + copysign(spacing, posAdd[0]));
			int y = (int)(pos[1]);
			if (posAdd[0] != 0.0f && level.isTileWall(x, y) == false)
				pos[0] += posAdd[0];

			//Do the same for the y direction.
			x = (int)(pos[0]);
			y = (int)(pos[1] + posAdd[1] + copysign(spacing, posAdd[1]));
			if (posAdd[1] != 0.0f && level.isTileWall(x, y) == false)
				pos[1] += posAdd[1];
		}
	}
}

void Unit::drawBatched(float tileSize) {
	Batchrenderer::Get().addQuadAA(Vector4f(pos[0] * static_cast<float>(tileSize) - static_cast<float>(Rect.width / 2), pos[1] * static_cast<float>(tileSize) - static_cast<float>(Rect.height / 2), static_cast<float>(Rect.width), static_cast<float>(Rect.height)), Vector4f(Rect.textureOffsetX, Rect.textureOffsetY, Rect.textureWidth, Rect.textureHeight), !timerJustHurt.timeSIsZero() ? Vector4f(1.0f, 0.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Rect.frame);
}

bool Unit::checkOverlap(const Vector2f& posOther, float sizeOther) {
	return (posOther - pos).length() <= (sizeOther + size) / 2.0f;
}

Vector2f Unit::computeNormalSeparation(std::vector<Unit>& listUnits) {
	Vector2f output;

	const float radiusSeparation = size;// 1.0f;
	for (auto& unitSelected : listUnits) {
		if (&unitSelected != this) {
			//Check if the selected unit is within radiusSeparation.
			Vector2f directionToOther = (unitSelected.pos - pos);
			float distanceToOther = directionToOther.length();

			if (distanceToOther <= radiusSeparation) {
				//Ensure that they're not directly on top of each other.
				if (directionToOther.length() > 0.01f) {
					//Add the direction normal from the other unit to this unit, to the output.
					Vector2f normalToThis(directionToOther.normalize() * -1.0f);
					output += normalToThis;
				}
			}
		}
	}
	output.normalize();

	return output;
}

bool Unit::isAlive() {
	return (healthCurrent > 0);
}



Vector2f Unit::getPos() {
	return pos;
}

void Unit::removeHealth(int damage) {
	if (damage > 0) {
		healthCurrent -= damage;
		if (healthCurrent < 0)
			healthCurrent = 0;

		timerJustHurt.resetToMax();
	}
}

void Unit::Init(const TextureRect& rect) {
	Rect = rect;
}