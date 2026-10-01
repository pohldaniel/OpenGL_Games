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

void Empty::OnDraw() {

}

bool Empty::OnMouseOver(int mouseX, int mouseY) {
	return true;
}