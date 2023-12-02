#include <engine/Batchrenderer.h>

#include "Pin.h"
#include "Beam.h"
#include "UnitBridge.h"

TextureRect Pin::Rect;

Pin::Pin(const Vector2f& setPos, bool setPositionFixed) :pos(setPos), positionFixed(setPositionFixed) {

}

void Pin::update(float dT) {
	if (positionFixed == false && massSumStored > 0.0f) {
		//Calculate the acceleration, and update the velocity and position.
		Vector2f acc = forceSumStored / massSumStored;
		vel += acc * dT;
		pos += vel * dT;
	}
}

void Pin::drawBatched() {
	Batchrenderer::Get().addQuadAA(Vector4f(pos[0] - static_cast<float>(Rect.width / 2), pos[1] - static_cast<float>(Rect.height / 2), static_cast<float>(Rect.width), static_cast<float>(Rect.height)), Vector4f(Rect.textureOffsetX, Rect.textureOffsetY, Rect.textureWidth, Rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Rect.frame);
}

void Pin::calculateForces(std::vector<std::shared_ptr<Beam>>& listBeams, UnitBridge& unitRedCreature) {
	//Calculate all the forces and mass, in any beams that are attached to this and add them up.
	//Also add the force of gravity from the unit if it's touching the beam.
	forceSumStored = Vector2f();
	massSumStored = 0.0f;

	for (auto beamSelected : listBeams)
		if (beamSelected != nullptr && beamSelected->checkIfconnectedToPin(*this)) {
			beamSelected->addForceAndMassDiv2AtPin(*this, forceSumStored, massSumStored);

			if (unitRedCreature.checkIfTouchingBeam(beamSelected))
				unitRedCreature.addForceAtPin(forceSumStored);
		}
}



const Vector2f& Pin::getPos() {
	return pos;
}

void Pin::Init(const TextureRect& rect) {
	Rect = rect;
}