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

class Text {

public:

	float StartPosition;
	std::string Content;

	Text(std::string content, float startPostion){
		Content = content;
		StartPosition = startPostion;
	}
};

class JellyOptionState;

class JellyOptions : public State, public KeyboardEventListener {

	friend class JellyOptionLib;

public:

	JellyOptions(StateMachine& machine);
	~JellyOptions();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

	void renderLevel();
	void renderBackground();
	void renderControls();

private:

	void OnKeyDown(Event::KeyboardEvent& event) override;
	void processInput();

	void InitCredits();
	
	void InitActionNames();

	unsigned int backWidth = 0, backHeight = 0;
	unsigned int columns = 0, rows = 0;
	float controlsWidth = 0, controlsHeight = 0;

	std::vector<SkinInfo> _carSkins;
	LevelManager* _levelManager;
	std::vector<LevelSoftBody*> _gameBodies;
	World* _world;

	glm::mat4 _jellyProjection;
	glm::vec4 _screenBounds;
	
	unsigned int m_options;

	std::vector<Text> _credits;
	float _creditsPosition = 600.0f;

	std::vector<CarAction> _carActions;
	std::map<CarAction, std::string> _actionTranslation;

	std::vector<LevelSoftBody*> _menuBodies;
	int _menuBodySelected;

	float _alphaScale = 1.0f;
	float _scaleFactor = 0.01f;
	bool _changeBinding;

	float dragX, dragY;
	bool touchF = false;
	int dragPoint = -1;
	Body *dragBody;

	Texture2* _creditsTexture;
	Texture2* _libsTexture;
	Texture2* _keyboardTexture;
	Texture2* _gamepadTexture;
	Texture2* _secretTexture;
	Texture2* _volumeTexture;

	int _selctedPosition;
	int _soundPosition = 0;


	std::stack<JellyOptionState*> m_states;
	JellyOptionState* addStateAtTop(JellyOptionState* state);
};

////////////////////////////////////////////////////////////////////////
class JellyOptionState : public StateInterface {

	friend class JellyOptions;

public:

	JellyOptionState(JellyOptions& machine);
	virtual ~JellyOptionState() = default;

protected:

	virtual void OnKeyDown(Event::KeyboardEvent& event) = 0;
	JellyOptions& m_machine;

private:

	virtual void processInput() = 0;
};

class JellyOptionLib : public JellyOptionState {

public:

	JellyOptionLib(JellyOptions& machine);
	~JellyOptionLib() = default;

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	void OnKeyDown(Event::KeyboardEvent& event) override;
	void processInput() override;

	void initLibs();

	int centerX;
	float _libsPosition;
	std::vector<Text> _libs;
	
};

class JellyOptionCredit : public JellyOptionState {

public:

	JellyOptionCredit(JellyOptions& machine);
	~JellyOptionCredit() = default;

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	void OnKeyDown(Event::KeyboardEvent& event) override;
	void processInput() override;
};

class JellyOptionControl : public JellyOptionState {

public:

	JellyOptionControl(JellyOptions& machine);
	~JellyOptionControl() = default;

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	void OnKeyDown(Event::KeyboardEvent& event) override;
	void processInput() override;
};

class JellyOptionSound : public JellyOptionState {

public:

	JellyOptionSound(JellyOptions& machine);
	~JellyOptionSound() = default;

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	void OnKeyDown(Event::KeyboardEvent& event) override;
	void processInput() override;
};