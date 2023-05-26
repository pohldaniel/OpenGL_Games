#include "ColumnSet.h"
#include "Constants.h"
#include "cTerrain.h"

ColumnSet::ColumnSet(const Vector3f& playerPos) : m_playerPos(playerPos) {

}

ColumnSet::~ColumnSet() {

}

void ColumnSet::init(const cTerrain& terrain) {
		
	std::vector<Matrix4f> instances = { Matrix4f::Translate(TERRAIN_SIZE / 2 + 18.0f, terrain.GetHeight(TERRAIN_SIZE / 2 + 18.0f, TERRAIN_SIZE / 2 + 8.0f),  TERRAIN_SIZE / 2 + 8.0f)  * Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), 90.0f),
										Matrix4f::Translate(TERRAIN_SIZE / 2 + 14.0f, terrain.GetHeight(TERRAIN_SIZE / 2 + 14.0f, TERRAIN_SIZE / 2 - 8.0f),  TERRAIN_SIZE / 2 - 8.0f)  * Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), 90.0f),
										Matrix4f::Translate(TERRAIN_SIZE / 2,         terrain.GetHeight(TERRAIN_SIZE / 2,         TERRAIN_SIZE / 2 - 16.0f), TERRAIN_SIZE / 2 - 16.0f) * Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), 180.0f),
										Matrix4f::Translate(TERRAIN_SIZE / 2 - 14.0f, terrain.GetHeight(TERRAIN_SIZE / 2 - 14.0f, TERRAIN_SIZE / 2 - 8.0f),  TERRAIN_SIZE / 2 - 8.0f)  * Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), -90.0f),
										Matrix4f::Translate(TERRAIN_SIZE / 2 - 18.0f, terrain.GetHeight(TERRAIN_SIZE / 2 - 18.0f, TERRAIN_SIZE / 2 + 8.0f),  TERRAIN_SIZE / 2 + 8.0f)  * Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), -90.0f)};

	std::vector<Vector4f> colors = { Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Vector4f(1.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.1f, 0.1f, 1.0f, 1.0f) , Vector4f(1.0f, 0.0f, 1.0f, 1.0f) };

	Globals::shapeManager.get("column").addInstances(instances);
	Globals::shapeManager.get("column").addVec4Attribute(colors);

	Globals::shapeManager.get("sphere_cl").addInstances(instances);
	Globals::shapeManager.get("sphere_cl").addVec4Attribute(colors);

	m_states = fromInstances(instances);
}

void ColumnSet::draw(const Camera& camera) {

	auto shader = Globals::shaderManager.getAssetPointer("column");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix()));

	shader->loadVector("u_lightPos", Vector3f(50.0f, 50.0f, 50.0f));

	shader->loadFloat("invRadius", 0.0f);
	shader->loadFloat("alpha", 1.0f);
	shader->loadInt("u_texture", 0);
	shader->loadInt("u_normalMap", 1);
	Globals::textureManager.get("column").bind(0);
	Globals::textureManager.get("column_nmp").bind(1);
	Globals::shapeManager.get("column").drawRawInstanced();
	shader->unuse();

	glEnable(GL_BLEND);
	shader = Globals::shaderManager.getAssetPointer("energy");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix()));
	Globals::shapeManager.get("sphere_cl").drawRawInstanced();
	shader->unuse();
	glDisable(GL_BLEND);
}

const std::vector<ColumnSet::State>& ColumnSet::fromInstances(const std::vector<Matrix4f>& instances) {
	m_states.clear();
	std::transform(instances.begin(), instances.end(), std::back_inserter(m_states), [](const Matrix4f& p)-> State { return{ { p[3][0], p[3][1] , p[3][2] },  atan2f(-p[0][2], p[2][2]) * _180_ON_PI }; });
	return m_states;
}

bool ColumnSet::insideGatheringArea(unsigned short index) {

	float yaw = m_states[index].yaw <= -180.0f ? -m_states[index].yaw : m_states[index].yaw;
	const Vector3f& pos = m_states[index].position;

	if (yaw == -90)
		return (m_playerPos[2] <= pos[2] + GATHERNG_AREA_SIDE / 2 && m_playerPos[2] >= pos[2] - GATHERNG_AREA_SIDE / 2 && m_playerPos[0] <= pos[0] && m_playerPos[0] >= pos[0] - GATHERNG_AREA_SIDE);
	else if (yaw == 90)
		return (m_playerPos[2] <= pos[2] + GATHERNG_AREA_SIDE / 2 && m_playerPos[2] >= pos[2] - GATHERNG_AREA_SIDE / 2 && m_playerPos[0] <= pos[0] + GATHERNG_AREA_SIDE && m_playerPos[0] >= pos[0]);
	else //if(yaw == 180)
		return (m_playerPos[2] <= pos[2] && m_playerPos[2] >= pos[2] - GATHERNG_AREA_SIDE && m_playerPos[0] <= pos[0] + GATHERNG_AREA_SIDE / 2 && m_playerPos[0] >= pos[0] - GATHERNG_AREA_SIDE / 2);
}

const Vector3f& ColumnSet::getPosition(unsigned short index) const {
	return m_states[index].position;
}

const Vector3f ColumnSet::getHole(unsigned short index) const {
	const Vector3f& pos = getPosition(index);
	float yaw = getYaw(index);
	return Vector3f(yaw == -90 ? pos[0] - 1.5f : yaw == 90 ? pos[0] + 1.5f : pos[0], pos[1] + 1.0f, yaw == 180 ? pos[2] - 1.5f : pos[2]);
}

float ColumnSet::getYaw(unsigned short index) const {
	return m_states[index].yaw <= -180.0f ? -m_states[index].yaw : m_states[index].yaw;
}

const std::vector<ColumnSet::State>& ColumnSet::getStates() {
	return m_states;
}