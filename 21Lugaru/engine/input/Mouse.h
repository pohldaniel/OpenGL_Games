#pragma once

#include <windows.h>
#include "Event.h"

class Mouse{
public:
	enum MouseButton{
		BUTTON_LEFT = 0,
		BUTTON_RIGHT = 1,
		BUTTON_MIDDLE = 2
	};

	static Mouse &instance();

	bool buttonDown(MouseButton button) const{
		return m_pCurrButtonStates[button];
	}

	bool buttonPressed(MouseButton button) const{
		return m_pCurrButtonStates[button] && !m_pPrevButtonStates[button];
	}

	bool buttonUp(MouseButton button) const{
		return !m_pCurrButtonStates[button];
	}

	bool cursorIsVisible() const{
		return m_cursorVisible;
	}

	bool isMouseSmoothing() const{
		return m_enableFiltering;
	}

	int xPosAbsolute() const{
		return m_xPosAbsolute;
	}

	int yPosAbsolute() const
	{
		return m_yPosAbsolute;
	}

	float xPosRelative() const{
		return m_xPosRelative;
	}

	float yPosRelative() const{
		return m_yPosRelative;
	}

	float weightModifier() const{
		return m_weightModifier;
	}

	float wheelPos() const{
		return m_mouseWheel;
	}

	bool attachRaw(HWND hWnd);
	void detachRaw();
	void attach(HWND hWnd);
	void detach();
	void handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void handleEvent(Event event);

	void hideCursor(bool hideCursor);
	void setPosition(UINT x, UINT y);
	void setCursorToMiddle();
	void setAbsolute(int x, int y);
	void setWeightModifier(float weightModifier);
	void smoothMouse(bool smooth);
	void update();

	static void SetCursorIcon(std::string file);
	static HCURSOR GetCursorIcon();

private:
	Mouse();
	Mouse(const Mouse &);
	Mouse &operator=(const Mouse &);
	~Mouse();

	void performMouseFiltering(float x, float y);
	void performMouseSmoothing(float x, float y);

	static const float WEIGHT_MODIFIER;
	static const int HISTORY_BUFFER_SIZE = 10;
	static const int TEMP_BUFFER_SIZE = 40;

	static BYTE m_tempBuffer[TEMP_BUFFER_SIZE];

	HWND m_hWnd;
	int m_xPosAbsolute, m_yPosAbsolute;
	int m_xLastPos, m_yLastPos;
	int m_historyBufferSize;
	int m_mouseIndex;
	int m_wheelDelta;
	int m_prevWheelDelta;
	float m_mouseWheel;
	float m_xPosRelative;
	float m_yPosRelative;
	float m_weightModifier;
	float m_filtered[2];
	float m_history[HISTORY_BUFFER_SIZE * 2];
	float m_mouseMovementX[2];
	float m_mouseMovementY[2];
	bool m_cursorVisible;
	bool m_enableFiltering;
	bool m_buttonStates[2][3];
	bool *m_pCurrButtonStates;
	bool *m_pPrevButtonStates;
	bool m_attached;

	int m_centerX;
	int m_centerY;

	static HCURSOR Cursor;
};
