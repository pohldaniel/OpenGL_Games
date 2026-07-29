#pragma once

enum theme { THEME_BLACK, THEME_WHITE, THEME_RED, THEME_BLUE, THEME_DARK };

extern "C" {
	void set_style(enum theme theme);
	void set_transparent_window_style();
	void reset_transparent_window_style();
}