#include "ApplicationEventListener.h"
#include "EventDispatcher.h"

ApplicationEventListener::~ApplicationEventListener() {
	if (mDispatcher)
		mDispatcher->RemoveApplicationListener(this);
}