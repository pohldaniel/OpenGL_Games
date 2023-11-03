#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <States/StateMachine.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Car/Car.h"
#include "Levels/LevelSoftBody.h"
#include "Levels/LevelManager.h"
#include "JellyPhysics/World.h"

#include <_Andromeda/TextureManager.h>

enum CarAction {
	Left = 0,
	Right = 1,
	RotateLeft = 2,
	RotateRight = 3,
	Transform = 4,
	Ballon = 5,
	Tire = 6,
	Map = 7,
	Count = 8
};

enum JellyOptionsState{
	Menu,
	Sound,
	Controls,
	Credits,
	Libs,
	Secret
};

class Text {

public:

	float StartPosition;
	std::string Content;

	Text(std::string content, float startPostion){
		Content = content;
		StartPosition = startPostion;
	}
};

class JellyOptions : public State, public KeyboardEventListener {

public:

	JellyOptions(StateMachine& machine);
	~JellyOptions();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	void OnKeyDown(Event::KeyboardEvent& event) override;
	void processInput();

	void InitCredits();
	void InitLibs();
	void InitActionNames();

	unsigned int backWidth = 0, backHeight = 0;
	unsigned int columns = 0, rows = 0;
	float controlsWidth = 0, controlsHeight = 0;

	std::vector<SkinInfo> _carSkins;
	LevelManager* _levelManager;
	std::vector<LevelSoftBody*> _gameBodies;
	World* _world;
	float _dt;

	glm::mat4 _jellyProjection;
	glm::vec4 _screenBounds;
	
	unsigned int m_options;

	std::vector<Text> _credits;
	float _creditsPosition = 600.0f;

	std::vector<Text> _libs;
	float _libsPosition = 600.0f;

	std::vector<CarAction> _carActions;
	std::map<CarAction, std::string> _actionTranslation;

	std::vector<LevelSoftBody*> _menuBodies;
	int _menuBodySelected;

	JellyOptionsState _optionsState;
	float _alphaScale = 1.0f;
	float _scaleFactor = 0.01f;
	bool _changeBinding;
	Body *dragBody;

	Texture2* _creditsTexture;
	Texture2* _libsTexture;
	Texture2* _keyboardTexture;
	Texture2* _gamepadTexture;
	Texture2* _secretTexture;
	Texture2* _volumeTexture;
};