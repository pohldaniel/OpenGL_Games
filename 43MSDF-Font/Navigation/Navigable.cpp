#include "Navigable.h"

Navigable::Navigable(SceneNodeLC* node) : m_node(node), m_recursive(true){

}

Navigable::Navigable(Navigable const& rhs) : m_node(rhs.m_node), m_recursive(rhs.m_recursive){

}

Navigable::Navigable(Navigable&& rhs) : m_node(rhs.m_node), m_recursive(rhs.m_recursive) {

}

Navigable& Navigable::operator=(const Navigable& rhs) {
	m_node = rhs.m_node;
	m_recursive = rhs.m_recursive;
	return *this;
}

Navigable& Navigable::operator=(Navigable&& rhs) {
	m_node = rhs.m_node;
	m_recursive = rhs.m_recursive;
	return *this;
}

Navigable::~Navigable(){

}

void Navigable::setRecursive(bool enable){
	m_recursive = enable;
}

bool Navigable::isRecursive() const { 
	return m_recursive;
}