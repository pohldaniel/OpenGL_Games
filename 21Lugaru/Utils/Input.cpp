#include "Utils/Input.hpp"

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