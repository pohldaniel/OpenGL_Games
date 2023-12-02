#include <engine/Batchrenderer.h>
#include "TileSet.h"
#include "UnitFlow.h"

const float UnitFlow::speed = 5.0f;
const float UnitFlow::size = 0.4f;
TextureRect UnitFlow::Rect;

UnitFlow::UnitFlow(const Vector2f& setPos) : pos(setPos), posDraw(pos) {
}

void UnitFlow::update(float dT, LevelFlow& level, std::vector<UnitFlow>& listUnits) {
	//Determine the distance to the target from the unit's current position.
	float distanceToTarget = (level.getTargetPos() - pos).length();

	//Determine the distance to move this frame.
	float distanceMove = speed * dT;
	if (distanceMove > distanceToTarget)
		distanceMove = distanceToTarget;

	//Find the normal by combining the normal from the flow field and seperation.
	Vector2f directionNormalFlowField(level.getFlowNormal((int)pos[0], (int)pos[1]));
	Vector2f directionNormalSeparation(computeNormalSeparation(listUnits));

	Vector2f directionNormalCombined = directionNormalFlowField + directionNormalSeparation * 5.0f;
	directionNormalCombined.normalize();

	Vector2f posAdd = directionNormalCombined * distanceMove;


	//Check if the new position would overlap any other units or not.
	bool moveOk = true;
	for (int count = 0; count < listUnits.size() && moveOk; count++) {
		auto& unitSelected = listUnits[count];
		if (&unitSelected != this && unitSelected.checkOverlap(pos, size)) {
			//They overlap so check and see if this unit is moving towards or away from the unit 
			//it overlaps.
			Vector2f directionToOther = (unitSelected.pos - pos);
			//Ensure that they're not directly on top of each other.
			if (directionToOther.length() > 0.01f) {
				//Check the angle between the units positions and the direction that this unit 
				//is traveling.  Ensure that this unit isn't moving directly towards the other 
				//unit (by checking the angle between).
				Vector2f normalToOther(directionToOther.normalize());
				float angleBtw = abs(normalToOther.angleBetween(directionNormalCombined));
				if (angleBtw < PI / 4.0f)
					//Don't allow the move.
					moveOk = false;
			}
		}
	}

	if (moveOk) {
		//Check if it needs to move in the x direction.  If so then check if the new x position, 
		//plus an amount of spacing (to keep from moving too close to the wall) is within a wall 
		//or not and update the position as required.
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

	const float fKeep = 0.93f;
	posDraw = posDraw * fKeep + pos * (1.0f - fKeep);
}

void UnitFlow::drawBatched(float tileSize) {
	Batchrenderer::Get().addQuadAA(Vector4f((posDraw[0] - size *0.5f) * tileSize, (posDraw[1] - size *0.5f) * tileSize, size * tileSize, size* tileSize), Vector4f(Rect.textureOffsetX, Rect.textureOffsetY, Rect.textureWidth, Rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Rect.frame);
}

bool UnitFlow::checkOverlap(const Vector2f& posOther, float sizeOther) {
	return (posOther - pos).length() <= (sizeOther + size) / 2.0f;
}

Vector2f UnitFlow::computeNormalSeparation(std::vector<UnitFlow>& listUnits) {
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

void UnitFlow::Inti(const TextureRect& rect) {
	Rect = rect;
}