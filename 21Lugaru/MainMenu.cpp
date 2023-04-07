#include "MainMenu.h"
#include "Application.h"
#include "GameLu.hpp"

using namespace GameLu;

extern float screenwidth, screenheight;
extern int mainmenu;
extern float flashamount, flashr, flashg, flashb;
extern int flashdelay;
extern int bloodtoggle;
extern bool ismotionblur;
extern bool decalstoggle;
extern bool musictoggle;
extern bool invertmouse;
extern float usermousesensitivity;
extern float volume;
extern bool showdamagebar;
extern bool devtools;
extern int detail;

std::vector<MenuItem> MainMenu::items;

MainMenu::MainMenu(StateMachine& machine) : State(machine, CurrentState::MAINMENU) {
	Application::SetCursorIcon("Data/Cursors/black.cur");
	EventDispatcher::AddMouseListener(this);

	Mainmenuitems[0].load("Textures/Lugaru.png", 0);
	Mainmenuitems[1].load("Textures/NewGame.png", 0);
	Mainmenuitems[2].load("Textures/Options.png", 0);
	Mainmenuitems[3].load("Textures/Quit.png", 0);
	Mainmenuitems[4].load("Textures/Eyelid.png", 0);
	Mainmenuitems[5].load("Textures/Resume.png", 0);
	Mainmenuitems[6].load("Textures/EndGame.png", 0);


	cursortexture.load("Textures/Cursor.png", 0);

	Mapcircletexture.load("Textures/MapCircle.png", 0);
	Mapboxtexture.load("Textures/MapBox.png", 0);
	Maparrowtexture.load("Textures/MapArrow.png", 0);

	Load();
}

MainMenu::~MainMenu() {
	EventDispatcher::RemoveMouseListener(this);
}

void MainMenu::fixedUpdate() {}

void MainMenu::update() {
	processInput();

	if (flashamount > 1) {
		flashamount = 1;
	}
	if (flashdelay <= 0) {
		flashamount -= m_dt;
	}
	flashdelay--;
	if (flashamount < 0) {
		flashamount = 0;
	}

	handleFadeEffect(m_dt);
}

void MainMenu::render() {
	drawMenu();
}

void MainMenu::processInput() {
	Keyboard &keyboard = Keyboard::instance();	

	if (keyboard.keyPressed(Keyboard::KEY_F)) {
		flash();
	}
}

void MainMenu::resize(int deltaW, int deltaH) {

}

void MainMenu::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void MainMenu::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	
}

void MainMenu::drawMenu() {

	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	GameLu::ReSizeGLScene(90, .1f);

	//draw menu background
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.001f);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, screenwidth, 0, screenheight, -100, 100);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(screenwidth / 2, screenheight / 2, 0);
	glPushMatrix();
	glScalef((float)screenwidth / 2, (float)screenheight / 2, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glColor4f(0, 0, 0, 1.0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glVertex3f(-1, -1, 0);
	glVertex3f(+1, -1, 0);
	glVertex3f(+1, +1, 0);
	glVertex3f(-1, +1, 0);
	glEnd();
	glEnable(GL_BLEND);
	glColor4f(0.4, 0.4, 0.4, 1.0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glEnable(GL_TEXTURE_2D);
	GameLu::Mainmenuitems[4].bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, 0);
	glTexCoord2f(1, 0);
	glVertex3f(+1, -1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(+1, +1, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, +1, 0);
	glEnd();
	glPopMatrix();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 640, 0, 480, -100, 100);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);

	drawItems();

	//draw mouse cursor
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, screenwidth, 0, screenheight, -100, 100);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(screenwidth / 2, screenheight / 2, 0);
	glPushMatrix();
	glScalef((float)screenwidth / 2, (float)screenheight / 2, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glPopMatrix();
	if (!GameLu::waiting) { // hide the cursor while waiting for a key
		glPushMatrix();
		glTranslatef(GameLu::mousecoordh - screenwidth / 2, GameLu::mousecoordv * -1 + screenheight / 2, 0);
		glScalef((float)screenwidth / 64, (float)screenwidth / 64, 1);
		glTranslatef(1, -1, 0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1, 1, 1, 1);
		GameLu::cursortexture.bind();
		glPushMatrix();
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-1, -1, 0.0f);
		glTexCoord2f(1, 0);
		glVertex3f(1, -1, 0.0f);
		glTexCoord2f(1, 1);
		glVertex3f(1, 1, 0.0f);
		glTexCoord2f(0, 1);
		glVertex3f(-1, 1, 0.0f);
		glEnd();
		glPopMatrix();
		glPopMatrix();
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	//draw screen flash
	if (flashamount > 0) {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDepthMask(GL_FALSE);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, screenwidth, 0, screenheight, -100, 100);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glScalef(screenwidth, screenheight, 1);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor4f(flashr, flashg, flashb, flashamount);
		glBegin(GL_QUADS);
		glVertex3f(0, 0, 0.0f);
		glVertex3f(256, 0, 0.0f);
		glVertex3f(256, 256, 0.0f);
		glVertex3f(0, 256, 0.0f);
		glEnd();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}
}

