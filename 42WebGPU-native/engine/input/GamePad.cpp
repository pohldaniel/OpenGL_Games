#include <set>
#include <algorithm>

#include "GamePad.h"

XInputController* XInputController::s_Players[4] = { nullptr, nullptr, nullptr, nullptr };

XInputController::XInputController(){
	m_playerIndex = -1;
	state = {};

	m_pCurrButtonStates = m_buttonStates[0];
	m_pPrevButtonStates = m_buttonStates[1];

	memset(m_pCurrButtonStates, false, 21);
	memset(m_pPrevButtonStates, false, 21);
	m_connected = false;
}

void XInputController::update() {
	
	bool* pTempGamePadStates = m_pPrevButtonStates;
	m_pPrevButtonStates = m_pCurrButtonStates;
	m_pCurrButtonStates = pTempGamePadStates;

	ZeroMemory(&state, sizeof(XINPUT_STATE));
	if (XInputGetState(m_playerIndex, &state) == ERROR_SUCCESS) {
		m_connected = true;
		WORD wButtons = state.Gamepad.wButtons;

		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_A)] = (wButtons & XINPUT_GAMEPAD_A);
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_B)] = (wButtons & XINPUT_GAMEPAD_B);
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_X)] = (wButtons & XINPUT_GAMEPAD_X);
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_Y)] = (wButtons & XINPUT_GAMEPAD_Y);

		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_LEFT_SHOULDER)] = (wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_RIGHT_SHOULDER)] = (wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_LEFT_THUMB)] = (wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_RIGHT_THUMB)] = (wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);

		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_BACK)] = (wButtons & XINPUT_GAMEPAD_BACK);
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_START)] = (wButtons & XINPUT_GAMEPAD_START);

		bool up = (wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
		bool down = (wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
		bool left = (wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
		bool right = (wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;

		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_UP)] = up;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_DOWN)] = down;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_LEFT)] = left;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_RIGHT)] = right;

		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_UP_RIGHT)] = up && right;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_DOWN_RIGHT)] = down && right;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_UP_LEFT)] = up && left;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_DOWN_LEFT)] = down && left;

		float rawLX = static_cast<float>(state.Gamepad.sThumbLX);
		float rawLY = static_cast<float>(state.Gamepad.sThumbLY);
		float rawRX = static_cast<float>(state.Gamepad.sThumbRX);
		float rawRY = static_cast<float>(state.Gamepad.sThumbRY);

		float magnitudeL = std::sqrt(rawLX * rawLX + rawLY * rawLY);
		if (magnitudeL < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			m_rightStickX = 0.0f;
			m_rightStickY = 0.0f;
		}else {
			float normLX = rawLX / magnitudeL;
			float normLY = rawLY / magnitudeL;
			float normalizedMagnitude = (magnitudeL - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) / (32767.0f - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

			normalizedMagnitude = std::min(normalizedMagnitude, 1.0f);

			m_rightStickX = normLX * normalizedMagnitude;
			m_rightStickY = normLY * normalizedMagnitude;
		}

		float magnitudeR = std::sqrt(rawRX * rawRX + rawRY * rawRY);
		if (magnitudeR < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
			m_leftStickX = 0.0f;
			m_leftStickY = 0.0f;
		}else {
			float normRX = rawRX / magnitudeR;
			float normRY = rawRY / magnitudeR;
			float normalizedMagnitude = (magnitudeR - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) / (32767.0f - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

			normalizedMagnitude = std::min(normalizedMagnitude, 1.0f);

			m_leftStickX = normRX * normalizedMagnitude;
			m_leftStickY = normRY * normalizedMagnitude;
		}
		
		m_leftTrigger = (state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? (state.Gamepad.bLeftTrigger / 255.0f) : 0.0f;
		m_rightTrigger = (state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? (state.Gamepad.bRightTrigger / 255.0f) : 0.0f;
		
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_LEFT_TRIGGER)] = (state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_RIGHT_TRIGGER)] = (state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	}else {
		if (m_connected) {
			m_connected = false;
			state = {};
			memset(m_pCurrButtonStates, false, 21);
		}
		
	}
}

void XInputController::setVibration(float leftMotor, float rightMotor) {
	leftMotor = std::clamp(leftMotor, 0.0f, 1.0f);
	rightMotor = std::clamp(rightMotor, 0.0f, 1.0f);

	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

	vibration.wLeftMotorSpeed = static_cast<WORD>(leftMotor * 65535.0f); 
	vibration.wRightMotorSpeed = static_cast<WORD>(rightMotor * 65535.0f);

	XInputSetState(m_playerIndex, &vibration);
}

