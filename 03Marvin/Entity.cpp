#include "Entity.h"

Entity::Entity(unsigned int category, const float& dt, const float& fdt) : RenderableObject(category) , m_dt(dt), m_fdt(fdt) {}

Entity::~Entity() {
}
