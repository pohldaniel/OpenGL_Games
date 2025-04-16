#include "NavArea.h"

static const unsigned MAX_NAV_AREA_ID = 255;
static const Vector3f DEFAULT_BOUNDING_BOX_MIN(-10.0f, -10.0f, -10.0f);
static const Vector3f DEFAULT_BOUNDING_BOX_MAX(10.0f, 10.0f, 10.0f);
static const unsigned DEFAULT_AREA_ID = 0;

NavArea::NavArea() :
	m_areaID(DEFAULT_AREA_ID),
	m_boundingBox(DEFAULT_BOUNDING_BOX_MIN, DEFAULT_BOUNDING_BOX_MAX),
	m_isEnabled(true)
{

}

NavArea::~NavArea(){
}

void NavArea::OnRenderDebug() {

	if (m_isEnabled) {
		DebugRenderer::Get().AddBoundingBox(m_boundingBox, Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	}
}

void NavArea::setAreaID(unsigned newID){
	if (newID > MAX_NAV_AREA_ID)
		std::cout << "NavArea Area ID %u exceeds maximum value of %u" << std::endl;
	m_areaID = (unsigned char)newID;
}


void NavArea::setBoundingBox(const BoundingBox& bnds) {
	m_boundingBox = bnds;
}

const BoundingBox& NavArea::getBoundingBox() const {
	return m_boundingBox;
}

unsigned NavArea::getAreaID() const {
	return (unsigned)m_areaID;
}



const bool NavArea::isEnabled() const {
	return m_isEnabled;
}