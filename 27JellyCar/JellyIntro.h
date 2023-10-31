#ifndef JellyIntro_H
#define JellyIntro_H

#include <_Andromeda/RenderManager.h>
#include <_Andromeda/Sprite.h>
#include <_Andromeda/Timer2.h>


#include "JellyPhysics/JellyPhysics.h"

#include "Car/Car.h"
#include "Levels/LevelManager.h"
#include "Levels/LevelSoftBody.h"

//#include "../InputHelper.h"
//#include "../AudioHelper.h"
#include "JellyCore.h"


using namespace JellyPhysics;

class JellyIntro 
{
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
	Sprite* _logoSprite;

	glm::mat4 _projection;
	glm::mat4 _jellyProjection;

	//timer
	Timer2* _timer;
	float _dt;

	bool _end;

	//input
	//InputHelper* _inputHelper;

	//audio
	//AudioHelper* _audioHelper;

	//level data
	LevelManager* _levelManager;

	//jelly physics
	World* _world;

	//car
	Car* _car;

	Vector2 _levelTarget;

	//level objects
	std::vector<LevelSoftBody*> _gameBodies;

public:

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

#endif