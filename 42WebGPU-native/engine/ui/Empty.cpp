#include "Empty.h"

Empty::Empty() : Widget() {

}

Empty::Empty(const Empty& rhs) :
	Widget(rhs) {
}

Empty::Empty(Empty&& rhs) noexcept :
	Widget(rhs){
}

Empty::~Empty() {

}

bool Empty::inputDefault(int mouseX, int mouseY, bool buttonLeft) {
	return false;
}

void Empty::createDefault() {

}

bool Empty::isMouseOverDefault(int mouseX, int mouseY) {
	return true;
}