void MainMenu::drawItems() {
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	for (vector<MenuItem>::iterator it = items.begin(); it != items.end(); it++) {
		switch (it->type) {
		case MenuItem::IMAGE:
		case MenuItem::IMAGEBUTTON:
		case MenuItem::MAPMARKER:
			glColor4f(it->r, it->g, it->b, 1);
			glPushMatrix();
			if (it->type == MenuItem::MAPMARKER) {
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glTranslatef(2.5, -4.5, 0); //from old code
			}
			else {
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			}
			it->texture.bind();
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex3f(it->x, it->y, 0);
			glTexCoord2f(1, 0);
			glVertex3f(it->x + it->w, it->y, 0);
			glTexCoord2f(1, 1);
			glVertex3f(it->x + it->w, it->y + it->h, 0);
			glTexCoord2f(0, 1);
			glVertex3f(it->x, it->y + it->h, 0);
			glEnd();
			if (it->type != MenuItem::IMAGE) {
				//mouseover highlight
				for (int i = 0; i < 10; i++) {
					if (1 - ((float)i) / 10 - (1 - it->effectfade) > 0) {
						glColor4f(it->r, it->g, it->b, (1 - ((float)i) / 10 - (1 - it->effectfade)) * .25);
						glBegin(GL_QUADS);
						glTexCoord2f(0, 0);
						glVertex3f(it->x - ((float)i) * 1 / 2, it->y - ((float)i) * 1 / 2, 0);
						glTexCoord2f(1, 0);
						glVertex3f(it->x + it->w + ((float)i) * 1 / 2, it->y - ((float)i) * 1 / 2, 0);
						glTexCoord2f(1, 1);
						glVertex3f(it->x + it->w + ((float)i) * 1 / 2, it->y + it->h + ((float)i) * 1 / 2, 0);
						glTexCoord2f(0, 1);
						glVertex3f(it->x - ((float)i) * 1 / 2, it->y + it->h + ((float)i) * 1 / 2, 0);
						glEnd();
					}
				}
			}
			glPopMatrix();
			break;
		case MenuItem::LABEL:
		case MenuItem::BUTTON:
			glColor4f(it->r, it->g, it->b, 1);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			GameLu::text->glPrint(it->x, it->y, it->text.c_str(), 0, 1, 640, 480);
			if (it->type != MenuItem::LABEL) {
				//mouseover highlight
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				for (int i = 0; i < 15; i++) {
					if (1 - ((float)i) / 15 - (1 - it->effectfade) > 0) {
						glColor4f(it->r, it->g, it->b, (1 - ((float)i) / 10 - (1 - it->effectfade)) * .25);
						GameLu::text->glPrint(it->x - ((float)i), it->y, it->text.c_str(), 0, 1 + ((float)i) / 70, 640, 480);
					}
				}
			}
			break;
		case MenuItem::MAPLABEL:
			GameLu::text->glPrintOutlined(0.9, 0, 0, 1, it->x, it->y, it->text.c_str(), 0, 0.6, 640, 480);
			break;
		case MenuItem::MAPLINE: {
			XYZ linestart;
			linestart.x = it->x;
			linestart.y = it->y;
			linestart.z = 0;
			XYZ lineend;
			lineend.x = it->x + it->w;
			lineend.y = it->y + it->h;
			lineend.z = 0;
			XYZ offset = lineend - linestart;
			XYZ fac = offset;
			Normalise(&fac);
			offset = DoRotation(offset, 0, 0, 90);
			Normalise(&offset);

			linestart += fac * 4 * it->linestartsize;
			lineend -= fac * 4 * it->lineendsize;

			glDisable(GL_TEXTURE_2D);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(it->r, it->g, it->b, 1);
			glPushMatrix();
			glTranslatef(2, -5, 0); //from old code
			glBegin(GL_QUADS);
			glVertex3f(linestart.x - offset.x * it->linestartsize, linestart.y - offset.y * it->linestartsize, 0.0f);
			glVertex3f(linestart.x + offset.x * it->linestartsize, linestart.y + offset.y * it->linestartsize, 0.0f);
			glVertex3f(lineend.x + offset.x * it->lineendsize, lineend.y + offset.y * it->lineendsize, 0.0f);
			glVertex3f(lineend.x - offset.x * it->lineendsize, lineend.y - offset.y * it->lineendsize, 0.0f);
			glEnd();
			glPopMatrix();
			glEnable(GL_TEXTURE_2D);
		} break;
		default:
		case MenuItem::NONE:
			break;
		}
	}
}

