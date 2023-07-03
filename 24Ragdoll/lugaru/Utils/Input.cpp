#include "../Utils/Input.hpp"

bool Input::isKeyDown(Keyboard::Key key){
   
	return Keyboard::instance().keyDown(key);
}

bool Input::isKeyPressed(Keyboard::Key key){
	return Keyboard::instance().keyPressed(key);
}

bool Input::MouseClicked() {
	return Mouse::instance().buttonPressed(Mouse::BUTTON_LEFT);
}

bool Input::isMousePressed(Mouse::MouseButton button) {
	return Mouse::instance().buttonPressed(button);
}

bool Input::isMouseDown(Mouse::MouseButton button) {
	return Mouse::instance().buttonDown(button);
}

std::string Input::GetCharsFromKeys(unsigned short key, bool shift, bool altGr) {
	BYTE kb[256];
	GetKeyboardState(kb);
	WCHAR uc[5];

	ToUnicode(key, MapVirtualKey((UINT)key, MAPVK_VK_TO_CHAR), kb, uc, 4, 0);

	char ch[5];
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, uc, -1, ch, 5, &DefChar, NULL);


	return std::string(ch);
}