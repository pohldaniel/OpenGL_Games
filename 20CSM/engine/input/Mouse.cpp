#include <iostream>
#include "Mouse.h"

const float Mouse::WEIGHT_MODIFIER = 0.2f;
BYTE Mouse::m_tempBuffer[TEMP_BUFFER_SIZE];

Mouse &Mouse::instance() {

	static Mouse theInstance;
	return theInstance;
}

Mouse::Mouse() {

	m_hWnd = 0;
	m_cursorVisible = true;
	m_enableFiltering = true;

	m_wheelDelta = 0;
	m_prevWheelDelta = 0;
	m_mouseWheel = 0.0f;

	m_xPosAbsolute = 0;
	m_yPosAbsolute = 0;
	m_xPosRelative = 0;
	m_yPosRelative = 0;
	m_weightModifier = WEIGHT_MODIFIER;
	m_historyBufferSize = HISTORY_BUFFER_SIZE;
	m_attached = false;

	m_pCurrButtonStates = m_buttonStates[0];
	m_pPrevButtonStates = m_buttonStates[1];
}

Mouse::~Mouse() {
	detach();
}

bool Mouse::attach(HWND hWnd) {

	if (!hWnd)
		return false;

	if (!m_hWnd) {

		RAWINPUTDEVICE rid[1] = { 0 };

		rid[0].usUsagePage = 1;
		rid[0].usUsage = 2;
		rid[0].dwFlags = 0;
		rid[0].hwndTarget = hWnd;

		if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0])))
			return false;

		m_hWnd = hWnd;
	}

	if (!m_cursorVisible)
		hideCursor(true);

	m_filtered[0] = 0.0f;
	m_filtered[1] = 0.0f;
	m_pCurrButtonStates = m_buttonStates[0];
	m_pPrevButtonStates = m_buttonStates[1];

	m_mouseIndex = 0;
	m_mouseMovementX[0] = m_mouseMovementX[1] = 0.0f;
	m_mouseMovementY[0] = m_mouseMovementY[1] = 0.0f;

	memset(m_history, 0, sizeof(m_history));
	memset(m_buttonStates, 0, sizeof(m_buttonStates));

	return true;
}

void Mouse::detach() {

	if (!m_cursorVisible) {

		hideCursor(false);
		// Save the cursor visibility state in case attach() is called later.
		m_cursorVisible = false;
	}

	m_hWnd = 0;
}

void Mouse::performMouseFiltering(float x, float y) {

	// Filter the relative mouse movement based on a weighted sum of the mouse
	// movement from previous frames to ensure that the mouse movement this
	// frame is smooth.
	//
	// For further details see:
	//  Nettle, Paul "Smooth Mouse Filtering", flipCode's Ask Midnight column.
	//  http://www.flipcode.com/cgi-bin/fcarticles.cgi?show=64462

	// Newer mouse entries towards front and older mouse entries towards end.
	for (int i = m_historyBufferSize - 1; i > 0; --i) {
		m_history[i * 2] = m_history[(i - 1) * 2];
		m_history[i * 2 + 1] = m_history[(i - 1) * 2 + 1];
	}

	// Store current mouse entry at front of array.
	m_history[0] = x;
	m_history[1] = y;

	float averageX = 0.0f;
	float averageY = 0.0f;
	float averageTotal = 0.0f;
	float currentWeight = 1.0f;

	// Filter the mouse.
	for (int i = 0; i < m_historyBufferSize; ++i) {
		averageX += m_history[i * 2] * currentWeight;
		averageY += m_history[i * 2 + 1] * currentWeight;
		averageTotal += 1.0f * currentWeight;
		currentWeight *= m_weightModifier;
	}

	m_filtered[0] = averageX / averageTotal;
	m_filtered[1] = averageY / averageTotal;
}

void Mouse::performMouseSmoothing(float x, float y) {
	// Smooth out the mouse movement by averaging the distance the mouse
	// has moved over a couple of frames.

	m_mouseMovementX[m_mouseIndex] = x;
	m_mouseMovementY[m_mouseIndex] = y;

	m_filtered[0] = (m_mouseMovementX[0] + m_mouseMovementX[1]) * 0.5f;
	m_filtered[1] = (m_mouseMovementY[0] + m_mouseMovementY[1]) * 0.5f;

	m_mouseIndex ^= 1;
	m_mouseMovementX[m_mouseIndex] = 0.0f;
	m_mouseMovementY[m_mouseIndex] = 0.0f;
}

