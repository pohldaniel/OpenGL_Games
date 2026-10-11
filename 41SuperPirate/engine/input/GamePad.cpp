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

	memset(m_pCurrButtonStates, false, 17);
	memset(m_pPrevButtonStates, false, 17);
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

			m_pCurrButtonStates[15] = (state.Gamepad.bLeftTrigger > 0) ? true : false;
			m_pCurrButtonStates[16] = (state.Gamepad.bRightTrigger > 0) ? true : false;
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
//########################################################
RawInput RawInput::s_instance;
std::unordered_map<HANDLE, RawInput*> RawInput::ConnectedControllers;

RawInput& RawInput::instance() {
	return s_instance;
}

RawInput::RawInput() {
	m_PlayerIndex = FindNextFreePlayerIndex();
	m_pCurrButtonStates = m_ButtonStatesA;
	m_pPrevButtonStates = m_ButtonStatesB;
}

void RawInput::Shutdown() {
	auto it = ConnectedControllers.begin();
	while (it != ConnectedControllers.end()) {
		RawInput* pad = it->second;
		it = ConnectedControllers.erase(it);

		if (pad != nullptr) {
			delete pad;
		}
	}
	ConnectedControllers.clear();
}

RawInput* RawInput::GetPlayer(int playerIndex) {
	for (auto& pair : ConnectedControllers) {
		if (pair.second && pair.second->GetPlayerIndex() == playerIndex) {
			return pair.second;
		}
	}
	return nullptr;
}

RawInput::~RawInput() {
	if (!ConnectedControllers.empty()) {
		for (auto it = ConnectedControllers.begin(); it != ConnectedControllers.end();) {
			if (it->second == this) {
				it = ConnectedControllers.erase(it);
				break;
			}else {
				++it;
			}
		}
	}
}

void RawInput::update() {
	bool* pTempGamePadStates = m_pPrevButtonStates;
	m_pPrevButtonStates = m_pCurrButtonStates;
	m_pCurrButtonStates = pTempGamePadStates;
	memcpy(m_pCurrButtonStates, m_pPrevButtonStates, sizeof(m_ButtonStatesA));
}

bool RawInput::buttonDown(GamepadButton button) const {
	return m_pCurrButtonStates[static_cast<int>(button)];
}

bool RawInput::attachRaw(HWND hWnd) {

	RAWINPUTDEVICE rid[2];

	rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[0].usUsage = HID_USAGE_GENERIC_JOYSTICK;
	rid[0].dwFlags = RIDEV_DEVNOTIFY | RIDEV_INPUTSINK;
	rid[0].hwndTarget = hWnd;


	rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[1].usUsage = HID_USAGE_GENERIC_GAMEPAD;
	rid[1].dwFlags = RIDEV_DEVNOTIFY | RIDEV_INPUTSINK;
	rid[1].hwndTarget = hWnd;


	if (RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE)) == FALSE) {
		return false;
	}
	return true;
}

void RawInput::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_INPUT_DEVICE_CHANGE: {
			HANDLE deviceHandle = (HANDLE)lParam;
			if (wParam == GIDC_REMOVAL) {
				auto it = ConnectedControllers.find(deviceHandle);
				if (it != ConnectedControllers.end()) {
					RawInput* padToRemove = it->second;
					delete padToRemove;
				}
			}
			break;
		}
		case WM_INPUT: {
			UINT dwSize = 0;
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
			
			if (dwSize == 0) break;

			BYTE* lpb = new BYTE[dwSize];
			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
				RAWINPUT* raw = (RAWINPUT*)lpb;
				if (raw->header.dwType == RIM_TYPEHID) {
					HANDLE deviceHandle = raw->header.hDevice;
					if (ConnectedControllers.find(deviceHandle) == ConnectedControllers.end()) {
						RawInput* newPad = new RawInput();
						ConnectedControllers[deviceHandle] = newPad;
					}

					ConnectedControllers[deviceHandle]->processInput(raw);
				}
			}

			delete[] lpb;
			break;
		}
	}
}

