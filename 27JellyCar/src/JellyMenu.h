#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <_Andromeda/Sprite.h>
#include <_Andromeda/Timer2.h>

#include "Levels/LevelManager.h"
#include "JellyCore.h"


using namespace JellyPhysics;

class JellyMenuBetter {

private:

	JellyCore* _core;

	//render manager
	RenderManager* _renderManager;

	//shader manager
	ShaderManager* _shaderManager;

	//texture manager
	TextureManager* _textureManager;

	//font
	TexturedFont* _menuFont;
	TexturedFont* _smallFont;
	TexturedFont* _titleFont;

	//shader
	Shader2* _shader;
	Sprite* _backSprite;
	Sprite* _levelImage;

	glm::mat4 _projection;
	glm::mat4 _jellyProjection;

	//timer
	Timer2* _timer;
	float _dt;

	//input
	//InputHelper* _inputHelper;

	//audio
	//AudioHelper* _audioHelper;

	//level data
	LevelManager* _levelManager;

	//menu
	int currentPosition;
	int columnStartPosition;
	int positionsInColumn;
	std::vector<std::string> _sceneFiles;

	//car skins names
	int carcurrentPosition;
	int carcolumnStartPosition;
	int carpositionsInColumn;
	std::vector<SkinInfo> _carSkins;

	//
	//jelly physics
	World* _world;

	//car
	Car* _car;

	//level objects
	std::vector<LevelSoftBody*> _gameBodies;

	//JellyGame* _jellyGame;

public:

	JellyMenuBetter(JellyCore* core);

	void Init();
	void Enter();
	void CleanUp();

	void Pause();
	void Resume();

	void GamePause();
	void GameResume();

	void HandleEvents();
	void Update();
	void Draw();
};