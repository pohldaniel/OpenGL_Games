#pragma once
class Event
{
public:
	struct MouseMoveEvent{
		int x; 
		int y;
	};

	enum EventType{
		CLOSED,                
		RESIZED,               
		MOUSEMOTION,
		COUNT                  
	};

	EventType type;

	union{
		MouseMoveEvent mouseMove;
	};
};