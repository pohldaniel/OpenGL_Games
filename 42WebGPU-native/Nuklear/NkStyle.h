#pragma once

enum theme { THEME_BLACK, THEME_WHITE, THEME_RED, THEME_BLUE, THEME_DARK };

extern "C" {
	void set_style(struct nk_context* ctx, enum theme theme);
}