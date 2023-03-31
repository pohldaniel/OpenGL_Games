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

	newGame();

	cellophane = 0;
	texdetail = 4;
	slomospeed = 0.25;
	slomofreq = 8012;

	DefaultSettings();

	initGL();
	GLint width = kContextWidth;
	GLint height = kContextHeight;
	screenwidth = width;
	screenheight = height;

	newdetail = detail;
	newscreenwidth = screenwidth;
	newscreenheight = screenheight;

	InitGame();

	bool gameDone = false;
	bool gameFocused = true;

	srand(time(nullptr));
	Menu::startChallengeLevel(1);
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

void Game::render(unsigned int &frameBuffer) {

	if (stereomode == stereoNone) {
		DrawGLScene(stereoCenter);
	} else {
		DrawGLScene(stereoLeft);
		DrawGLScene(stereoRight);
	}

	if(Globals::drawUi)
		renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	
}

void Game::resize(int deltaW, int deltaH) {
	
}

void Game::renderUi() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceId = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();

	if (m_initUi) {
		m_initUi = false;
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

void Game::initGL() {

	// clear all states
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glDisable(GL_LOGIC_OP);
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_TEXTURE_2D);
	glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
	glPixelTransferi(GL_RED_SCALE, 1);
	glPixelTransferi(GL_RED_BIAS, 0);
	glPixelTransferi(GL_GREEN_SCALE, 1);
	glPixelTransferi(GL_GREEN_BIAS, 0);
	glPixelTransferi(GL_BLUE_SCALE, 1);
	glPixelTransferi(GL_BLUE_BIAS, 0);
	glPixelTransferi(GL_ALPHA_SCALE, 1);
	glPixelTransferi(GL_ALPHA_BIAS, 0);

	// set initial rendering states
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_DITHER);
	glEnable(GL_COLOR_MATERIAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glAlphaFunc(GL_GREATER, 0.5f);

	if (CanInitStereo(stereomode)) {
		InitStereo(stereomode);
	}
	else {
		fprintf(stderr, "Failed to initialize stereo, disabling.\n");
		stereomode = stereoNone;
	}
}