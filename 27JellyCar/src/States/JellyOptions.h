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
	Vector2f touchToScreen(Vector4f screenBound, Vector2f touch);

	unsigned int m_backWidth, m_backHeight;
	int m_columns, m_rows;
	float m_controlsWidth, m_controlsHeight;

	LevelManager* m_levelManager;
	std::vector<LevelSoftBody*> m_gameBodies;
	World* m_world;

	glm::mat4 m_jellyProjection;
	Vector4f m_screenBounds;
	
	unsigned int m_options;

	std::vector<LevelSoftBody*> m_menuBodies;
	int m_menuBodySelected;

	float m_alphaScale = 1.0f;
	float m_scaleFactor = 0.01f;


	float m_dragX, m_dragY;
	bool m_touchF = false;
	int m_dragPoint = -1;
	Body *m_dragBody;

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

	std::vector<Text> m_libs;
	int m_centerX;
	float m_libsPosition;
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

	std::vector<Text> m_credits;
	float m_creditsPosition;
	int m_centerX;
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

	std::vector<CarAction> m_carActions;
	std::map<CarAction, std::string> m_actionTranslation;
	std::map<CarAction, Keyboard::Key> m_carKeyboardMapping;

	float m_alphaScale;
	int m_centerX;

	float m_scaleFactor;

	int m_selctedPosition;
	bool m_changeBinding;
	CarAction m_selectedAction;

	float m_controlsWidth, m_controlsHeight;
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

	float m_carVolume;
	float m_soundsVolume;
	float m_musicVolume;

	int m_soundPosition;
	float m_alphaScale;
	float m_scaleFactor;

	int m_optionsCarLevel;
	int m_optionsSoundLevel;
	int m_optionsMusicLevel;
};