#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Framebuffer.h>
#include <States/StateMachine.h>

#include "Car/Car.h"
#include "Levels/LevelSoftBody.h"
#include "Levels/LevelManager.h"
#include "JellyPhysics/World.h"

class JellyDialog : public State {

public:

	JellyDialog(StateMachine& machine, Framebuffer& mainRT);
	JellyDialog(StateMachine& machine, Framebuffer& mainRT, std::string text);
	virtual ~JellyDialog() = default;

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render() override;
	void resize(int deltaW, int deltaH) override;

protected:

	Framebuffer& mainRT;
	float m_controlsWidth, m_controlsHeight;

private:

	std::string m_text;
};

class JellyGame;
class JellyDialogPause : public JellyDialog {

public:

	JellyDialogPause(StateMachine& machine, Framebuffer& mainRT, JellyGame* game);
	~JellyDialogPause() = default;

	void update() override;
	void render() override;

private:

	JellyGame* m_game;
};

class JellyDialogFinish : public JellyDialog {

public:

	JellyDialogFinish(StateMachine& machine, Framebuffer& mainRT, bool newJumpRecord = false, bool newTimeRecord = false);
	~JellyDialogFinish() = default;

	void update() override;
	void render() override;

private:

	bool m_newJumpRecord, m_newTimeRecord;
};