void MainMenu::handleFadeEffect(float delta) {

	for (vector<MenuItem>::iterator it = items.begin(); it != items.end(); it++) {
		if (it->id == GameLu::selected) {
			it->effectfade += delta * 5;
			if (it->effectfade > 1) {
				it->effectfade = 1;
			}
		}
		else {
			it->effectfade -= delta * 5;
			if (it->effectfade < 0) {
				it->effectfade = 0;
			}
		}
	}
}

void MainMenu::clearMenu() {
	items.clear();
}

void MainMenu::addImage(int id, TextureLu texture, int x, int y, int w, int h, float r, float g, float b) {
	items.emplace_back(MenuItem::IMAGE, id, "", texture, x, y, w, h, r, g, b);
}

void MainMenu::addButtonImage(int id, TextureLu texture, int x, int y, int w, int h, float r, float g, float b) {
	items.emplace_back(MenuItem::IMAGEBUTTON, id, "", texture, x, y, w, h, r, g, b);
}

void MainMenu::addLabel(int id, const string& text, int x, int y, float r, float g, float b) {
	items.emplace_back(MenuItem::LABEL, id, text, TextureLu(), x, y, -1, -1, r, g, b);
}

void MainMenu::addButton(int id, const string& text, int x, int y, float r, float g, float b) {
	items.emplace_back(MenuItem::BUTTON, id, text, TextureLu(), x, y, -1, -1, r, g, b);
}

void MainMenu::setText(int id, const string& text) {
	for (vector<MenuItem>::iterator it = items.begin(); it != items.end(); it++) {
		if (it->id == id) {
			it->text = text;
			it->w = it->text.length() * 10;
			break;
		}
	}
}

void MainMenu::setText(int id, const string& text, int x, int y, int w, int h) {
	for (vector<MenuItem>::iterator it = items.begin(); it != items.end(); it++) {
		if (it->id == id) {
			it->text = text;
			it->x = x;
			it->y = y;
			if (w == -1) {
				it->w = it->text.length() * 10;
			}
			if (h == -1) {
				it->h = 20;
			}
			break;
		}
	}
}

void MainMenu::updateSettingsMenu() {
	std::string sbuf = std::string("Resolution: ") + to_string(newscreenwidth) + "*" + to_string(newscreenheight);
	if (((float)newscreenwidth <= (float)newscreenheight * 1.61) && ((float)newscreenwidth >= (float)newscreenheight * 1.59)) {
		sbuf += " (widescreen)";
	}
	setText(0, sbuf);
	setText(14, Application::Fullscreen ? "Fullscreen: On" : "Fullscreen: Off");
	if (newdetail == 0) {
		setText(1, "Detail: Low");
	}
	if (newdetail == 1) {
		setText(1, "Detail: Medium");
	}
	if (newdetail == 2) {
		setText(1, "Detail: High");
	}
	if (bloodtoggle == 0) {
		setText(2, "Blood: Off");
	}
	if (bloodtoggle == 1) {
		setText(2, "Blood: On, low detail");
	}
	if (bloodtoggle == 2) {
		setText(2, "Blood: On, high detail (slower)");
	}
	setText(4, ismotionblur ? "Blur Effects: Enabled (less compatible)" : "Blur Effects: Disabled (more compatible)");
	setText(5, decalstoggle ? "Decals: Enabled (slower)" : "Decals: Disabled");
	setText(6, musictoggle ? "Music: Enabled" : "Music: Disabled");
	setText(9, invertmouse ? "Invert mouse: Yes" : "Invert mouse: No");
	setText(10, std::string("Mouse Speed: ") + to_string(int(usermousesensitivity * 5)));
	setText(11, std::string("Volume: ") + to_string(int(volume * 100)) + "%");
	setText(13, showdamagebar ? "Damage Bar: On" : "Damage Bar: Off");
	if ((newdetail == detail) && (newscreenheight == (int)screenheight) && (newscreenwidth == (int)screenwidth)) {
		setText(8, "Back");
	}
	else {
		setText(8, "Back (some changes take effect next time Lugaru is opened)");
	}
}

