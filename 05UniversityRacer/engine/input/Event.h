#pragma once
class Event{
public:
	struct MouseMoveEvent{
		int x; 
		int y;
	};

	struct MouseButtonEvent {
		enum MouseButton {
			BUTTON_LEFT = 0,
			BUTTON_RIGHT = 1,
			BUTTON_MIDDLE = 2
		};

		int x;
		int y;
		MouseButton button;
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