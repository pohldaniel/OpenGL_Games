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

class IController {

public:
	virtual void update() = 0;
	virtual void setVibration(float leftMotor, float rightMotor) = 0;

	virtual float getRightStickX() const = 0;
	virtual float getRightStickY() const = 0;
	virtual float getLeftStickX() const = 0;
	virtual float getLeftStickY() const = 0;
	virtual float getLeftTrigger() const = 0;
	virtual float getRightTrigger() const = 0;

	virtual bool buttonPressed(GamepadButton button) const = 0;
	virtual bool buttonDown(GamepadButton button) const = 0;
	virtual bool buttonUp(GamepadButton button) const = 0;
	virtual bool isConnected() = 0;
};

class XInputController : public IController {

public:

	void update() override;
	void setVibration(float leftMotor, float rightMotor) override;

	float getRightStickX() const override;
	float getRightStickY() const override;
	float getLeftStickX() const override;
	float getLeftStickY() const override;
	float getLeftTrigger() const override;
	float getRightTrigger() const override;

	bool buttonPressed(GamepadButton button) const override;
	bool buttonDown(GamepadButton button) const override;
	bool buttonUp(GamepadButton button) const override;
	bool isConnected() override;
	
	static void Update();
	static void Initialize();
	static void Shutdown();
	static IController* GetPlayer(int playerIndex);

private:

	XInputController();
	int m_playerIndex;
	bool m_buttonStates[2][21];
	bool* m_pCurrButtonStates;
	bool* m_pPrevButtonStates;
	
	XINPUT_STATE state;
	bool m_connected;
	float m_leftStickX;
	float m_leftStickY;
	float m_rightStickX;
	float m_rightStickY;
	float m_leftTrigger;
	float m_rightTrigger;

	static XInputController* s_Players[4];
};

class RawInputController : public IController {
	
public:

	RawInputController::~RawInputController();
	
	void update() override;
	void setVibration(float leftMotor, float rightMotor) override;

	float getRightStickX() const override;
	float getRightStickY() const override;
	float getLeftStickX() const override;
	float getLeftStickY() const override;
	float getLeftTrigger() const override;
	float getRightTrigger() const override;
	
	bool buttonPressed(GamepadButton button) const override;
	bool buttonDown(GamepadButton button) const override;
	bool buttonUp(GamepadButton button) const override;
	bool isConnected() override;

	void setWriteHandle(HANDLE writeHandle);
	void setIsBluetooth(bool isBluetooth);

	static void Update();
	static bool Initialize(HWND hWnd);
	static void Shutdown();
	static void HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static IController* GetPlayer(int playerIndex);

private:

	RawInputController();
	void processInput(RAWINPUT* raw);
	int getPlayerIndex() const;

	int m_playerIndex;
	bool m_buttonStates[2][21];
	bool* m_pCurrButtonStates;
	bool* m_pPrevButtonStates;
	float m_rightStickX = 0.0f;
	float m_rightStickY = 0.0f;
	float m_leftStickX = 0.0f;
	float m_leftStickY = 0.0f;
	float m_leftTrigger = 0.0f;
	float m_rightTrigger = 0.0f;
	bool m_isBluetooth;
	const float DEADZONE = 0.15f;

	HANDLE m_writeHandle = INVALID_HANDLE_VALUE;

	static int NextIndex();
	static std::unordered_map<HANDLE, RawInputController*> ConnectedControllers;
};