#pragma once
#include <memory>

class GameObject {

public:
	typedef std::unique_ptr<GameObject> Ptr;
	GameObject(unsigned int category) {
		m_category = category;
	}
	unsigned int getCategory() const {
		return m_category;
	}
	

private:
	unsigned int m_category;
	
};