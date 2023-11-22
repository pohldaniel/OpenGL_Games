#include <time.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "GameLu.hpp"
#include "Game.h"
#include "User/Settings.hpp"
#include "Menu/Menu.hpp"
#include "Constants.h"

using namespace GameLu;

extern float multiplier;
extern float realmultiplier;
extern int slomo;
extern int difficulty;
extern int mainmenu;
extern float slomospeed;
extern bool cellophane;
extern float texdetail;
extern float slomofreq;

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

}

Game::~Game() {
	deleteGame();
}

void Game::fixedUpdate() {

}

void Game::update() {

	deltah = Mouse::instance().xPosRelative();
	deltav = Mouse::instance().yPosRelative();

	multiplier = m_dt;
	if (multiplier < .001) {
		multiplier = .001;
	}
	if (multiplier > 10) {
		multiplier = 10;
	}

	if (multiplier > .6) {
		multiplier = .6;
	}

	realmultiplier = multiplier;
	multiplier *= gamespeed;
	if (difficulty == 1) {
		multiplier *= .9;
	}
	if (difficulty == 0) {
		multiplier *= .8;
	}

	if (loading == 4) {
		multiplier *= .00001;
	}
	if (slomo && !mainmenu) {
		multiplier *= slomospeed;
	}

	DoMouse();

	TickOnce();
	Tick();
	TickOnceAfter();

};

void Game::render() {
	
	if (stereomode == stereoNone) {
		DrawGLScene(stereoCenter);
	} else {
		DrawGLScene(stereoLeft);
		DrawGLScene(stereoRight);
	}
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	
}

void Game::resize(int deltaW, int deltaH) {
	
}

void Game::DoMouse() {

	if (mainmenu || ((abs(deltah) < 10 * m_dt * 1000) && (abs(deltav) < 10 * m_dt * 1000))) {
		deltah *= usermousesensitivity;
		deltav *= usermousesensitivity;
		mousecoordh += deltah;
		mousecoordv += deltav;
		if (mousecoordh < 0) {
			mousecoordh = 0;
		}
		else if (mousecoordh >= kContextWidth) {
			mousecoordh = kContextWidth - 1;
		}
		if (mousecoordv < 0) {
			mousecoordv = 0;
		}
		else if (mousecoordv >= kContextHeight) {
			mousecoordv = kContextHeight - 1;
		}
	}
}