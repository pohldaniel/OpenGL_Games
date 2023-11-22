#include "MouseEventListener.h"
#include "EventDispatcher.h"

MouseEventListener::~MouseEventListener(){
	if (mDispatcher)
		mDispatcher->RemoveMouseListener(this);
}