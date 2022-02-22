#ifndef __applicationH__
#define __applicationH__

#include <windows.h>
#include "Extension.h"
#include "Constants.h"

class Application {
public:
	Application();
	~Application();

	bool isRunning();
	HWND getWindow();

private:

	bool initWindow();
	void initOpenGL();
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT DisplayWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	void enableWireframe(bool enableWireframe);
	void enableVerticalSync(bool enableVerticalSync);
	void setCursortoMiddle(HWND hWnd);

	POINT m_oldCursorPos;
	bool m_enableVerticalSync;
	bool m_enableWireframe;
	HWND m_window;
	MSG msg;
};

#endif // __applicationH__
