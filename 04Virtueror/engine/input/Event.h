#pragma once
class Event{
public:
	struct MouseMoveEvent{
		int x; 
		int y;
	};

	struct MouseButtonEvent {
		int x;
		int y;
	};

	struct KeyboardEvent {

	};

	enum EventType{
		CLOSED,                
		RESIZED,               
		MOUSEMOTION,
		MOUSEBUTTONDOWN,
		MOUSEBUTTONUP,
		KEYDOWN,
		KEYUP,
		COUNT                  
	};

	EventType type;

	union{
		MouseMoveEvent mouseMove;
		MouseButtonEvent mouseButton;
		KeyboardEvent keyboard;
	};
};