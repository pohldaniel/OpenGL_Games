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

void Empty::inputDefault(int mouseX, int mouseY, bool buttonLeft) {

}

/*void Empty::layoutDefault() {

}*/

void Empty::createDefault() {

}