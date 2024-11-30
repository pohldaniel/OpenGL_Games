#include "Empty.h"

Empty::Empty() : WidgetMH() {

}

Empty::Empty(const Empty& rhs) :
	WidgetMH(rhs) {
}

Empty::Empty(Empty&& rhs) :
	WidgetMH(rhs) {
}

Empty::~Empty() {

}

void Empty::drawDefault() {

}