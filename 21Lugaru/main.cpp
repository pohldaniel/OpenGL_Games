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
#include <tchar.h>
#include "GameLu.hpp"
#include "Application.h"
#include "engine/Clock.h"
#include "engine/input/Mouse.h"
#include "Constants.h"

#include "Audio/openal_wrapper.hpp"
#include "Graphic/gamegl.hpp"
#include "Platform/Platform.hpp"
#include "User/Settings.hpp"
#include "Menu/Menu.hpp"
#include "Version.hpp"

#include <fstream>
#include <iostream>
#include <math.h>
#include <set>
#include <stdio.h>
#include <string.h>
#include <time.h>

using namespace GameLu;

#ifdef WIN32
#include <shellapi.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

extern float multiplier;
extern float realmultiplier;
extern int slomo;
extern bool cellophane;
extern float texdetail;

extern bool freeze;
extern bool stillloading;
extern int mainmenu;

extern float slomospeed;
extern float slomofreq;

extern int difficulty;

using namespace std;

set<pair<int, int>> resolutions;

// statics/globals (internal only) ------------------------------------------

// Menu defs

int kContextWidth;
int kContextHeight;

//-----------------------------------------------------------------------------------------------------------------------

// OpenGL Drawing

void initGL(){
   
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
    } else {
        fprintf(stderr, "Failed to initialize stereo, disabling.\n");
        stereomode = stereoNone;
    }
}
// --------------------------------------------------------------------------
static Point gMidPoint;


static void DoMouse(){

    if (mainmenu || ((abs(deltah) < 10 * realmultiplier * 1000) && (abs(deltav) < 10 * realmultiplier * 1000))) {
        deltah *= usermousesensitivity;
        deltav *= usermousesensitivity;
        mousecoordh += deltah;
        mousecoordv += deltav;
        if (mousecoordh < 0) {
            mousecoordh = 0;
        } else if (mousecoordh >= kContextWidth) {
            mousecoordh = kContextWidth - 1;
        }
        if (mousecoordv < 0) {
            mousecoordv = 0;
        } else if (mousecoordv >= kContextHeight) {
            mousecoordv = kContextHeight - 1;
        }
    }
}

void DoFrameRate(int update)
{
    static long frames = 0;

    static AbsoluteTime time = { 0, 0 };
    static AbsoluteTime frametime = { 0, 0 };
    AbsoluteTime currTime = UpTime();
    double deltaTime = (float)AbsoluteDeltaToDuration(currTime, frametime);

    if (0 > deltaTime) { // if negative microseconds
        deltaTime /= -1000000.0;
    } else { // else milliseconds
        deltaTime /= 1000.0;
    }

    multiplier = deltaTime;
    if (multiplier < .001) {
        multiplier = .001;
    }
    if (multiplier > 10) {
        multiplier = 10;
    }
    if (update) {
        frametime = currTime; // reset for next time interval
    }

    deltaTime = (float)AbsoluteDeltaToDuration(currTime, time);

    if (0 > deltaTime) { // if negative microseconds
        deltaTime /= -1000000.0;
    } else { // else milliseconds
        deltaTime /= 1000.0;
    }
    frames++;
    if (0.001 <= deltaTime) { // has update interval passed
        if (update) {
            time = currTime; // reset for next time interval
            frames = 0;
        }
    }
}

