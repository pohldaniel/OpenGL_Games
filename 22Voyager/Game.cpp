#include <time.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "Constants.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

}

Game::~Game() {

}

void Game::fixedUpdate() {

}

void Game::update() {

};

void Game::render() {
	
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	
}

void Game::resize(int deltaW, int deltaH) {
	
}

