#pragma once
enum GameState {
	MAIN_MENU,
	LOADING,
	INGAME
};

enum DisplayMode {
	WINDOWED,
	BORDERLESS,
	FULLSCREEN
};

enum WindowProgressState {
	NONE,
	INDETERMINATE,
	NORMAL,
	PAUSED,
	_ERROR
};

enum WindowFlashAction {
	START,
	START_COUNT,
	START_UNTIL_FG,
	STOP
};
