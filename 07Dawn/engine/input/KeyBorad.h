#pragma once
#include <windows.h>

class Keyboard{
public:
	enum Key{
		KEY_0 = 48,
		KEY_1 = 49,
		KEY_2 = 50,
		KEY_3 = 51,
		KEY_4 = 52,
		KEY_5 = 53,
		KEY_6 = 54,
		KEY_7 = 55,
		KEY_8 = 56,
		KEY_9 = 57,

		KEY_A = 65,
		KEY_B = 66,
		KEY_C = 67,
		KEY_D = 68,
		KEY_E = 69,
		KEY_F = 70,
		KEY_G = 71,
		KEY_H = 72,
		KEY_I = 73,
		KEY_J = 74,
		KEY_K = 75,
		KEY_L = 76,
		KEY_M = 77,
		KEY_N = 78,
		KEY_O = 79,
		KEY_P = 80,
		KEY_Q = 81,
		KEY_R = 82,
		KEY_S = 83,
		KEY_T = 84,
		KEY_U = 85,
		KEY_V = 86,
		KEY_W = 87,
		KEY_X = 88,
		KEY_Y = 89,
		KEY_Z = 90,

		KEY_F1 = 112,
		KEY_F2 = 113,
		KEY_F3 = 114,
		KEY_F4 = 115,
		KEY_F5 = 116,
		KEY_F6 = 117,
		KEY_F7 = 118,
		KEY_F8 = 119,
		KEY_F9 = 120,
		KEY_F10 = 121,
		KEY_F11 = 122,
		KEY_F12 = 123,

		KEY_BACKSPACE = 8,
		KEY_TAB = 9,
		KEY_ENTER = 13,
		KEY_CAPSLOCK = 20,
		KEY_ESCAPE = 27,
		KEY_SPACE = 32,
		KEY_APP = 93,
		KEY_SEMICOLON = 186,
		KEY_ADD = 187,
		KEY_COMMA = 188,
		KEY_SUBTRACT = 189,
		KEY_PERIOD = 190,
		KEY_FWDSLASH = 191,
		KEY_TILDE = 192,
		KEY_LBRACKET = 219,
		KEY_BACKSLASH = 220,
		KEY_RBRACKET = 221,
		KEY_APOSTROPHE = 222,

		
		KEY_SHIFT = 16,
		KEY_CTRL = 17,
		KEY_LWIN = 91,
		KEY_RWIN = 92,
		KEY_LSHIFT = 160, /*VK_LSHIFT*/
		KEY_RSHIFT = 161, /*VK_RSHIFT*/
		KEY_LCTRL = 162,  /*VK_LCONTROL*/
		KEY_RCTRL = 163,  /*VK_RCONTROL*/
		KEY_LALT = 164,
		KEY_RALT = 165,

		KEY_INSERT = 45,
		KEY_DELETE = 46,
		KEY_HOME = 36,
		KEY_END = 35,
		KEY_PAGEUP = 33,
		KEY_PAGEDOWN = 34,

		KEY_UP = 38,
		KEY_DOWN = 40,
		KEY_RIGHT = 39,
		KEY_LEFT = 37,

		KEY_NUMLOCK = 144,
		KEY_NUMPAD_DIVIDE = 111,
		KEY_NUMPAD_MULTIPLY = 106,
		KEY_NUMPAD_SUBTRACT = 109,
		KEY_NUMPAD_ADD = 107,
		KEY_NUMPAD_DECIMAL = 110,
		KEY_NUMPAD_0 = 96,
		KEY_NUMPAD_1 = 97,
		KEY_NUMPAD_2 = 98,
		KEY_NUMPAD_3 = 99,
		KEY_NUMPAD_4 = 100,
		KEY_NUMPAD_5 = 101,
		KEY_NUMPAD_6 = 102,
		KEY_NUMPAD_7 = 103,
		KEY_NUMPAD_8 = 104,
		KEY_NUMPAD_9 = 105,
	};

	static Keyboard &instance();

	int getLastChar() const{
		return m_lastChar;
	}

	bool keyDown(Key key) const{
		return (m_pCurrKeyStates[key] & 0x80) ? true : false;
	}

	bool keyUp(Key key) const{
		return (m_pCurrKeyStates[key] & 0x80) ? false : true;
	}

	bool keyPressed(Key key) const{
		return ((m_pCurrKeyStates[key] & 0x80)
			&& !(m_pPrevKeyStates[key] & 0x80)) ? true : false;
	}

	void handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void update();
	void disable();
	void enable();

private:
	Keyboard();
	~Keyboard();

	int m_lastChar;
	BYTE m_keyStates[2][256];
	BYTE *m_pCurrKeyStates;
	BYTE *m_pPrevKeyStates;
	bool m_disabled = false;
};
