#define NOMINMAX
#define STB_IMAGE_IMPLEMENTATION
#include <windows.h>			
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <tchar.h>
#include <list>
#include "stb\stb_image.h"

#include "Application.h"
#include "Clock.h"
#include "GameObject.h"
#include "Extension.h"
#include "Level.h"
#include "Character.h"
#include "Enemy.h"

extern float Globals::offset = 0.0f;
extern unsigned long Globals::CONTROLLS = 0;
extern unsigned char Globals::pKeyBuffer[256] = { 0 };

Character* character;
Level* level;
Enemy* enemies[10];
int noEnemies;

//prototype funktions
void initApp();
void cleanup();

float elapsed_secs = 0.0f;
int frames = 0;
double framesTime = 0;

void addGameObject(GameObject* gameObj);
void removeGameObject(GameObject* gameObj);
std::list<GameObject*> objectList;

// the main windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 1300, 0, 550, 300, true);
	std::cout << "arrow keys        : move character" << std::endl;
	std::cout << "space             : jump" << std::endl;
	std::cout << "v                 : toggle vsync" << std::endl;
	std::cout << "z                 : toggle wireframe" << std::endl;

	Application application;
	initApp();

	HWND hwnd = application.getWindow();
	HDC hdc;
	Clock deltaClock;
	// main message loop
	while (application.isRunning()) {

		
		elapsed_secs = deltaClock.restartSec();		
		// Display FPS
		framesTime += elapsed_secs;
		frames++;
		if (framesTime > 1) {
			_TCHAR fpsText[32];
			_sntprintf(fpsText, 32, "Game: %d FPS", frames);
			SetWindowText(hwnd, fpsText);
			frames = 0;
			framesTime = 0;
		}

		if (elapsed_secs > 0.1){
			elapsed_secs = 0.1; // safety so it doesn't go wild
		}
		
		glClearColor(0.0f, 0.60f, 0.86f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//////////////////////Simulation/////////////////////////////////
		std::list<GameObject*>::iterator iter;

		for (iter = objectList.begin(); iter != objectList.end(); iter++)
			(*iter)->update(elapsed_secs);

		///////////////////////Correct Simulation///////////////////////////////////////
		if (!character->isDead()) {
			
			character->goesLeft = Globals::CONTROLLS & Globals::KEY_LEFT;
			character->goesRight = Globals::CONTROLLS & Globals::KEY_RIGHT;
			
			level->pickUpCollectibles(character);
			
			CollisionDistances cd = level->characterCollides(character);
			if (cd.bottom > 0){
				// if it's a bottom collision, we stop the character from falling
				character->stopFalling(cd.bottom);
				if (Globals::CONTROLLS & Globals::KEY_UP){ // if the user pressed the jump key, the character jumps
					character->jump(true);
				}
			}

			if (cd.left > 0){
				// if it's a left collision
				character->stopMovingLeft(cd.left);
			}

			if (cd.right > 0){
				// if it's a right collision
				character->stopMovingRight(cd.right);
			}

			if (cd.top > 0){
				// if it's a top collision
				character->bounceTop();
			}

			for (int i = 0; i < noEnemies; i++){ 
				// check for collisions with enemies
				CollisionDistances cd = enemies[i]->characterCollides(character);
				if (cd.bottom > 0){

					// if we hit the enemy from above, remove enemy
					Enemy* enemy = enemies[i];
					for (int k = i; k < noEnemies - 1; k++){
						enemies[k] = enemies[k + 1];
					}
					enemies[noEnemies - 1] = NULL;
					removeGameObject(enemy);
					delete enemy;
					noEnemies--;
					character->jump(false);

				}else if (cd.left > 0 || cd.right > 0 || cd.top > 0){
					// if we hit the enemy from any other direction, die
					character->die();
				}
			}

			if (character->getPosition()[1] > HEIGHT){
				// If the character falls below the level, it dies
				character->die();
			}

			if (level->levelExit(character)){
				// if the character hit the exit point of the level, game over, you win
				std::cout << "Level Finished" << std::endl;
			}
				
			// Change the display offset based on character position, but clamp it to the limits
			Globals::offset = character->getPosition()[0] - WIDTH/ 2;
			if (Globals::offset < 0)
				Globals::offset = 0;
			if (Globals::offset > LEVEL_WIDTH* TILE_WIDTH - WIDTH)
				Globals::offset = LEVEL_WIDTH * TILE_WIDTH - WIDTH;
		}else{
			// if the character is dead (floating up) and it hits the top of the screen, we can reset (if we still have lives)
			if (character->getPosition()[1] < 0){
				character->reset();
			}
		}
		////////////////////////Rendering////////////////////////////////////////
		for (iter = objectList.begin(); iter != objectList.end(); iter++)
			(*iter)->render();
			
		hdc = GetDC(hwnd);
		SwapBuffers(hdc);
		ReleaseDC(hwnd, hdc);
	} // end while

	cleanup();

	hdc = GetDC(hwnd);
	wglMakeCurrent(hdc, 0);
	wglDeleteContext(wglGetCurrentContext());
	ReleaseDC(hwnd, hdc);

	return 0;
}

void initApp() {
	// Initialize level
	level = new Level();
	addGameObject(level);

	// Initialize character
	character = new Character();
	addGameObject(character);

	noEnemies = 3;
	enemies[0] = new Enemy(425, 350, 625, 0);
	addGameObject(enemies[0]);

	enemies[1] = new Enemy(1475, 150, 1525, 7);
	addGameObject(enemies[1]);

	enemies[2] = new Enemy(3275, 300, 3475, 11);
	addGameObject(enemies[2]);
}

void addGameObject(GameObject* gameObj) {
	objectList.push_back(gameObj);
}

void removeGameObject(GameObject* gameObj) {
	objectList.remove(gameObj);
}

void cleanup() {
	std::list<GameObject*>::iterator iter;
	for (iter = objectList.begin(); iter != objectList.end(); iter++)
		delete (*iter);

	objectList.clear();
}