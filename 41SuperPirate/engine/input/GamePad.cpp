#include <iostream>
#include "GamePad.h"

XInputController XInputController::s_instance(0);

XInputController& XInputController::instance() {
	return s_instance;
}

XInputController::XInputController(int num){
	n = num;
	state = {};
	m_pCurrButtonStates = m_buttonStates[0];
	m_pPrevButtonStates = m_buttonStates[1];

	memset(m_pCurrButtonStates, false, 15);
	memset(m_pPrevButtonStates, false, 15);
}

void XInputController::update() {
	XInputController& gamepad = XInputController::instance();

	if (gamepad.IsConnected()){
		bool* pTempGamePadStates = m_pPrevButtonStates;

		m_pPrevButtonStates = m_pCurrButtonStates;
		m_pCurrButtonStates = pTempGamePadStates;

		ZeroMemory(&state, sizeof(XINPUT_STATE));
		DWORD result = XInputGetState(n, &state);
		if (result == ERROR_SUCCESS && state.dwPacketNumber){

			m_pCurrButtonStates[1] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? true : false;
			m_pCurrButtonStates[2] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? true : false;
			m_pCurrButtonStates[3] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? true : false;
			m_pCurrButtonStates[4] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? true : false;

			m_pCurrButtonStates[5] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) ? true : false;
			m_pCurrButtonStates[6] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ? true : false;

			m_pCurrButtonStates[7] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? true : false;
			m_pCurrButtonStates[8] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? true : false;

			m_pCurrButtonStates[9] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? true : false;
			m_pCurrButtonStates[10] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? true : false;

			m_pCurrButtonStates[11] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) ? true : false;
			m_pCurrButtonStates[12] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) ? true : false;
			m_pCurrButtonStates[13] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) ? true : false;
			m_pCurrButtonStates[14] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ? true : false;			
		}
	}	
}

bool XInputController::buttonPressed(GamepadButton button) const {
	return m_pCurrButtonStates[button] && !m_pPrevButtonStates[button];
}

bool XInputController::buttonDown(GamepadButton button) const {
	return m_pCurrButtonStates[button];
}

bool XInputController::buttonUp(GamepadButton button) const {
	return !m_pCurrButtonStates[button];
}