#include <time.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "GameLu.hpp"
#include "Tutorial.h"
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

Tutorial::Tutorial(StateMachine& machine) : State(machine, CurrentState::TUTORIAL) {
	Menu::startChallengeLevel(-1);
}

Tutorial::~Tutorial() {
	deleteGame();
}

void Tutorial::fixedUpdate() {

}

void Tutorial::update() {

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

void Tutorial::render() {

	if (stereomode == stereoNone) {
		DrawGLScene(stereoCenter);
	}
	else {
		DrawGLScene(stereoLeft);
		DrawGLScene(stereoRight);
	}

	if (Globals::drawUi)
		renderUi();
}

void Tutorial::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Tutorial::OnMouseButtonDown(Event::MouseButtonEvent& event) {

}

void Tutorial::OnMouseButtonUp(Event::MouseButtonEvent& event) {

}

void Tutorial::resize(int deltaW, int deltaH) {

}

void Tutorial::renderUi() {
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

void Tutorial::DoMouse() {

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