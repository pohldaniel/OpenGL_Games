#include <engine/Batchrenderer.h>
#include "MathAddon.h"
#include "Beam.h"

std::vector<TextureRect> Beam::TextureRects;
const float Beam::massPerLength = 0.01f, Beam::kStiffness = 1000.0f, Beam::forceBeamMax = 2000.0f;

Beam::Beam(std::shared_ptr<Pin>& setPinConnection1,std::shared_ptr<Pin>& setPinConnection2, bool setIsRoad) :
	pinConnection1(setPinConnection1), pinConnection2(setPinConnection2),
	isRoad(setIsRoad) {
	//Setup the initial length of the beam and it's mass.
	if (pinConnection1 != nullptr && pinConnection2 != nullptr) {
		length0 = (pinConnection1->getPos() - pinConnection2->getPos()).length();
		mass = massPerLength * length0;
	}
}

void Beam::drawBatched() {
	if (pinConnection1 != nullptr && pinConnection2 != nullptr) {
		//Set the draw color based on the force in the beam.
		float color = 1.0f - colorForceFactor;
		//Calculate the position and dimensions that the image will be drawn at.
		Vector2f posCenter = (pinConnection1->getPos() + pinConnection2->getPos()) / 2.0f;
		Vector2f lengthCurrent = pinConnection1->getPos() - pinConnection2->getPos();
		float angleDeg = MathAddon::angleRadToDeg(lengthCurrent.angle());

		const TextureRect& rect = isRoad ? TextureRects[0] : TextureRects[1];

		Batchrenderer::Get().addRotatedQuadRH(Vector4f(posCenter[0] - lengthCurrent.length() / 2.0f, posCenter[1] - thickness / 2.0f, lengthCurrent.length(), thickness),
			angleDeg,
			static_cast<float>(lengthCurrent.length() / 2),
			static_cast<float>(thickness / 2),
			Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight),
			Vector4f(1.0f, color, color, 1.0f), rect.frame);
	}
}

bool Beam::getIsBroken() {
	return isBroken;
}

bool Beam::checkIfconnectedToPin(Pin& pin) {
	return ((pinConnection1 != nullptr && pinConnection1.get() == &pin) ||
		(pinConnection2 != nullptr && pinConnection2.get() == &pin));
}

void Beam::addForceAndMassDiv2AtPin(const Pin& pin, Vector2f& forceSum, float& massSum) {
	forceSum += getForceAtPin(pin);
	massSum += mass / 2.0f;
}

bool Beam::checkOverlapWithUnit(const Vector2f& posUnit, float radiusUnit) {
	if (isRoad && pinConnection1 != nullptr && pinConnection2 != nullptr)
		return MathAddon::checkOverlapLineCircle(pinConnection1->getPos(), pinConnection2->getPos(),
			posUnit, (radiusUnit + thickness / 2.0f));

	return false;
}

Vector2f Beam::getForceAtPin(const Pin& pin) {
	if (pinConnection1 != nullptr && pinConnection2 != nullptr && length0 > 0.0f) {
		//Determine the current length and displacement of the beam.
		Vector2f lengthCurrent = pinConnection1->getPos() - pinConnection2->getPos();
		Vector2f displacement = Vector2f(lengthCurrent).normalize() * (lengthCurrent.length() - length0);

		//Determine the forces.
		Vector2f forceBeam = displacement * kStiffness;
		Vector2f forceGravity = Vector2f(0.0f, mass * MathAddon::accGravity);

		//Determine how red the beam should be drawn and check if the forces are high enough to break it.
		if (forceBeamMax > 0.0f) {
			colorForceFactor = forceBeam.length() / forceBeamMax;
			if (colorForceFactor >= 1.0f) {
				colorForceFactor = 1.0f;
				isBroken = true;
			}
		}

		//Output the forces.
		if (pinConnection1.get() == &pin)
			return (forceBeam * -1.0f) + forceGravity / 2.0f;
		else if (pinConnection2.get() == &pin)
			return forceBeam + forceGravity / 2.0f;
	}

	return Vector2f();
}

void Beam::Init(std::vector<TextureRect>& textureRects) {
	TextureRects = textureRects;
}