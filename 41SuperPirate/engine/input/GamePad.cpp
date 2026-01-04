#include <iostream>
#include "GamePad.h"

XInputController XInputController::s_instance(0);

XInputController& XInputController::instance() {
	return s_instance;
}

XInputController::XInputController(int num){
	n = num;
	state = {};
}

void XInputController::update() {
	XInputController& gamepad = XInputController::instance();

	if (gamepad.IsConnected()){
		if (gamepad.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A){
			std::cout << "A" << std::endl;
			gamepad.vibrate(65535, 0);
		}

		if (gamepad.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B){
			std::cout << "B" << std::endl;
			gamepad.vibrate(0, 65535);
		}

		if (gamepad.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X){
			std::cout << "X" << std::endl;
			gamepad.vibrate(65535, 65535);
		}

		if (gamepad.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y){
			std::cout << "Y" << std::endl;
			gamepad.vibrate();
		}

		if (gamepad.GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK){
			std::cout << "EXIT" << std::endl;
			gamepad.vibrate();
		}
	}
}