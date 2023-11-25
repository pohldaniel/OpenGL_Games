#include <engine/Batchrenderer.h>
#include "TileSet.h"
#include "Unit.h"

const float Unit::speed = 5.0f;
const float Unit::size = 0.4f;
TextureRect Unit::Rect;

Unit::Unit(const Vector2f& setPos) : pos(setPos) {
}

void Unit::update(float dT, Level& level, std::vector<Unit>& listUnits) {
	//Determine the distance to the target from the unit's current position.
	float distanceToTarget = (level.getTargetPos() - pos).length();

	//Determine the distance to move this frame.
	float distanceMove = speed * dT;
	if (distanceMove > distanceToTarget)
		distanceMove = distanceToTarget;

	//Find the normal from the flow field.
	Vector2f directionNormal(level.getFlowNormal((int)pos[0], (int)pos[1]));
	Vector2f posAdd = directionNormal * distanceMove;

	//Check if the new position would overlap any other units or not.
	bool moveOk = true;
	for (int count = 0; count < listUnits.size() && moveOk; count++) {
		auto& unitSelected = listUnits[count];
		if (&unitSelected != this && unitSelected.checkOverlap(pos, size)) {
			//They overlap so check and see if this unit is moving towards or away from the unit it overlaps.
			Vector2f directionToOther = (unitSelected.pos - pos);
			//Ensure that they're not directly on top of each other.
			if (directionToOther.length() > 0.01f) {
				//Check the angle between the units positions and the direction that this unit is traveling.
				//Ensure that this unit isn't moving directly towards the other unit (by checking the angle between).
				Vector2f normalToOther(directionToOther.normalize());
				float angleBtw = abs(normalToOther.angleBetween(directionNormal));
				if (angleBtw < PI / 4.0f)
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

void Unit::drawBatched(int tileSize) {
	Batchrenderer::Get().addQuadAA(Vector4f((pos[0] - size *0.5f) * static_cast<float>(tileSize), (pos[1] - size *0.5f) * static_cast<float>(tileSize), size * tileSize, size* tileSize), Vector4f(Rect.textureOffsetX, Rect.textureOffsetY, Rect.textureWidth, Rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Rect.frame);
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

void Unit::Inti(const TextureRect& rect) {
	Rect = rect;
}