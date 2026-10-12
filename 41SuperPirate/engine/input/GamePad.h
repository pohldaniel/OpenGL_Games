#pragma once
#include <windows.h>
#include <hidusage.h>
#include <hidsdi.h>

#include <XInput.h>
#include <unordered_map>

enum GamepadButton {
	NONE = 0,
	DPAD_UP = 1,
	DPAD_DOWN = 2,
	DPAD_LEFT = 3,
	DPAD_RIGHT = 4,
	DPAD_UP_RIGHT = 5,
	DPAD_DOWN_RIGHT = 6,
	DPAD_UP_LEFT = 7,
	DPAD_DOWN_LEFT = 8,
	GAMEPAD_START = 9,
	GAMEPAD_BACK = 10,
	GAMEPAD_LEFT_THUMB = 11,
	GAMEPAD_RIGHT_THUMB = 12,

	GAMEPAD_LEFT_SHOULDER = 13,
	GAMEPAD_RIGHT_SHOULDER = 14,
	GAMEPAD_A = 15,
	GAMEPAD_B = 16,
	GAMEPAD_X = 17,
	GAMEPAD_Y = 18,
	GAMEPAD_LEFT_TRIGGER = 19,
	GAMEPAD_RIGHT_TRIGGER = 20,
};

class XInputController {

public:

	void update();
	void setVibration(float leftMotor, float rightMotor);
	float getRightStickX() const;
	float getRightStickY() const;
	float getLeftStickX() const;
	float getLeftStickY() const;
	float getLeftTrigger() const;
	float getRightTrigger() const;

	bool buttonPressed(GamepadButton button) const;
	bool buttonDown(GamepadButton button) const;
	bool buttonUp(GamepadButton button) const;
	bool isConnected();

	static XInputController* GetPlayer(int playerIndex);
	static void Update();
	static void Initialize();
	static void Shutdown();

private:

	XInputController();
	int m_PlayerIndex;
	bool m_buttonStates[2][21];
	bool* m_pCurrButtonStates;
	bool* m_pPrevButtonStates;
	
	XINPUT_STATE state;
	bool m_connected;
	float m_LeftStickX ;
	float m_LeftStickY;
	float m_RightStickX;
	float m_RightStickY;
	float m_LeftTrigger;
	float m_RightTrigger;

	static XInputController* s_Players[4];
};

#pragma pack(push, 1) // Verhindert, dass der Compiler unsichtbare Lücken (Padding) einbaut
struct DualSenseBTHidapiReport {
	uint8_t reportId;    // 0x11
	uint8_t btHeader;    // 0x02 (Manchmal auch 0x80 je nach Firmware)
	uint8_t reserved;    // 0x00 -> WICHTIG: Dieses Byte muss hier sitzen!
	uint8_t flags;       // 0x03 -> Aktiviert Motoren (Bit 0 = Enable, Bit 1 = Enable)
	uint8_t reserved2[2]; // Weitere 2 Byte Platzhalter im DualSense-Protokoll

	uint8_t motorRight;  // Schwacher / hochfrequenter Motor
	uint8_t motorLeft;   // Starker / niederfrequenter Motor

	uint8_t padding[63]; // Auffüllen auf die vom Treiber erwartete Report-Länge
	uint32_t crc32;      // Die letzten 4 Bytes für die CRC
};
#pragma pack(pop)
class RawInputController {
	
public:

	RawInputController::~RawInputController();
	
	void update();
	void setVibration(float leftMotor, float rightMotor);
	bool buttonPressed(GamepadButton button) const;
	bool buttonDown(GamepadButton button) const;
	bool buttonUp(GamepadButton button) const;

	float getRightStickX() const;
	float getRightStickY() const;

	float getLeftStickX() const;
	float getLeftStickY() const;
	void setWriteHandle(HANDLE writeHandle);
	void setIsBluetooth(bool isBluetooth);

	static void Update();
	static bool Initialize(HWND hWnd);
	static void Shutdown();
	static RawInputController* GetPlayer(int playerIndex);
	static void HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:

	RawInputController();
	void processInput(RAWINPUT* raw);
	int getPlayerIndex() const;

	int m_PlayerIndex;
	bool m_buttonStates[2][21];
	bool* m_pCurrButtonStates;
	bool* m_pPrevButtonStates;
	float m_RightStickX = 0.0f;
	float m_RightStickY = 0.0f;
	float m_LeftStickX = 0.0f;
	float m_LeftStickY = 0.0f;
	float m_LeftTrigger = 0.0f;
	float m_RightTrigger = 0.0f;
	bool m_isBluetooth;
	const float DEADZONE = 0.15f;

	HANDLE m_writeHandle = INVALID_HANDLE_VALUE;

	static int NextIndex();
	static std::unordered_map<HANDLE, RawInputController*> ConnectedControllers;
};