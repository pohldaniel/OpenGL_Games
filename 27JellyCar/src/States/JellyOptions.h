#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/input/Keyboard.h>
#include <States/StateMachine.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Car/Car.h"
#include "Levels/LevelSoftBody.h"
#include "Levels/LevelManager.h"
#include "JellyPhysics/World.h"

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
	friend class JellyOptionCredit;
	friend class JellyOptionSound;
	friend class JellyOptionControl;

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

	JellyOptionState* addStateAtTop(JellyOptionState* state);
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void processInput();

	unsigned int m_backWidth, m_backHeight;
	int m_columns, m_rows;
	float m_controlsWidth, m_controlsHeight;

	LevelManager* _levelManager;
	std::vector<LevelSoftBody*> _gameBodies;
	World* _world;

	glm::mat4 _jellyProjection;
	glm::vec4 _screenBounds;
	
	unsigned int m_options;

	std::vector<LevelSoftBody*> _menuBodies;
	int _menuBodySelected;

	float _alphaScale = 1.0f;
	float _scaleFactor = 0.01f;


	float dragX, dragY;
	bool touchF = false;
	int dragPoint = -1;
	Body *dragBody;

	std::stack<JellyOptionState*> m_states;
};

////////////////////////////////////////////////////////////////////////
class JellyOptionState : public StateInterface {

	friend class JellyOptions;

public:

	JellyOptionState(JellyOptions& machine);
	virtual ~JellyOptionState() = default;
	virtual void resize(int deltaW, int deltaH) override;

protected:

	virtual void OnKeyDown(Event::KeyboardEvent& event) = 0;
	
	JellyOptions& m_machine;
	Matrix4f m_orthographic;

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

	std::vector<Text> _libs;
	int centerX;
	float _libsPosition;	
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
	void initCredits();

	std::vector<Text> _credits;
	float _creditsPosition;
	int centerX;
};

class JellyOptionControl : public JellyOptionState {

public:

	JellyOptionControl(JellyOptions& machine);
	~JellyOptionControl();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	void OnKeyDown(Event::KeyboardEvent& event) override;
	void processInput() override;
	void initActionNames();
	void loadSettings(std::string path);
	void saveSettings(std::string path);
	std::map<int, int> getActionKeyMapping();

	std::vector<CarAction> _carActions;
	std::map<CarAction, std::string> _actionTranslation;
	std::map<CarAction, Keyboard::Key> _carKeyboardMapping;

	float _alphaScale;
	int centerX;

	float _scaleFactor;

	int _selctedPosition;
	bool _changeBinding;
	CarAction _selectedAction;

	float controlsWidth, controlsHeight;
};

class JellyOptionSound : public JellyOptionState {

public:

	JellyOptionSound(JellyOptions& machine);
	~JellyOptionSound();

	void fixedUpdate() override;
	void update() override;
	void render() override;

private:

	void OnKeyDown(Event::KeyboardEvent& event) override;
	void processInput() override;

	void loadSettings(std::string path);
	void saveSettings(std::string path);

	float _carVolume;
	float _soundsVolume;
	float _musicVolume;

	int _soundPosition;
	float _alphaScale;
	float _scaleFactor;

	int _optionsCarLevel;
	int _optionsSoundLevel;
	int _optionsMusicLevel;	
};