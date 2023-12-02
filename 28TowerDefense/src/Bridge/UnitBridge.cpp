#include <engine/Batchrenderer.h>
#include "MathAddon.h"
#include "UnitBridge.h"
#include "Application.h"

TextureRect UnitBridge::Rect;
const float UnitBridge::speedX = 100.0f, UnitBridge::size = 64.0f, UnitBridge::mass = 14.5f;

UnitBridge::UnitBridge(const Vector2f& setPos) : pos(setPos), posStart(setPos) {

}

void UnitBridge::update(float dT, std::vector<std::shared_ptr<Beam>>& listBeams) {
	//Calculate the acceleration, update the speed, and determine the desired position
	//if nothing is blocking this unit.

	//Note that the speed is split into x and y components because speedX is fixed.
	speedY += MathAddon::accGravity * dT;
	Vector2f posDesired = pos + Vector2f(speedX * dT, speedY * dT);

	//If the unit was at posDesired, check if it would overlap anything.  If it does
	//then adjust it slightly to be closer to the top of the screen and check it again.
	//Keep iterating until it doesn't overlap anything or a maximum number of iterations 
	//(distance) is reached.
	bool posDesiredOk = false;
	for (int count = 0; count < 30 && posDesiredOk == false; count++) {
		if (checkOverlapWithBridgeAndLand(posDesired, listBeams, (count == 0)) == false)
			posDesiredOk = true;
		else {
			posDesired[1] += 0.1f;
			//Reset it's speed in the y direction because it's overlapping something that stopped it.
			speedY = 0.0f;
		}
	}

	//Set the desired position to the current position if it's ok.
	if (posDesiredOk)
		pos = posDesired;

	//Check if it reached the end of the level
	if (pos[0] > (960.0f - 64.0f) && pos[1] < static_cast<float>(Application::Height) - 376.0f)
		reachedEndOfLevel = true;

	//Check if it got stuck.  Either because it wasn't able to move or is below the screen.
	if (posDesiredOk == false || pos[1] < -size / 2.0f)
		isStuck = true;

}



void UnitBridge::drawBatched() {
	Batchrenderer::Get().addQuadAA(Vector4f(pos[0] - static_cast<float>(Rect.width / 2), pos[1] - static_cast<float>(Rect.height / 2), static_cast<float>(Rect.width), static_cast<float>(Rect.height)), Vector4f(Rect.textureOffsetX, Rect.textureOffsetY, Rect.textureWidth, Rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Rect.frame);	
}

void UnitBridge::reset() {
	//Reset the unit to it's state at the beginning of the game.
	pos = posStart;
	speedY = 0.0f;
	isStuck = false;
	reachedEndOfLevel = false;
}



bool UnitBridge::checkIfTouchingBeam(std::shared_ptr<Beam> beam) {
	for (auto& beamSelected : listBeamsTouching)
		if (auto beamSelectedSP = beamSelected.lock())
			if (beamSelectedSP.get() == beam.get())
				return true;

	return false;
}

void UnitBridge::addForceAtPin(Vector2f& forceSum) {
	//Determine how many pins the unit's force of gravity will be distributed over 
	//and output the total force divided by that amount.
	int pinCount = (int)listBeamsTouching.size() * 2;
	if (pinCount > 0) {
		float massAtPin = mass / pinCount;
		forceSum += Vector2f(0.0f, massAtPin * MathAddon::accGravity);
	}
}

bool UnitBridge::getIsStuck() {
	return isStuck;
}


bool UnitBridge::getReachedEndOfLevel() {
	return reachedEndOfLevel;
}

bool UnitBridge::checkOverlapWithBridgeAndLand(const Vector2f& posCheck, std::vector<std::shared_ptr<Beam>>& listBeams, bool isFirstIteration) {
	bool overlapFound = false;

	//If this is the first iteration then clear listBeamsTouching.
	if (isFirstIteration)
		listBeamsTouching.clear();

	//Loop through all the beams and see which ones overlap with this unit.
	for (auto& beamSelected : listBeams) {
		if (beamSelected != nullptr && beamSelected->checkOverlapWithUnit(posCheck, size / 2.0f)) {
			overlapFound = true;
			//If this is the first iteration then store the beam on listBeamsTouching.
			//This is done on the first iteration because that's when the possible new 
			//position being checked is going to overlap the most beams.
			if (isFirstIteration)
				listBeamsTouching.push_back(beamSelected);
		}
	}


	//Check overlap with land
	if (overlapFound == false) {
		//Left piece
		Vector2f posLeft1(0.0f, static_cast<float>(Application::Height) - 387.0f);
		Vector2f posLeft2(264.0f, static_cast<float>(Application::Height) - 387.0f);
		if (MathAddon::checkOverlapLineCircle(posLeft1, posLeft2, posCheck, (size / 2.0f - 11.0f)))
			overlapFound = true;

		if (overlapFound == false) {
			//Right piece
			Vector2f posRight1(696.0f, static_cast<float>(Application::Height) - 387.0f);
			Vector2f posRight2(960.0f, static_cast<float>(Application::Height) - 387.0f);
			if (MathAddon::checkOverlapLineCircle(posRight1, posRight2, posCheck, (size / 2.0f - 11.0f)))
				overlapFound = true;
		}
	}

	return overlapFound;
}

void UnitBridge::Init(const TextureRect& rect) {
	Rect = rect;
}