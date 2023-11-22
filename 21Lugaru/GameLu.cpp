/*
Copyright (C) 2003, 2010 - Wolfire Games
Copyright (C) 2010-2017 - Lugaru contributors (see AUTHORS file)

This file is part of Lugaru.

Lugaru is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Lugaru is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lugaru.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "GameLu.hpp"

#include "Audio/openal_wrapper.hpp"
#include "Level/Dialog.hpp"


extern int mainmenu;

const char* pathtypenames[] = { "keepwalking", "pause" };
const char* editortypenames[] = {
    "active", "sitting", "sitting wall", "sleeping",
    "dead1", "dead2", "dead3", "dead4"
};

namespace GameLu
{
TextureLu terraintexture;
TextureLu terraintexture2;
TextureLu loadscreentexture;
TextureLu Maparrowtexture;
TextureLu Mapboxtexture;
TextureLu Mapcircletexture;
TextureLu cursortexture;
GLuint screentexture = 0;
GLuint screentexture2 = 0;
TextureLu Mainmenuitems[10];

int selected = 0;
int keyselect = 0;

int newdetail = 0;
int newscreenwidth = 0;
int newscreenheight = 0;

bool gameon = 0;
float deltah = 0;
float deltav = 0;
int mousecoordh = 0;
int mousecoordv = 0;
int oldmousecoordh = 0;
int oldmousecoordv = 0;
float yaw = 0;
float pitch = 0;
SkyBox* skybox = NULL;
bool cameramode = 0;
bool firstLoadDone = false;

TextureLu hawktexture;
float hawkyaw = 0;
float hawkcalldelay = 0;

float leveltime = 0;
float wonleveltime = 0;
float loadtime = 0;

ModelLu hawk;
XYZ hawkcoords;
XYZ realhawkcoords;

ModelLu eye;
ModelLu iris;
ModelLu cornea;

bool stealthloading = 0;

int musictype = 0;

XYZ mapcenter;
float mapradius = 0;

Text* text = NULL;
Text* textmono = NULL;

bool editorenabled = 0;
int editortype = 0;
float editorsize = 0;
float editoryaw = 0;
float editorpitch = 0;

int tryquit = 0;

XYZ pathpoint[30];
int numpathpoints = 0;
int numpathpointconnect[30] = {};
int pathpointconnect[30][30] = {};
int pathpointselected = 0;

int endgame = 0;
bool scoreadded = 0;
int numchallengelevels = 0;

bool console = false;
std::string consoletext[15] = {};
float consoleblinkdelay = 0;
bool consoleblink = 0;
unsigned consoleselected = 0;

unsigned short crouchkey = 0, jumpkey = 0, forwardkey = 0, backkey = 0, leftkey = 0, rightkey = 0, drawkey = 0, throwkey = 0, attackkey = 0;
unsigned short consolekey = 0;

int loading = 0;

int oldenvironment = 0;
int targetlevel = 0;
float changedelay = 0;

bool waiting = false;
}

void GameLu::fireSound(int sound){
    emit_sound_at(sound);
}