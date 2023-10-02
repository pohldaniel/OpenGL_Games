#pragma once
class Event{
public:
	Event() = default;

	struct MouseMoveEvent{
		enum MouseButton {
			NONE = 0,
			BUTTON_LEFT = 1,
			BUTTON_RIGHT = 2,
			BUTTON_MIDDLE = 3
		};

		int x; 
		int y;
		bool titleBar;
		MouseButton button;
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

	struct MouseWheelEvent {
		enum WheelDirection {
			UP = 0,
			DOWN = 1
		};
		WheelDirection direction;
		short delta;
	};

	struct KeyboardEvent {
		unsigned int keyCode;
	};

	struct ApplicationEvent {
		int width;
		int height;
	};

	enum EventType{
		CLOSED,                
		RESIZE,               
		MOUSEMOTION,
		MOUSEWHEEL,
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
		MouseWheelEvent mouseWheel;
	};
	U data;
};