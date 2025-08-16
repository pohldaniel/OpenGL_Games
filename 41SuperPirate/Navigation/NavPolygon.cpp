#include "NavPolygon.h"

NavPolygon::NavPolygon() :
	m_areaID(0u),
	m_isEnabled(true),
	m_numVerts(0),
	m_minY(0.0f),
	m_maxY(0.0f),
	m_verts(nullptr)
{

}

NavPolygon::NavPolygon(NavPolygon const& rhs) : 
	m_areaID(rhs.m_areaID), 
	m_isEnabled(rhs.m_isEnabled), 
	m_numVerts(rhs.m_numVerts),
	m_verts(rhs.m_verts),
	m_minY(rhs.m_minY),
	m_maxY(rhs.m_maxY),
	m_boundingBox(rhs.m_boundingBox){
}

NavPolygon::NavPolygon(NavPolygon&& rhs) : 
	m_areaID(rhs.m_areaID),
	m_isEnabled(rhs.m_isEnabled),
	m_numVerts(rhs.m_numVerts),
	m_verts(rhs.m_verts),
	m_minY(rhs.m_minY),
	m_maxY(rhs.m_maxY),
	m_boundingBox(rhs.m_boundingBox) {
}

NavPolygon& NavPolygon::operator=(const NavPolygon& rhs) {
	m_areaID = rhs.m_areaID;
	m_isEnabled = rhs.m_isEnabled;
	m_numVerts = rhs.m_numVerts;
	m_verts = rhs.m_verts;
	m_minY = rhs.m_minY;
	m_maxY = rhs.m_maxY;
	m_boundingBox = rhs.m_boundingBox;
	return *this;
}

NavPolygon& NavPolygon::operator=(NavPolygon&& rhs) {
	m_areaID = rhs.m_areaID;
	m_isEnabled = rhs.m_isEnabled;
	m_numVerts = rhs.m_numVerts;
	m_verts = rhs.m_verts;
	m_minY = rhs.m_minY;
	m_maxY = rhs.m_maxY;
	m_boundingBox = rhs.m_boundingBox;
	return *this;
}

NavPolygon::~NavPolygon(){
}

void NavPolygon::OnRenderDebug() const {
	if (m_isEnabled) {
		for (int i = 0; i < m_numVerts; i++) {
			if (i == m_numVerts - 1) {
				DebugRenderer::Get().AddLine(Vector3f(m_verts[(m_numVerts - 1) * 3 + 0], m_verts[(m_numVerts - 1) * 3 + 1], m_verts[(m_numVerts - 1) * 3 + 2]), Vector3f(m_verts[0], m_verts[1], m_verts[2]), Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
			}else {
				DebugRenderer::Get().AddLine(Vector3f(m_verts[i * 3 + 0], m_verts[i * 3 + 1], m_verts[i * 3 + 2]), Vector3f(m_verts[i * 3 + 3], m_verts[i * 3 + 4], m_verts[i * 3 + 5]), Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
			}
		}
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

const BoundingBox& NavPolygon::getBoundingBox() const {
	return m_boundingBox;
}

//Neccessary for faster NavMesh build
void NavPolygon::createBoundingBox() {
	float xmin = FLT_MAX, ymin = FLT_MAX, zmin = FLT_MAX;
	float xmax = -FLT_MAX, ymax = -FLT_MAX, zmax = -FLT_MAX;

	for (int i = 0; i < m_numVerts; i++) {
		xmin = (std::min)(m_verts[i * 3 + 0], xmin);
		ymin = (std::min)(m_verts[i * 3 + 1], ymin);
		zmin = (std::min)(m_verts[i * 3 + 2], zmin);

		xmax = (std::max)(m_verts[i * 3 + 0], xmax);
		ymax = (std::max)(m_verts[i * 3 + 1], ymax);
		zmax = (std::max)(m_verts[i * 3 + 2], zmax);
	}

	m_boundingBox.min = Vector3f(xmin, ymin, zmin);
	m_boundingBox.max = Vector3f(xmax, ymax, zmax);
}

void NavPolygon::setNumVerts(int numVerts) {
	m_numVerts = numVerts;
}

void NavPolygon::setMinY(float minY) {
	m_minY = minY;
}
void NavPolygon::setMaxY(float maxY) {
	m_maxY = maxY;
}
void NavPolygon::setVerts(float* verts) {
	m_verts = verts;
}