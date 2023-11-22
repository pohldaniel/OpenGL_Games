#include "KeyboardEventListener.h"
#include "EventDispatcher.h"

KeyboardEventListener::~KeyboardEventListener() {
	if (mDispatcher)
	  mDispatcher->RemoveKeyboardListener(this);
}