void RawInput::processInput(RAWINPUT* raw) {
	UINT pcbPreparsedData = 0;
	GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, NULL, &pcbPreparsedData);
	if (pcbPreparsedData == 0) return;

	PHIDP_PREPARSED_DATA pPreparsedData = (PHIDP_PREPARSED_DATA)new BYTE[pcbPreparsedData];
	GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &pcbPreparsedData);

	HIDP_CAPS caps;
	HidP_GetCaps(pPreparsedData, &caps);

	// --- 1. NORMALE BUTTONS AUSLESEN ---
	ULONG usageLength = caps.NumberInputButtonCaps;
	if (usageLength > 0) {
		USAGE* usage = new USAGE[usageLength];
		HidP_GetUsages(HidP_Input, HID_USAGE_PAGE_BUTTON, 0, usage, &usageLength, pPreparsedData, (PCHAR)raw->data.hid.bRawData, raw->data.hid.dwSizeHid);
		for (int i = 5; i < 32; ++i) m_pCurrButtonStates[i] = false;

		for (ULONG i = 0; i < usageLength; i++) {
			USAGE buttonId = usage[i];
			
			if (buttonId == 1) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_Y)] = true;
			if (buttonId == 2) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_B)] = true;
			if (buttonId == 3) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_A)] = true;
			if (buttonId == 4) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_X)] = true;
			if (buttonId == 5) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_LEFT_SHOULDER)] = true;
			if (buttonId == 6) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_RIGHT_SHOULDER)] = true;
			if (buttonId == 7) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_LEFT_TRIGGER)] = true;
			if (buttonId == 8) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_RIGHT_TRIGGER)] = true;
			if (buttonId == 9) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_BACK)] = true;
			if (buttonId == 10) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_START)] = true;
			if (buttonId == 11) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_LEFT_THUMB)] = true;
			if (buttonId == 12) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_RIGHT_THUMB)] = true;
		}
		delete[] usage;
	}
	m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_UP)] = false;
	m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_DOWN)] = false;
	m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_LEFT)] = false;
	m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_RIGHT)] = false;

	m_LeftStickX = 0.0f;
	m_LeftStickY = 0.0f;

	m_RightStickX = 0.0f;
	m_RightStickY = 0.0f;

	USHORT valueCapsLength = caps.NumberInputValueCaps;
	if (valueCapsLength > 0) {
		PHIDP_VALUE_CAPS valueCaps = new HIDP_VALUE_CAPS[valueCapsLength];
		HidP_GetValueCaps(HidP_Input, valueCaps, &valueCapsLength, pPreparsedData);

		for (ULONG i = 0; i < valueCapsLength; i++) {		
			for (ULONG i = 0; i < valueCapsLength; i++) {
				ULONG value = 0;
				HidP_GetUsageValue(HidP_Input, valueCaps[i].UsagePage, 0, valueCaps[i].NotRange.Usage, &value, pPreparsedData, (PCHAR)raw->data.hid.bRawData, raw->data.hid.dwSizeHid);
				
				USAGE axisId = valueCaps[i].NotRange.Usage;
				switch (axisId) {
					case 0x32: {
						float rawNorm = (static_cast<float>(value) - 128.0f) / 127.0f;
						if (abs(rawNorm) < DEADZONE) m_LeftStickX = 0.0f;
						else m_LeftStickX = rawNorm;
						break;
					}case 0x35: {
						float rawNorm = (static_cast<float>(value) - 128.0f) / 127.0f;
						rawNorm = -rawNorm;
						if (abs(rawNorm) < DEADZONE) m_LeftStickY = 0.0f;
						else m_LeftStickY = rawNorm;
						break;
					}case 0x30: {				
						float rawNorm = (static_cast<float>(value) - 128.0f) / 127.0f;
						if (abs(rawNorm) < DEADZONE) m_RightStickX = 0.0f;
						else m_RightStickX = rawNorm;
						break;
					}case 0x31: {					
						float rawNorm = (static_cast<float>(value) - 128.0f) / 127.0f;
						rawNorm = -rawNorm;
						if (abs(rawNorm) < DEADZONE) m_RightStickY = 0.0f;
						else m_RightStickY = rawNorm;
						break;
					}case 0x39:
						if (value == 0)
							m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_UP)] = true;
						if (value == 1)
							m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_UP_RIGHT)] = true;
						if (value == 2)
							m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_RIGHT)] = true;
						if (value == 3)
							m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_DOWN_RIGHT)] = true;
						if (value == 4)
							m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_DOWN)] = true;
						if (value == 5)
							m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_DOWN_LEFT)] = true;
						if (value == 6)
							m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_LEFT)] = true;
						if (value == 7)
							m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_UP_LEFT)] = true;
						break;
				}
			}
		}
		delete[] valueCaps;
	}

	delete[](BYTE*)pPreparsedData;
}

int RawInput::FindNextFreePlayerIndex() {
	if (ConnectedControllers.empty())
		return 0;

	std::set<int> activeIndices;
	for (const auto& pair : ConnectedControllers) {
		if (pair.second != nullptr) {
			activeIndices.insert(pair.second->GetPlayerIndex());
		}
	}
	int freeIndex = 0;
	while (activeIndices.find(freeIndex) != activeIndices.end()) {
		freeIndex++;
	}

	return freeIndex;
}