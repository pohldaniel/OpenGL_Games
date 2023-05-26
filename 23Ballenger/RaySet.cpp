#include "RaySet.h"
#include "Constants.h"

RaySet::RaySet(const cPortal& portal) : m_portal(portal) {

}

RaySet::~RaySet() {

}

void RaySet::init() {
	lineColors = { Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Vector4f(1.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.2f, 0.2f, 1.0f, 1.0f) , Vector4f(1.0f, 0.0f, 1.0f, 1.0f) };
	ang = 0.0f;
}

void RaySet::draw(const Camera& camera) {
	if (m_line.isActive()) {
		auto shader = Globals::shaderManager.getAssetPointer("line");
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", camera.getViewMatrix());
		m_line.drawRaw();
		shader->unuse();
	}
}

void RaySet::deploy(const Vector3f& begin, const Vector3f& end, int id, int numDeployed) {
	int numrays = 6;
	for (int j = 0; j < numrays; j++) {
		m_line.addToBuffer(begin, end);
		colors.push_back(lineColors[id]);
		colors.push_back(lineColors[id]);
	}

	m_line.addVec4Attribute(colors);
	m_line.addMat4Attribute(numDeployed * numrays * 2, 0u);

	m_pickedIds.push_back(id);
	m_numDeployed = numDeployed;
}

void RaySet::update(const float dt, const std::vector<ColumnSet::State>& columns) {
	if (m_line.isActive()) {
		int numrays = 6;
		float r = 0.5f;
		ang = fmod(ang + 2, 360);
		std::vector<Matrix4f> mtxLines;
		for (int i = 0; i < m_numDeployed; i++) {

			const Vector3f& columnPos = columns[m_pickedIds[i]].position;
			Vector3f center = Vector3f(columnPos[0], columnPos[1] + 7.0f + 1.0f, columnPos[2]);
			Vector3f axis = Vector3f::Normalize(Vector3f(m_portal.GetReceptorX(m_pickedIds[i]), m_portal.GetReceptorY(m_pickedIds[i]), m_portal.GetZ()) - center);

			for (int j = 0; j < numrays; j++) {
				mtxLines.push_back(Matrix4f::Translate(cosf((ang + j * 360.0f / numrays) * PI_ON_180) * r, sinf((ang + j * 360.0f / numrays) * PI_ON_180) * r, 0.0f));
				mtxLines.push_back(Matrix4f::IDENTITY);
			}
			m_line.updateMat4Attribute(mtxLines);
		}
	}
}

