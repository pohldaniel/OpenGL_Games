#ifndef KEYPRESSEDEVENT_H
#define KEYPRESSEDEVENT_H

#include <engine/input/Keyboard.h>
#include "../ECS/ECS.h"
#include "../EventBus/Event.h"


class KeyPressedEvent: public EventNew {
    public:
		Keyboard::Key symbol;
        KeyPressedEvent(Keyboard::Key symbol): symbol(symbol) {}
};

#endif
