#pragma once

enum CursorType {
	ROTATION = 0,
	ARROW = 1,
	NO = 2,
	ACTIVATE = 3,
	DESACTIVATE = 4
};

namespace evnt {
	struct ChangeCursor {
		ChangeCursor(CursorType cursor) : cursor(cursor) {}

		CursorType cursor;
	};
}