void Mouse::handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	RAWINPUT *pRaw = 0;
	UINT size = TEMP_BUFFER_SIZE;
	switch (msg) {

	default: {

		break;
	}
	case WM_INPUT:
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, m_tempBuffer, &size, sizeof(RAWINPUTHEADER));
		pRaw = reinterpret_cast<RAWINPUT*>(m_tempBuffer);
		if (pRaw->header.dwType == RIM_TYPEMOUSE) {

			m_xPosRelative = static_cast<float>(pRaw->data.mouse.lLastX);
			m_yPosRelative = static_cast<float>(pRaw->data.mouse.lLastY);

			if (m_enableFiltering) {

				performMouseFiltering(m_xPosRelative, m_yPosRelative);

				m_xPosRelative = m_filtered[0];
				m_yPosRelative = m_filtered[1];

				performMouseSmoothing(m_xPosRelative, m_yPosRelative);

				m_xPosRelative = m_filtered[0];
				m_yPosRelative = m_filtered[1];
			}
		}

		break;

	case WM_MOUSEMOVE: {
		int x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
		int y = static_cast<int>(static_cast<short>(HIWORD(lParam)));

		m_xPosRelative = x - centerX;
		m_yPosRelative = y - centerY;


		m_xPosAbsolute = x;
		m_yPosAbsolute = y;

		break;

	}case WM_MOUSEWHEEL:
		m_wheelDelta += static_cast<int>(static_cast<int>(wParam) >> 16);
		break;
	}
}

void Mouse::handleEvent(Event event) {
	RAWINPUT *pRaw = 0;
	UINT size = TEMP_BUFFER_SIZE;

	switch (event.type) {
	case Event::MOUSEMOTION: {
		int x = event.data.mouseMove.x;
		int y = event.data.mouseMove.y;

		m_xPosRelative = x - centerX;
		m_yPosRelative = y - centerY;

		m_xPosAbsolute = x;
		m_yPosAbsolute = y;
		break;
	}
	}
}

void Mouse::hideCursor(bool hideCursor) {

	if (hideCursor) {
		m_cursorVisible = false;

		while (ShowCursor(FALSE) >= 0)
			; // do nothing
	}
	else {
		m_cursorVisible = true;

		while (ShowCursor(TRUE) < 0)
			; // do nothing
	}
}

void Mouse::setPosition(UINT x, UINT y) {
	POINT pt = { x, y };

	if (ClientToScreen(m_hWnd, &pt)) {
		SetCursorPos(pt.x, pt.y);

		m_xPosAbsolute = x;
		m_yPosAbsolute = y;

		m_xPosRelative = 0.0f;
		m_yPosRelative = 0.0f;
	}
}

void Mouse::setCursorToMiddle() {
	SetCursorPos(centerX, centerY);
}

void Mouse::setWeightModifier(float weightModifier) {

	m_weightModifier = weightModifier;
}

void Mouse::smoothMouse(bool smooth) {

	m_enableFiltering = smooth;
}

void Mouse::update() {

	bool *pTempMouseStates = m_pPrevButtonStates;

	m_pPrevButtonStates = m_pCurrButtonStates;
	m_pCurrButtonStates = pTempMouseStates;

	m_pCurrButtonStates[0] = (GetKeyState(VK_LBUTTON) & 0x8000) ? true : false;
	m_pCurrButtonStates[1] = (GetKeyState(VK_RBUTTON) & 0x8000) ? true : false;
	m_pCurrButtonStates[2] = (GetKeyState(VK_MBUTTON) & 0x8000) ? true : false;

	m_mouseWheel = static_cast<float>(m_wheelDelta - m_prevWheelDelta) / static_cast<float>(WHEEL_DELTA);
	m_prevWheelDelta = m_wheelDelta;

	if (m_attached) {

		POINT        CursorPos;
		GetCursorPos(&CursorPos);
		m_xPosRelative = (CursorPos.x - centerX);
		m_yPosRelative = (CursorPos.y - centerY);
		setCursorToMiddle();
	}
}

void Mouse::attach2(HWND hWnd) {
	if (m_attached) return;
	m_hWnd = hWnd;

	POINT        CursorPos;
	GetCursorPos(&CursorPos);
	m_xLastPos = CursorPos.x;
	m_yLastPos = CursorPos.y;

	RECT rectClient, rectWindow;
	GetWindowRect(m_hWnd, &rectWindow);

	GetClientRect(m_hWnd, &rectClient);
	centerX = rectWindow.left + rectClient.right / 2;
	centerY = rectWindow.top + rectClient.bottom / 2;

	setCursorToMiddle();
	hideCursor(true);
	m_attached = true;
}

void Mouse::detach2() {
	if (!m_attached) return;

	SetCursorPos(m_xLastPos, m_yLastPos);
	m_attached = false;
	hideCursor(false);
	m_xPosRelative = 0;
	m_yPosRelative = 0;
	m_hWnd = 0;
}

void Mouse::setAbsolute(int x, int y) {
	m_xPosAbsolute = x;
	m_yPosAbsolute = y;
}