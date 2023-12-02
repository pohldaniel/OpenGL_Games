#pragma once
#include <engine/Vector.h>
#include <engine/Rect.h>
#include "Pin.h"

class Beam {

public:

	Beam(std::shared_ptr<Pin>& setPinConnection1, std::shared_ptr<Pin>& setPinConnection2, bool setIsRoad = false);
	void drawBatched();
	bool getIsBroken();
	bool checkIfconnectedToPin(Pin& pin);
	void addForceAndMassDiv2AtPin(const Pin& pin, Vector2f& forceSum, float& massSum);
	bool checkOverlapWithUnit(const Vector2f& posUnit, float radiusUnit);

	static void Init(std::vector<TextureRect>& textureRects);

private:
	Vector2f getForceAtPin(const Pin& pin);


	const static float massPerLength, kStiffness, forceBeamMax;
	const static int thickness = 22;

	float length0 = 0.0f;
	float mass = 0.0f;

	bool isRoad;

	std::shared_ptr<Pin> pinConnection1 = nullptr, pinConnection2 = nullptr;

	float colorForceFactor = 0.0f;
	bool isBroken = false;

	static std::vector<TextureRect> TextureRects;
};