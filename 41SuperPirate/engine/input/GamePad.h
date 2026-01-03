#pragma once
#include <windows.h>
#include <XInput.h>

class XInputController {
private:
	int n;
	XINPUT_STATE state;

public:

	static XInputController& instance();
	static XInputController s_instance;


	XInputController(int num);

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
};