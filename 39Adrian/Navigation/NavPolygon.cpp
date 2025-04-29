#include "NavPolygon.h"

NavPolygon::NavPolygon() :
	m_areaID(0u),
	m_isEnabled(true)
{

}

NavPolygon::NavPolygon(NavPolygon const& rhs) : m_areaID(rhs.m_areaID), m_isEnabled(rhs.m_isEnabled) {

}

NavPolygon::NavPolygon(NavPolygon&& rhs) : m_areaID(rhs.m_areaID), m_isEnabled(rhs.m_isEnabled) {

}

NavPolygon& NavPolygon::operator=(const NavPolygon& rhs) {
	m_areaID = rhs.m_areaID;
	m_isEnabled = rhs.m_isEnabled;
	return *this;
}

NavPolygon& NavPolygon::operator=(NavPolygon&& rhs) {
	m_areaID = rhs.m_areaID;
	m_isEnabled = rhs.m_isEnabled;
	return *this;
}

NavPolygon::~NavPolygon(){
}

void NavPolygon::OnRenderDebug() const {

	if (m_isEnabled) {
		//DebugRenderer::Get().AddBoundingBox(m_boundingBox, Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	}
}

void NavPolygon::setAreaID(unsigned int newID) const {
	if (newID > 255u)
		std::cout << "NavArea Area ID %u exceeds maximum value of %u" << std::endl;
	m_areaID = (unsigned char)newID;
}

unsigned NavPolygon::getAreaID() const {
	return (unsigned)m_areaID;
}

const bool NavPolygon::isEnabled() const {
	return m_isEnabled;
}