float XInputController::getRightStickX() const { 
	return m_rightStickX; 
}

float XInputController::getRightStickY() const { 
	return m_rightStickY; 
}

float XInputController::getLeftStickX() const { 
	return m_leftStickX; 
}

float XInputController::getLeftStickY() const { 
	return m_leftStickY; 
}

float XInputController::getLeftTrigger() const { 
	return m_leftTrigger; 
}

float XInputController::getRightTrigger() const { 
	return m_rightTrigger; 
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

bool XInputController::isConnected() {
	return m_connected;
}

IController* XInputController::GetPlayer(int playerIndex) {
	if (playerIndex >= 0 && playerIndex < 4) return s_Players[playerIndex];
	return nullptr;
}

void XInputController::Initialize() {
	for (int i = 0; i < 4; ++i) {
		s_Players[i] = new XInputController();
		s_Players[i]->m_playerIndex = i;
	}
}

void XInputController::Update() {
	for (int i = 0; i < 4; ++i) {
		IController* player = XInputController::GetPlayer(i);
		if (player) {
			player->update();
		}
	}
}

void XInputController::Shutdown() {
	for (int i = 0; i < 4; ++i) {
		if (s_Players[i] != nullptr) {
			if (s_Players[i]->m_connected) {
				XINPUT_VIBRATION stopVibration = { 0, 0 };
				XInputSetState(static_cast<DWORD>(i), &stopVibration);
			}
			delete s_Players[i];
			s_Players[i] = nullptr;
		}
	}
}
//########################################################

uint32_t calculate_crc32(const uint8_t* data, size_t len) {
	uint32_t crc = 0xFFFFFFFF;
	for (size_t i = 0; i < len; ++i) {
		crc ^= data[i];
		for (int j = 0; j < 8; ++j) {
			if (crc & 1) crc = (crc >> 1) ^ 0xEDB88320;
			else crc >>= 1;
		}
	}
	return ~crc;
}

std::unordered_map<HANDLE, RawInputController*> RawInputController::ConnectedControllers;

RawInputController::RawInputController() {
	m_playerIndex = NextIndex();
	m_pCurrButtonStates = m_buttonStates[0];
	m_pPrevButtonStates = m_buttonStates[1];

	memset(m_pCurrButtonStates, false, 21);
	memset(m_pPrevButtonStates, false, 21);
}

RawInputController::~RawInputController() {
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
	if (m_writeHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(m_writeHandle);
		m_writeHandle = INVALID_HANDLE_VALUE;
	}
}

void RawInputController::update() {
	bool* pTempGamePadStates = m_pPrevButtonStates;
	m_pPrevButtonStates = m_pCurrButtonStates;
	m_pCurrButtonStates = pTempGamePadStates;
	memcpy(m_pCurrButtonStates, m_pPrevButtonStates, 21);
}

bool RawInputController::buttonPressed(GamepadButton button) const {
	return m_pCurrButtonStates[button] && !m_pPrevButtonStates[button];
}

bool RawInputController::buttonDown(GamepadButton button) const {
	return m_pCurrButtonStates[static_cast<int>(button)];
}

bool RawInputController::buttonUp(GamepadButton button) const {
	return !m_pCurrButtonStates[button];
}

bool RawInputController::isConnected() {
	return true;
}

void RawInputController::processInput(RAWINPUT* raw) {
	
	UINT pcbPreparsedData = 0;
	GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, NULL, &pcbPreparsedData);
	if (pcbPreparsedData == 0) return;

	PHIDP_PREPARSED_DATA pPreparsedData = (PHIDP_PREPARSED_DATA)new BYTE[pcbPreparsedData];
	GetRawInputDeviceInfo(raw->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &pcbPreparsedData);
	
	if (m_isBluetooth) {
		memset(m_pCurrButtonStates, false, 21);

		BYTE* data = raw->data.hid.bRawData;
		ULONG dataSize = raw->data.hid.dwSizeHid;

		int hasReportId = (data[0] == 0x01 || data[0] == 0x31) ? 1 : 0;

		BYTE rightX = data[1 + hasReportId];
		BYTE rightY = data[2 + hasReportId];
		BYTE leftX = data[3 + hasReportId];
		BYTE leftY = data[4 + hasReportId];
		BYTE leftTrigger = data[5 + hasReportId];
		BYTE rightTrigger = data[6 + hasReportId];

		float rawLeftX = (static_cast<float>(leftX) - 128.0f) / 127.0f;
		float rawLeftY = -(static_cast<float>(leftY) - 128.0f) / 127.0f;
		float rawRightX = (static_cast<float>(rightX) - 128.0f) / 127.0f;
		float rawRightY = -(static_cast<float>(rightY) - 128.0f) / 127.0f;

		m_leftStickX = (abs(rawLeftX) < DEADZONE) ? 0.0f : rawLeftX;
		m_leftStickY = (abs(rawLeftY) < DEADZONE) ? 0.0f : rawLeftY;
		m_rightStickX = (abs(rawRightX) < DEADZONE) ? 0.0f : rawRightX;
		m_rightStickY = (abs(rawRightY) < DEADZONE) ? 0.0f : rawRightY;
		m_leftTrigger = static_cast<float>(leftTrigger) / 255.0f;
		m_rightTrigger = static_cast<float>(rightTrigger) / 255.0f;

		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_LEFT_TRIGGER)] = m_leftTrigger > 0.5f;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_RIGHT_TRIGGER)] = m_rightTrigger > 0.5f;

		BYTE byte8 = data[8 + hasReportId];
		BYTE byte9 = data[9 + hasReportId];

		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_X)] = (byte8 & 0x10) != 0;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_A)] = (byte8 & 0x20) != 0;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_B)] = (byte8 & 0x40) != 0;		
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_Y)] = (byte8 & 0x80) != 0;
		
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_LEFT_SHOULDER)] = (byte9 & 0x01) != 0;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_RIGHT_SHOULDER)] = (byte9 & 0x02) != 0;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_LEFT_TRIGGER)] = (byte9 & 0x04) != 0;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_RIGHT_TRIGGER)] = (byte9 & 0x08) != 0;

		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_BACK)] = (byte9 & 0x10) != 0;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_START)] = (byte9 & 0x20) != 0;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_LEFT_THUMB)] = (byte9 & 0x40) != 0;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_RIGHT_THUMB)] = (byte9 & 0x80) != 0;


		BYTE dpadRaw = byte8 & 0x0F;
		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_UP)] = (dpadRaw == 0);
		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_RIGHT)] = (dpadRaw == 2);
		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_DOWN)] = (dpadRaw == 4);
		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_LEFT)] = (dpadRaw == 6);

		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_UP_RIGHT)] = (dpadRaw == 1);
		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_DOWN_RIGHT)] = (dpadRaw == 3);
		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_UP_LEFT)] = (dpadRaw == 7);
		m_pCurrButtonStates[static_cast<int>(GamepadButton::DPAD_DOWN_LEFT)] = (dpadRaw == 5);
	}else {


		HIDP_CAPS caps;
		NTSTATUS status = HidP_GetCaps(pPreparsedData, &caps);
		ULONG usageLength = caps.NumberInputButtonCaps;
		if (usageLength > 0) {
			USAGE* usage = new USAGE[usageLength];
			status = HidP_GetUsages(HidP_Input, HID_USAGE_PAGE_BUTTON, 0, usage, &usageLength, pPreparsedData, (PCHAR)raw->data.hid.bRawData, raw->data.hid.dwSizeHid);
			memset(m_pCurrButtonStates, false, 21);

			for (ULONG i = 0; i < usageLength; i++) {
				USAGE buttonId = usage[i];
				if (buttonId == 1) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_X)] = true;
				if (buttonId == 2) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_A)] = true;
				if (buttonId == 3) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_B)] = true;
				if (buttonId == 4) m_pCurrButtonStates[static_cast<int>(GamepadButton::GAMEPAD_Y)] = true;
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

		m_leftStickX = 0.0f;
		m_leftStickY = 0.0f;

		m_rightStickX = 0.0f;
		m_rightStickY = 0.0f;

		USHORT valueCapsLength = caps.NumberInputValueCaps;
		if (valueCapsLength > 0) {
			PHIDP_VALUE_CAPS valueCaps = new HIDP_VALUE_CAPS[valueCapsLength];
			status = HidP_GetValueCaps(HidP_Input, valueCaps, &valueCapsLength, pPreparsedData);

			float rawLeftX = 0.0f;
			float rawLeftY = 0.0f;
			float rawRightX = 0.0f;
			float rawRightY = 0.0f;

			for (ULONG i = 0; i < valueCapsLength; i++) {
				for (ULONG i = 0; i < valueCapsLength; i++) {
					ULONG value = 0;
					status = HidP_GetUsageValue(HidP_Input, valueCaps[i].UsagePage, 0, valueCaps[i].NotRange.Usage, &value, pPreparsedData, (PCHAR)raw->data.hid.bRawData, raw->data.hid.dwSizeHid);
					
					USAGE axisId = valueCaps[i].NotRange.Usage;
					switch (axisId) {
					case 0x32: {
						rawLeftX = (static_cast<float>(value) - 128.0f) / 127.0f;
						break;
					}case 0x35: {
						rawLeftY = -((static_cast<float>(value) - 128.0f) / 127.0f);
						break;
					}case 0x30: {
						rawRightX = (static_cast<float>(value) - 128.0f) / 127.0f;
						break;
					}case 0x31: {
						rawRightY = -((static_cast<float>(value) - 128.0f) / 127.0f);
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

			if (rawLeftX || rawLeftY) {
				float magnitudeL = std::sqrt(rawLeftX * rawLeftX + rawLeftY * rawLeftY);

				if (magnitudeL < DEADZONE) {
					m_leftStickX = 0.0f;
					m_leftStickY = 0.0f;
				}else {
					float normX = rawLeftX / magnitudeL;
					float normY = rawLeftY / magnitudeL;
					float normalizedMagnitude = (magnitudeL - DEADZONE) / (1.0f - DEADZONE);
					normalizedMagnitude = std::min(normalizedMagnitude, 1.0f);

					m_leftStickX = normX * normalizedMagnitude;
					m_leftStickY = normY * normalizedMagnitude;
				}
			}

			if (rawRightX || rawRightY) {
				float magnitudeR = std::sqrt(rawRightX * rawRightX + rawRightY * rawRightY);

				if (magnitudeR < DEADZONE) {
					m_rightStickX = 0.0f;
					m_rightStickY = 0.0f;
				}else {
					float normX = rawRightX / magnitudeR;
					float normY = rawRightY / magnitudeR;
					float normalizedMagnitude = (magnitudeR - DEADZONE) / (1.0f - DEADZONE);
					normalizedMagnitude = std::min(normalizedMagnitude, 1.0f);

					m_rightStickX = normX * normalizedMagnitude;
					m_rightStickY = normY * normalizedMagnitude;
				}
			}
		}

		delete[](BYTE*)pPreparsedData;
	}
}

float RawInputController::getRightStickX() const {
	return m_rightStickX; 
}

float RawInputController::getRightStickY() const { 
	return m_rightStickY; 
}

float RawInputController::getLeftStickX() const {
	return m_leftStickX; 
}

float RawInputController::getLeftStickY() const {
	return m_leftStickY;
}

float RawInputController::getLeftTrigger() const {
	return m_leftTrigger;
}

float RawInputController::getRightTrigger() const {
	return m_rightTrigger;
}

int RawInputController::getPlayerIndex() const { 
	return m_playerIndex; 
}

void RawInputController::setWriteHandle(HANDLE writeHandle) {
	m_writeHandle = writeHandle;
}

void RawInputController::setIsBluetooth(bool isBluetooth) {
	m_isBluetooth = isBluetooth;
}

void RawInputController::setVibration(float leftMotor, float rightMotor) {
	if (m_writeHandle == INVALID_HANDLE_VALUE) return;
	
	leftMotor = std::clamp(leftMotor, 0.0f, 1.0f);
	rightMotor = std::clamp(rightMotor, 0.0f, 1.0f);

	BYTE leftMotorByte = static_cast<BYTE>(leftMotor * 255.0f);
	BYTE rightMotorByte = static_cast<BYTE>(rightMotor * 255.0f);

	DWORD bytesWritten = 0;
	if (m_isBluetooth) {	
		uint8_t report[78] = { 0 };
		report[0] = 0x31;
		report[1] = 0x00;
		report[2] = 0x10;
		report[3] = 0x03;
		report[4] = 0x00;
		report[5] = rightMotorByte;
		report[6] = leftMotorByte;
		uint8_t crc_buffer[75];
		crc_buffer[0] = 0xA2;
		memcpy(&crc_buffer[1], report, 74);

		uint32_t crc = calculate_crc32(crc_buffer, 75);
		report[74] = (crc >> 0) & 0xFF;
		report[75] = (crc >> 8) & 0xFF;
		report[76] = (crc >> 16) & 0xFF;
		report[77] = (crc >> 24) & 0xFF;
		BOOL success = WriteFile(m_writeHandle, report, sizeof(report), &bytesWritten, NULL);	
	}else{
		BYTE buffer[48] = { 0 };

		buffer[0] = 0x02;
		buffer[1] = 0xFF;
		buffer[2] = 0x00;
		buffer[3] = rightMotorByte;
		buffer[4] = leftMotorByte;
		
		WriteFile(m_writeHandle, buffer, sizeof(buffer), &bytesWritten, NULL);
	}
}

void RawInputController::Update() {
	for (int i = 0; i < 4; ++i) {
		IController* controller = RawInputController::GetPlayer(i);
		if (controller) {
			controller->update();
		}
	}
}

bool RawInputController::Initialize(HWND hWnd) {

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

void RawInputController::Shutdown() {
	if (ConnectedControllers.empty())
		return;

	auto it = ConnectedControllers.begin();
	while (it != ConnectedControllers.end()) {
		RawInputController* controller = it->second;
		it = ConnectedControllers.erase(it);

		if (controller != nullptr) {
			delete controller;
		}
	}
	ConnectedControllers.clear();
}

IController* RawInputController::GetPlayer(int playerIndex) {
	for (auto& pair : ConnectedControllers) {
		if (pair.second && pair.second->getPlayerIndex() == playerIndex) {
			return pair.second;
		}
	}
	return nullptr;
}

void RawInputController::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_INPUT_DEVICE_CHANGE: {
			HANDLE deviceHandle = (HANDLE)lParam;
			if (wParam == GIDC_REMOVAL) {
				auto it = ConnectedControllers.find(deviceHandle);
				if (it != ConnectedControllers.end()) {
					RawInputController* controller = it->second;
					delete controller;
				}
			}
			break;
		}case WM_INPUT: {
			UINT dwSize = 0;
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

			if (dwSize == 0) break;

			BYTE* lpb = new BYTE[dwSize];
			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
				RAWINPUT* raw = (RAWINPUT*)lpb;
				if (raw->header.dwType == RIM_TYPEHID) {
					HANDLE deviceHandle = raw->header.hDevice;
					if (ConnectedControllers.find(deviceHandle) == ConnectedControllers.end()) {						
						RID_DEVICE_INFO deviceInfo = { 0 };
						deviceInfo.cbSize = sizeof(RID_DEVICE_INFO);
						UINT infoSize = deviceInfo.cbSize;
						if (GetRawInputDeviceInfoW(deviceHandle, RIDI_DEVICEINFO, &deviceInfo, &infoSize) != (UINT)-1) {
							if (deviceInfo.dwType == RIM_TYPEHID) {

								if (deviceInfo.hid.usUsagePage == 0x01 && (deviceInfo.hid.usUsage == 0x05 || deviceInfo.hid.usUsage == 0x04)) {

									USHORT vid = deviceInfo.hid.dwVendorId;
									USHORT pid = deviceInfo.hid.dwProductId;
									bool isXInput = false;
									if (vid == 0x045E) {
										return;
									}

									UINT nameSize = 0;
									GetRawInputDeviceInfoW(deviceHandle, RIDI_DEVICENAME, NULL, &nameSize);
									if (nameSize > 0) {
										wchar_t* deviceName = new wchar_t[nameSize];
										if (GetRawInputDeviceInfoW(deviceHandle, RIDI_DEVICENAME, deviceName, &nameSize) != (UINT)-1) {
											if (wcsstr(deviceName, L"&IG_") != nullptr || wcsstr(deviceName, L"&ig_") != nullptr) {
												return;
											}
											
											bool isBluetooth = false;
											if (wcsstr(deviceName, L"{00001124-") != nullptr) {
												isBluetooth = true;
											}else if (wcsstr(deviceName, L"#BTH#") != nullptr || wcsstr(deviceName, L"#bth#") != nullptr) {
												isBluetooth = true;
											}else {
												isBluetooth = false;
											}

											RawInputController* controller = new RawInputController();
											ConnectedControllers[deviceHandle] = controller;
											HANDLE writeHandle = CreateFileW(
												deviceName,
												GENERIC_WRITE | GENERIC_READ,
												FILE_SHARE_READ | FILE_SHARE_WRITE,
												NULL,
												OPEN_EXISTING,
												0,
												NULL
											);

											if (writeHandle != INVALID_HANDLE_VALUE) {
												controller->setWriteHandle(writeHandle);
												controller->setIsBluetooth(isBluetooth);
												uint8_t feature_buf[42] = { 0 };
												feature_buf[0] = 0x05;									
												BOOLEAN success = HidD_GetFeature(writeHandle, feature_buf, sizeof(feature_buf));	
											}
										}
										
									}
								}
							}
						}
					}
					ConnectedControllers[deviceHandle]->processInput(raw);
				}
			}
			delete[] lpb;
			break;
		}
	}
}

int RawInputController::NextIndex() {
	if (ConnectedControllers.empty())
		return 0;

	std::set<int> activeIndices;
	for (const auto& pair : ConnectedControllers) {
		if (pair.second != nullptr) {
			activeIndices.insert(pair.second->getPlayerIndex());
		}
	}
	int freeIndex = 0;
	while (activeIndices.find(freeIndex) != activeIndices.end()) {
		freeIndex++;
	}

	return freeIndex;
}