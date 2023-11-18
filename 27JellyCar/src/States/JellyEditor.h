#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <States/StateMachine.h>

#include "Car/Car.h"

#include "JellyPhysics/World.h"
#include "JellyPhysics/JellyPhysics.h"
#include "Levels/StaticBody.h"
#include "Levels/FallingBody.h"
#include "Levels/GamePressureBody.h"
#include "Levels/GameSpringBody.h"
#include "Levels/LevelSoftBody.h"

class JellyEditor : public State, public MouseEventListener, public KeyboardEventListener {

public:

	JellyEditor(StateMachine& machine);
	~JellyEditor();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

	static void RenderGlobalShapeLine(std::vector<PointMass> &mPointMasses, float R, float G, float B);
	static void RenderLine(Vector2 start, Vector2 stop, float R, float G, float B);

private:

	void OnKeyDown(Event::KeyboardEvent& event) override;
	void processInput();
	Vector2f touchToScreen(Vector4f screenBound, Vector2f touch);

	unsigned int m_backWidth, m_backHeight;
	unsigned int m_columns, m_rows;

	std::vector<LevelSoftBody*> m_gameBodies;
	World* m_world;
	Car* m_car;
	Matrix4f m_jellyProjection;
	Vector2 m_levelTarget;

	//ground
	StaticBody *groundBody;

	//falling objects
	ClosedShape fallingShape;
	std::vector<FallingBody *> fallingBodies;
	bool fall = true;

	//pressure objects
	ClosedShape pressureShape;
	std::vector<GamePressureBody *> pressureBodies;
	bool pressure = true;

	//spring bodies
	ClosedShape springShape;
	std::vector<GameSpringBody *> springBodies;
	bool spring = true;

	//dragging
	float dragX, dragY;
	bool touchF = false;
	int dragPoint = -1;
	Body *dragBody;

	Vector4f m_screenBounds;
};