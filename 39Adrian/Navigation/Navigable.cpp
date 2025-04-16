#include "Navigable.h"

Navigable::Navigable(SceneNodeLC* node) : m_node(node), m_recursive(true){

}

Navigable::~Navigable(){

}

void Navigable::setRecursive(bool enable){
	m_recursive = enable;
}

bool Navigable::isRecursive() const { 
	return m_recursive;
}