// For conditions of distribution and use, see copyright notice in License.txt

#include "InputTu.h"
#include <tracy/Tracy.hpp>

InputTu::InputTu(SDL_Window* window_) :
    window(window_),
    mouseMove(IntVector2::ZERO),
    mouseWheel(IntVector2::ZERO),
    focus(false),
    shouldExit(false)
{
    assert(window);
}

InputTu::~InputTu()
{
}

void InputTu::Update()
{
   
}

ButtonState InputTu::KeyState(unsigned keyCode) const
{
    auto it = keyStates.find(keyCode);
    return it != keyStates.end() ? it->second : STATE_UP;
}

ButtonState InputTu::MouseButtonState(unsigned num) const
{
    auto it = mouseButtonStates.find(num);
    return it != mouseButtonStates.end() ? it->second : STATE_UP;
}