void DoUpdate()
{
    static float sps = 200;
    static int count;
    static float oldmult;

    DoFrameRate(1);
    if (multiplier > .6) {
        multiplier = .6;
    }

    fps = 1 / multiplier;

    count = multiplier * sps;
    if (count < 2) {
        count = 2;
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
    oldmult = multiplier;
    multiplier /= (float)count;

    DoMouse();

    TickOnce();

    for (int i = 0; i < count; i++) {
        Tick();
    }
    multiplier = oldmult;

    TickOnceAfter();
    /* - Debug code to test how many channels were active on average per frame
        static long frames = 0;

        static AbsoluteTime start = {0,0};
        AbsoluteTime currTime = UpTime ();
        static int num_channels = 0;

        num_channels += OPENAL_GetChannelsPlaying();
        double deltaTime = (float) AbsoluteDeltaToDuration (currTime, start);

        if (0 > deltaTime)  // if negative microseconds
            deltaTime /= -1000000.0;
        else                // else milliseconds
            deltaTime /= 1000.0;

        ++frames;

        if (deltaTime >= 1)
        {
            start = currTime;
            float avg_channels = (float)num_channels / (float)frames;

            ofstream opstream("log.txt",ios::app);
            opstream << "Average frame count: ";
            opstream << frames;
            opstream << " frames - ";
            opstream << avg_channels;
            opstream << " per frame.\n";
            opstream.close();

            frames = 0;
            num_channels = 0;
        }
    */
    if (stereomode == stereoNone) {
        DrawGLScene(stereoCenter);
    } else {
        DrawGLScene(stereoLeft);
        DrawGLScene(stereoRight);
    }
}

// --------------------------------------------------------------------------

void CleanUp(void) {
    LOGFUNC;

    delete[] commandLineOptionsBuffer;
}

#ifndef WIN32
// (code lifted from physfs: http://icculus.org/physfs/ ... zlib license.)
static char* findBinaryInPath(const char* bin, char* envr)
{
    size_t alloc_size = 0;
    char* exe = NULL;
    char* start = envr;
    char* ptr;

    do {
        size_t size;
        ptr = strchr(start, ':'); /* find next $PATH separator. */
        if (ptr) {
            *ptr = '\0';
        }
        size = strlen(start) + strlen(bin) + 2;
        if (size > alloc_size) {
            char* x = (char*)realloc(exe, size);
            if (x == NULL) {
                if (exe != NULL) {
                    free(exe);
                }
                return (NULL);
            } /* if */

            alloc_size = size;
            exe = x;
        } /* if */

        /* build full binary path... */
        strcpy(exe, start);
        if ((exe[0] == '\0') || (exe[strlen(exe) - 1] != '/')) {
            strcat(exe, "/");
        }
        strcat(exe, bin);

        if (access(exe, X_OK) == 0) { /* Exists as executable? We're done. */
            strcpy(exe, start);       /* i'm lazy. piss off. */
            return (exe);
        } /* if */

        start = ptr + 1; /* start points to beginning of next element. */
    } while (ptr != NULL);

    if (exe != NULL) {
        free(exe);
    }

    return (NULL); /* doesn't exist in path. */
} /* findBinaryInPath */

char* calcBaseDir(const char* argv0)
{
    /* If there isn't a path on argv0, then look through the $PATH for it. */
    char* retval;
    char* envr;

    if (strchr(argv0, '/')) {
        retval = strdup(argv0);
        if (retval) {
            *((char*)strrchr(retval, '/')) = '\0';
        }
        return (retval);
    }

    envr = getenv("PATH");
    if (!envr) {
        return NULL;
    }
    envr = strdup(envr);
    if (!envr) {
        return NULL;
    }
    retval = findBinaryInPath(argv0, envr);
    free(envr);
    return (retval);
}

static inline void chdirToAppPath(const char* argv0)
{
    char* dir = calcBaseDir(argv0);
    if (dir) {
#if (defined(__APPLE__) && defined(__MACH__))
        // Chop off /Contents/MacOS if it's at the end of the string, so we
        //  land in the base of the app bundle.
        const size_t len = strlen(dir);
        const char* bundledirs = "/Contents/MacOS";
        const size_t bundledirslen = strlen(bundledirs);
        if (len > bundledirslen) {
            char* ptr = (dir + len) - bundledirslen;
            if (strcasecmp(ptr, bundledirs) == 0)
                *ptr = '\0';
        }
#endif
        errno = 0;
        if (chdir(dir) != 0) {
            printf("Error changing dir to '%s' (%s).\n", dir, strerror(errno));
        }
        free(dir);
    }
}
#endif

const option::Descriptor usage[] =
    {
      { UNKNOWN, 0, "", "", option::Arg::None, "USAGE: lugaru [options]\n\n"
                                               "Options:" },
      { VERSION, 0, "v", "version", option::Arg::None, " -v, --version     Print version and exit." },
      { HELP, 0, "h", "help", option::Arg::None, " -h, --help        Print usage and exit." },
      { FULLSCREEN, 1, "f", "fullscreen", option::Arg::None, " -f, --fullscreen  Start the game in fullscreen mode." },
      { FULLSCREEN, 0, "w", "windowed", option::Arg::None, " -w, --windowed    Start the game in windowed mode (default)." },
      { NOMOUSEGRAB, 1, "", "nomousegrab", option::Arg::None, " --nomousegrab     Disable mousegrab." },
      { NOMOUSEGRAB, 0, "", "mousegrab", option::Arg::None, " --mousegrab       Enable mousegrab (default)." },
      { SOUND, 1, "", "nosound", option::Arg::None, " --nosound         Disable sound." },
      { OPENALINFO, 0, "", "openal-info", option::Arg::None, " --openal-info     Print info about OpenAL at launch." },
      { SHOWRESOLUTIONS, 0, "", "showresolutions", option::Arg::None, " --showresolutions List the resolutions found by SDL at launch." },
      { DEVTOOLS, 0, "d", "devtools", option::Arg::None, " -d, --devtools    Enable dev tools: console, level editor and debug info." },
      { CMD, 0, "c", "command", option::Arg::Optional, " -c, --command    Run this command at game start. May be used to load a map." },
      { 0, 0, 0, 0, 0, 0 }
    };

option::Option commandLineOptions[commandLineOptionsNumber];
option::Option* commandLineOptionsBuffer;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	#if DEBUG
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 1300, 0, 550, 300, true);
	std::cout << "right mouse      : capture game" << std::endl;
	std::cout << "space            : capure ui" << std::endl;
	std::cout << "ESC              : quit" << std::endl;
	std::cout << "v                : toggle vsync" << std::endl;
	std::cout << "alt + enter      : fullscreen" << std::endl;

	int frames = 0;
	float framesTime = 0;
	#endif

	float deltaTime = 0.0f;
	float fixedDeltaTime = 0.0f;
	float physicsElapsedTime = 0.0;
	Application application(deltaTime, fixedDeltaTime);

	HWND hwnd = application.getWindow();
	HDC hdc;

	Clock deltaClock;
	Clock fixedDeltaClock;
