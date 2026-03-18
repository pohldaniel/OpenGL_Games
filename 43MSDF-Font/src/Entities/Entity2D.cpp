#include "Entity2D.h"

Entity2D::Entity2D() {

}

Entity2D& Entity2D::operator=(const Entity2D& rhs) {
	return *this;
}

Entity2D& Entity2D::operator=(Entity2D&& rhs) noexcept {
	return *this;
}

Entity2D::Entity2D(Entity2D const& rhs) {
	
}

Entity2D::Entity2D(Entity2D&& rhs)  noexcept {
	
}

Entity2D::~Entity2D() {

}