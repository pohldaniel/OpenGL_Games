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
#include "TutorialLu.hpp"
#include "Environment/Lights.hpp"
#include "Environment/Terrain.hpp"
#include "Level/Awards.hpp"
#include "Level/Dialog.hpp"
#include "Level/Hotspot.hpp"
#include "Objects/Person.hpp"

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

extern float motionbluramount;
extern float slomodelay;
extern float camerashake;
extern bool freeze;
extern bool winfreeze;
extern bool gamestarted;
extern int kTextureSize;
extern float woozy;
extern int environment;
extern float blurness;
extern float targetblurness;
extern XYZ viewer;
extern FrustumLu frustum;
extern Terrain terrain;
extern Light light;
extern float smoketex;
extern float viewdistance;
extern float playerdist;
extern float fadestart;
extern bool campaign;
extern float blackout;
extern float flashamount, flashr, flashg, flashb;
extern int flashdelay;

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

	SetUpLighting();
	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);

	glViewport(0, 0, screenwidth, screenheight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, (GLfloat)screenwidth / (GLfloat)screenheight, 0.1f, viewdistance);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glAlphaFunc(GL_GREATER, 0.0001f);
	glEnable(GL_ALPHA_TEST);
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();

	glRotatef(pitch, 1, 0, 0);
	glRotatef(yaw, 0, 1, 0);
	SetUpLight(&light, 0);
	glPushMatrix();

	skybox->draw();
	glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, 0);
	glPopMatrix();
	glTranslatef(-viewer.x, -viewer.y, -viewer.z);
	frustum.GetFrustum();

	//Terrain
	glEnable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	terraintexture.bind();
	terrain.draw(0);
	
	//Model
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);

	glEnable(GL_COLOR_MATERIAL);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glDepthMask(GL_TRUE);

	Person::players[0]->DrawSkeleton();
	//Person::players[0]->DrawSkeletonSmall();

	glPushMatrix();
	glCullFace(GL_BACK);
	glEnable(GL_TEXTURE_2D);
	Object::Draw();
	glPopMatrix();

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