////////////////////////////////////////////////////////////////////////////       
	newGame();

	cellophane = 0;
	texdetail = 4;
	slomospeed = 0.25;
	slomofreq = 8012;

	DefaultSettings();

	initGL();
	GLint width = kContextWidth;
	GLint height = kContextHeight;
	gMidPoint.h = width / 2;
	gMidPoint.v = height / 2;
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

	while (application.isRunning()) {

		physicsElapsedTime += deltaTime;
		while (physicsElapsedTime > PHYSICS_STEP) {
			fixedDeltaTime = fixedDeltaClock.resetSec();
			if (fixedDeltaTime > PHYSICS_STEP * 5.0f)
				fixedDeltaTime = PHYSICS_STEP;

			application.fixedUpdate();
			physicsElapsedTime -= PHYSICS_STEP;
		}

		deltah = -Mouse::instance().xPosRelative();
		deltav = -Mouse::instance().yPosRelative();

		// game
		DoUpdate();

		application.update();
		application.render();

		deltaTime = deltaClock.resetSec();

#if DEBUG
		framesTime += deltaTime;
		frames++;
		if (framesTime > 1) {
			_TCHAR fpsText[32];
			_sntprintf(fpsText, 32, "FPS: %d FPS", frames);
			SetWindowText(hwnd, fpsText);
			frames = 0;
			framesTime = 0;
			deltaClock.restart();
		}

#endif

		hdc = GetDC(hwnd);
		SwapBuffers(hdc);
		ReleaseDC(hwnd, hdc);
	}

	deleteGame();       
	CleanUp();

	return 0;
}
