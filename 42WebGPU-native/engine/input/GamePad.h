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
	bool m_Connected;
	float m_LeftStickX;
	float m_LeftStickY;
	float m_RightStickX;
	float m_RightStickY;
	float m_LeftTrigger;
	float m_RightTrigger;

	static XInputController* s_Players[4];
};

class RawInputController {
public:

	RawInputController::~RawInputController();
	
	void update();

	bool buttonPressed(GamepadButton button) const;
	bool buttonDown(GamepadButton button) const;
	bool buttonUp(GamepadButton button) const;

	float getRightStickX() const;
	float getRightStickY() const;

	float getLeftStickX() const;
	float getLeftStickY() const;


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
	const float DEADZONE = 0.15f;

	static int NextIndex();
	static std::unordered_map<HANDLE, RawInputController*> ConnectedControllers;
};