#pragma once
#include <windows.h>
#include <XInput.h>

class XInputController {

public:

	enum GamepadButton {
		NONE = 0,
		DPAD_UP = 1,
		DPAD_DOWN = 2,
		DPAD_LEFT = 3,
		DPAD_RIGHT = 4,

		GAMEPAD_START = 5,
		GAMEPAD_BACK = 6,
		GAMEPAD_LEFT_THUMB = 7,
		GAMEPAD_RIGHT_THUMB = 8,

		GAMEPAD_LEFT_SHOULDER = 9,
		GAMEPAD_RIGHT_SHOULDER = 10,
		GAMEPAD_A = 11,
		GAMEPAD_B = 12,
		GAMEPAD_X = 13,
		GAMEPAD_Y = 14,
		GAMEPAD_LEFT_TRIGGER = 15,
		GAMEPAD_RIGHT_TRIGGER = 16,
	};

	static XInputController& instance();
	static XInputController s_instance;

	void update();

	XINPUT_STATE GetState()
	{
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		XInputGetState(n, &state);
		return state;
	}

	bool IsConnected()
	{
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		DWORD statenow = XInputGetState(n, &state);

		if (statenow == ERROR_SUCCESS) return true;

		return false;
	}

	void vibrate(int LV = 0, int RV = 0)
	{
		XINPUT_VIBRATION vibration;
		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

		vibration.wLeftMotorSpeed = LV;
		vibration.wRightMotorSpeed = RV;

		XInputSetState(n, &vibration);
	}
	bool buttonPressed(GamepadButton button) const;
	bool buttonDown(GamepadButton button) const;
	bool buttonUp(GamepadButton button) const;

private:
	XInputController(int num);

	bool m_buttonStates[2][17];
	bool* m_pCurrButtonStates;
	bool* m_pPrevButtonStates;
	int n;
	XINPUT_STATE state;
};