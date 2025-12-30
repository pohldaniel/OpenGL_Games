#include "Entity_new.h"

EntityNew::EntityNew() {

}

EntityNew& EntityNew::operator=(const EntityNew& rhs) {
	return *this;
}

EntityNew& EntityNew::operator=(EntityNew&& rhs) noexcept {
	return *this;
}

EntityNew::EntityNew(EntityNew const& rhs) {
	
}

EntityNew::EntityNew(EntityNew&& rhs)  noexcept {
	
}

EntityNew::~EntityNew() {

}