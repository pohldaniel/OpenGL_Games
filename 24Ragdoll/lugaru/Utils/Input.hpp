#ifndef _INPUT_HPP_
#define _INPUT_HPP_


#include "..\..\engine\input\Keyboard.h"
#include "..\..\engine\input\Mouse.h"

class Input {

public:
    static bool isKeyDown(Keyboard::Key key);
    static bool isKeyPressed(Keyboard::Key key);
    static bool MouseClicked();
	static bool isMousePressed(Mouse::MouseButton button);
	static bool isMouseDown(Mouse::MouseButton button);

	static std::string GetCharsFromKeys(unsigned short key, bool shift = false, bool altGr = false);
};

#endif