void MainMenu::updateStereoConfigMenu() {
	setText(0, std::string("Stereo mode: ") + StereoModeName(newstereomode));
	setText(1, std::string("Stereo separation: ") + to_string(stereoseparation));
	setText(2, std::string("Reverse stereo: ") + (stereoreverse ? "Yes" : "No"));
}

void MainMenu::Load() {
	clearMenu();
	switch (mainmenu) {
	case 1:
	case 2:
		addImage(0, Mainmenuitems[0], 150, 480 - 128, 256, 128);
		addButtonImage(1, Mainmenuitems[mainmenu == 1 ? 1 : 5], 18, 480 - 152 - 32, 128, 32);
		addButtonImage(2, Mainmenuitems[2], 18, 480 - 228 - 32, 112, 32);
		addButtonImage(3, Mainmenuitems[mainmenu == 1 ? 3 : 6], 18, 480 - 306 - 32, mainmenu == 1 ? 68 : 132, 32);
		addLabel(-1, VERSION_NUMBER + VERSION_SUFFIX, 640 - 100, 10);
		break;
	case 3:
		addButton(0, "", 10 + 20, 440);
		addButton(14, "", 10 + 400, 440);
		addButton(1, "", 10 + 60, 405);
		addButton(2, "", 10 + 70, 370);
		addButton(4, "", 10, 335);
		addButton(5, "", 10 + 60, 300);
		addButton(6, "", 10 + 70, 265);
		addButton(9, "", 10, 230);
		addButton(10, "", 20, 195);
		addButton(11, "", 10 + 60, 160);
		addButton(13, "", 30, 125);
		addButton(7, "-Configure Controls-", 10 + 15, 90);
		addButton(12, "-Configure Stereo -", 10 + 15, 55);
		addButton(8, "Back", 10, 10);
		updateSettingsMenu();
		break;
	case 4:
		addButton(0, "", 10, 400);
		addButton(1, "", 10 + 40, 360);
		addButton(2, "", 10 + 40, 320);
		addButton(3, "", 10 + 30, 280);
		addButton(4, "", 10 + 20, 240);
		addButton(5, "", 10 + 40, 200);
		addButton(6, "", 10 + 40, 160);
		addButton(7, "", 10 + 30, 120);
		addButton(8, "", 10 + 20, 80);
		if (devtools) {
			addButton(9, "", 10 + 10, 40);
		}
		addButton(devtools ? 10 : 9, "Back", 10, 10);
		//updateControlsMenu();
		break;
	case 5: {
		//LoadCampaign();
		addLabel(-1, Account::active().getName(), 5, 400);
		addButton(1, "Tutorial", 5, 300);
		addButton(2, "Challenge", 5, 240);
		addButton(3, "Delete User", 400, 10);
		addButton(4, "Main Menu", 5, 10);
		addButton(5, "Change User", 5, 180);
		addButton(6, "Campaign : " + Account::active().getCurrentCampaign(), 200, 420);

		//show campaign map
		//with (2,-5) offset from old code
		addImage(-1, Mainmenuitems[7], 150 + 2, 60 - 5, 400, 400);
		//show levels
		/*int numlevels = Account::active().getCampaignChoicesMade();
		numlevels += numlevels > 0 ? campaignlevels[numlevels - 1].nextlevel.size() : 1;
		for (int i = 0; i < numlevels; i++) {
			XYZ midpoint = campaignlevels[i].getCenter();
			float itemsize = campaignlevels[i].getWidth();
			const bool active = (i >= Account::active().getCampaignChoicesMade());
			if (!active) {
				itemsize /= 2;
			}

			if (i >= 1) {
				XYZ start = campaignlevels[i - 1].getCenter();
				addMapLine(start.x, start.y, midpoint.x - start.x, midpoint.y - start.y, 0.5, active ? 1 : 0.5, active ? 1 : 0.5, 0, 0);
			}
			addMapMarker(NB_CAMPAIGN_MENU_ITEM + i, Mapcircletexture,
				midpoint.x - itemsize / 2, midpoint.y - itemsize / 2, itemsize, itemsize, active ? 1 : 0.5, 0, 0);

			if (active) {
				addMapLabel(-2, campaignlevels[i].description,
					campaignlevels[i].getStartX() + 10,
					campaignlevels[i].getStartY() - 4);
			}
		}*/
	} break;
	case 6:
		addLabel(-1, "Are you sure you want to delete this user?", 10, 400);
		addButton(1, "Yes", 10, 360);
		addButton(2, "No", 10, 320);
		break;
	case 7:
		if (Account::getNbAccounts() < 8) {
			addButton(0, "New User", 10, 400);
		}
		else {
			addLabel(0, "No More Users", 10, 400);
		}
		addLabel(-2, "", 20, 400);
		addButton(Account::getNbAccounts() + 1, "Back", 10, 10);
		for (int i = 0; i < Account::getNbAccounts(); i++) {
			addButton(i + 1, Account::get(i).getName(), 10, 340 - 20 * (i + 1));
		}
		break;
	case 8:
		addButton(0, "Easier", 10, 400);
		addButton(1, "Difficult", 10, 360);
		addButton(2, "Insane", 10, 320);
		break;
	case 9:
		for (int i = 0; i < numchallengelevels; i++) {
			string name = "Level ";
			name += to_string(i + 1);
			if (name.size() < 17) {
				name.append((17 - name.size()), ' ');
			}
			name += to_string(int(Account::active().getHighScore(i)));
			if (name.size() < 32) {
				name.append((32 - name.size()), ' ');
			}
			int fasttime = (int)round(Account::active().getFastTime(i));
			name += to_string(int((fasttime - fasttime % 60) / 60));
			name += ":";
			if (fasttime % 60 < 10) {
				name += "0";
			}
			name += to_string(fasttime % 60);

			addButton(i, name, 10, 400 - i * 25, i > Account::active().getProgress() ? 0.5 : 1, 0, 0);
		}

		addButton(-1, "             High Score      Best Time", 10, 440);
		addButton(numchallengelevels, "Back", 10, 10);
		break;
	case 10: {
		//addLabel(0, campaignEndText[0], 80, 330);
		//addLabel(1, campaignEndText[1], 80, 300);
		//addLabel(2, campaignEndText[2], 80, 270);
		addButton(3, "Back", 10, 10);
		addLabel(4, string("Your score:         ") + to_string((int)Account::active().getCampaignScore()), 190, 200);
		addLabel(5, string("Highest score:      ") + to_string((int)Account::active().getCampaignHighScore()), 190, 180);
	} break;
	case 18:
		addButton(0, "", 70, 400);
		addButton(1, "", 10, 360);
		addButton(2, "", 40, 320);
		addButton(3, "Back", 10, 10);
		updateStereoConfigMenu();
		break;
	}
}

MenuItem::MenuItem(MenuItemType _type, int _id, const string& _text, TextureLu _texture,
	int _x, int _y, int _w, int _h, float _r, float _g, float _b,
	float _linestartsize, float _lineendsize)
	: type(_type)
	, id(_id)
	, text(_text)
	, texture(_texture)
	, x(_x)
	, y(_y)
	, w(_w)
	, h(_h)
	, r(_r)
	, g(_g)
	, b(_b)
	, effectfade(0)
	, linestartsize(_linestartsize)
	, lineendsize(_lineendsize) {

	if (type == MenuItem::BUTTON) {
		if (w == -1) {
			w = text.length() * 10;
		}
		if (h == -1) {
			h = 20;
		}
	}
}

