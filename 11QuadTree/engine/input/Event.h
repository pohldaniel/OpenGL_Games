#pragma once
class Event{
public:
	Event() = default;

	struct MouseMoveEvent{
		int x; 
		int y;
		bool titleBar;
	};

	struct MouseButtonEvent {
		enum MouseButton {
			NONE = 0,
			BUTTON_LEFT = 1,
			BUTTON_RIGHT = 2,
			BUTTON_MIDDLE = 3
		};

		int x;
		int y;
		MouseButton button;
	};

	struct KeyboardEvent {

	};

	struct ApplicationEvent {
		int width;
		int height;
	};

	enum EventType{
		CLOSED,                
		RESIZE,               
		MOUSEMOTION,
		MOUSEBUTTONDOWN,
		MOUSEBUTTONUP,
		KEYDOWN,
		KEYUP,
		COUNT                  
	};

	EventType type;
	
	union U{
		U() { mouseMove.titleBar = false; }
		MouseMoveEvent mouseMove;
		MouseButtonEvent mouseButton;
		KeyboardEvent keyboard;
		ApplicationEvent application;
	};
	U data;
};