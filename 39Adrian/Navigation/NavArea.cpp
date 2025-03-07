#include "NavArea.h"

static const unsigned MAX_NAV_AREA_ID = 255;
static const Vector3f DEFAULT_BOUNDING_BOX_MIN(-10.0f, -10.0f, -10.0f);
static const Vector3f DEFAULT_BOUNDING_BOX_MAX(10.0f, 10.0f, 10.0f);
static const unsigned DEFAULT_AREA_ID = 0;

NavArea::NavArea() :
	areaID_(DEFAULT_AREA_ID),
	boundingBox_(DEFAULT_BOUNDING_BOX_MIN, DEFAULT_BOUNDING_BOX_MAX),
	isEnabled_(true)
{

}

NavArea::~NavArea(){
}

void NavArea::SetAreaID(unsigned newID){
	if (newID > MAX_NAV_AREA_ID)
		std::cout << "NavArea Area ID %u exceeds maximum value of %u" << std::endl;
	areaID_ = (unsigned char)newID;
}

BoundingBox NavArea::GetWorldBoundingBox() const{
	return boundingBox_;
}

void NavArea::OnRenderDebug() {

	if (isEnabled_){
		DebugRenderer::Get().AddBoundingBox(boundingBox_, Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
		//DebugRenderer::Get().AddBoundingBox(boundingBox_, Vector4f(0.0f, 1.0f, 0.0f, 0.15f));
	}
}