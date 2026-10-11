#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <hidusage.h>
#include <hidsdi.h>

#include <XInput.h>
#include <unordered_map>
#include <set>

class XInputController {

public:

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

	static XInputController& instance();
	static XInputController s_instance;

	void update();
	void setVibration(float leftMotor, float rightMotor);
	float getRightStickX() const { return m_RightStickX; }
	float getRightStickY() const { return m_RightStickY; }

	float getLeftStickX() const { return m_LeftStickX; }
	float getLeftStickY() const { return m_LeftStickY; }

	float getLeftTrigger() const { return m_LeftTrigger; }
	float getRightTrigger() const { return m_RightTrigger; }


	bool buttonPressed(GamepadButton button) const;
	bool buttonDown(GamepadButton button) const;
	bool buttonUp(GamepadButton button) const;
	void ResetState();
	XINPUT_STATE GetState();
	bool IsConnected();
	static XInputController* GetPlayer(int playerIndex);
	static void UpdateAll();
	static void InitializePlayers();
	static void Shutdown();

private:

	XInputController();
	int m_PlayerIndex;
	bool m_buttonStates[2][17];
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

class RawInput {	
public:

	enum class GamepadButton {
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

	RawInput::~RawInput();
	bool attachRaw(HWND hWnd);
	void detachRaw();
	void update();
	int GetPlayerIndex() const { return m_PlayerIndex; }
	bool buttonDown(GamepadButton button) const;

	float getRightStickX() const { return m_RightStickX; }
	float getRightStickY() const { return m_RightStickY; }

	float getLeftStickX() const { return m_LeftStickX; }
	float getLeftStickY() const { return m_LeftStickY; }

	static RawInput& instance();
	static void HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void Shutdown();
	static RawInput* GetPlayer(int playerIndex);

private:

	RawInput();
	void processInput(RAWINPUT* raw);

	int m_PlayerIndex;

	bool m_ButtonStatesA[32] = { false };
	bool m_ButtonStatesB[32] = { false };
	bool* m_pCurrButtonStates;
	bool* m_pPrevButtonStates;
	float m_RightStickX = 0.0f;
	float m_RightStickY = 0.0f;
	float m_LeftStickX = 0.0f;
	float m_LeftStickY = 0.0f;
	const float DEADZONE = 0.15f;

	static int NextIndex();
	static std::unordered_map<HANDLE, RawInput*> ConnectedControllers;
	static RawInput s_instance;
};