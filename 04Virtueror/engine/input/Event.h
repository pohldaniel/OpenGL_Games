#pragma once
class Event{
public:
	struct MouseMoveEvent{
		int x; 
		int y;
	};

	struct KeyboardEvent {

	};

	enum EventType{
		CLOSED,                
		RESIZED,               
		MOUSEMOTION,
		KEYDOWN,
		KEYUP,
		COUNT                  
	};

	EventType type;

	union{
		MouseMoveEvent mouseMove;
		KeyboardEvent keyboard;
	};
};