#include "KeySet.h"
#include "Terrain.h"
#include "Globals.h"

KeySet::KeySet(const Vector3f& playerPos) : m_playerPos(playerPos){
	ang = 0.0f;
	m_numDeployed = 0;
	m_pickedKeyId = -1;
}

KeySet::~KeySet() {

}

void KeySet::init(const Terrain& terrain) {
	m_colors.assign({ Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Vector4f(1.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.2f, 0.2f, 1.0f, 1.0f) , Vector4f(1.0f, 0.0f, 1.0f, 1.0f) });
	std::vector<Matrix4f> instances = { //Matrix4f::Translate(883.0f, terrain.heightAt(883.0f, 141.0f), 141.0f),
										Matrix4f::Translate(TERRAIN_SIZE / 2, terrain.heightAt(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2 + 10.0f), TERRAIN_SIZE / 2 + 10.0f),
										//Matrix4f::Translate(345.0f, terrain.heightAt(345.0f, 229.0f), 229.0f),
										Matrix4f::Translate(TERRAIN_SIZE / 2, terrain.heightAt(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2 + 20.0f), TERRAIN_SIZE / 2 + 20.0f),
										Matrix4f::Translate(268.0f, terrain.heightAt(268.0f, 860.0f), 860.0f),
										Matrix4f::Translate(780.0f, terrain.heightAt(780.0f, 858.0f), 858.0f),
										Matrix4f::Translate(265.0f, terrain.heightAt(265.0f, 487.0f), 487.0f) };

	addInstances(instances);

	Globals::shapeManager.get("key").addInstances(5u, 1u, GL_DYNAMIC_DRAW);
	Globals::shapeManager.get("key").updateInstances(instances);
	Globals::shapeManager.get("key").addVec4Attribute(m_colors, 1);
	Globals::shapeManager.get("key").addMat4Attribute(5u, 1u);

	Globals::shapeManager.get("cylinder_key").addInstances(instances);
	Globals::shapeManager.get("cylinder_key").addVec4Attribute(m_colors, 1);
	Globals::shapeManager.get("cylinder_key").addMat4Attribute(5u, 1u);
	updateCylinderShape();

	m_idCache.resize(instances.size());
	std::iota(std::begin(m_idCache), std::end(m_idCache), 0);
	

}

void KeySet::draw(const Camera& camera) {

	auto shader = Globals::shaderManager.getAssetPointer("key");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix()));

	shader->loadVector("u_lightPos", Vector3f(50.0f, 50.0f, 50.0f));

	shader->loadFloat("invRadius", 0.0f);
	shader->loadFloat("alpha", 1.0f);
	shader->loadInt("u_texture", 0);
	shader->loadInt("u_normalMap", 1);

	Globals::textureManager.get("key").bind(0);
	Globals::textureManager.get("key_nmp").bind(1);

	Globals::shapeManager.get("key").drawRawInstanced();
	shader->unuse();

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	shader = Globals::shaderManager.getAssetPointer("beam");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix()));
	Globals::shapeManager.get("cylinder_key").drawRawInstanced();
	shader->unuse();

	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void KeySet::addInstances(const std::vector<Matrix4f>& values) {
	m_keyStates = fromInstances(values);
}

unsigned short KeySet::getNumDeployed() {
	return m_numDeployed;
}

const std::vector<KeySet::State>& KeySet::getKeyStates() {
	return m_keyStates;
}

void KeySet::updateCylinderShape() {
	std::vector<float> heights;
	const std::vector<Matrix4f>& instances = Globals::shapeManager.get("cylinder_key").getInstances();
	for (int i = 0; i < instances.size(); i++) {
		float y = instances[i][3][1];
		heights.push_back(BEACON_HEIGHT - y);
	}
	Globals::shapeManager.get("cylinder_key").addFloatAttribute(heights);
}

void KeySet::setPickedKeyId(int value) {
	m_pickedKeyId = value;
	
	m_colors.erase(m_colors.begin() + m_idCache[m_pickedKeyId]);
	Globals::shapeManager.get("cylinder_key").addVec4Attribute(m_colors, 1);

	Globals::shapeManager.get("cylinder_key").removeInstance(m_idCache[m_pickedKeyId]);
	updateCylinderShape();

	for_each(m_idCache.begin() + value, m_idCache.end(), [](unsigned short& v) {v--; });
}

void KeySet::deploy(int id, const Vector3f& pos, float yaw) {
	m_keyStates[id].deployed = true;
	Globals::shapeManager.get("key").updateInstance(Matrix4f::Translate(pos[0], pos[1], pos[2]) * Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), yaw) * Matrix4f::Rotate(Vector3f(1.0f, 0.0f, 0.0f), 225.0f) * Matrix4f::Translate(0.0f, -0.69f, 0.0f), id);
	m_pickedKeyId = -1;
	m_numDeployed++;
}

bool KeySet::isDeployed(unsigned short index) {
	return m_keyStates[index].deployed;
}

const Vector3f& KeySet::getPosition(unsigned short index) {
	return m_keyStates[index].position;
}

void KeySet::update(const float dt) {
	ang = fmod(ang + LEVITATION_SPEED, 360.0f);	
	m_mtxKey.clear();

	for (int i = 0; i < m_keyStates.size(); i++) {
		m_mtxKey.push_back(m_keyStates[i].deployed ? Matrix4f::IDENTITY : i == m_pickedKeyId ? Matrix4f::Translate(0.0f, 0.7f, 0.0f) * Matrix4f::Rotate(0.0f, ang, 0.0f) : Matrix4f::Translate(0.0f, 0.5f + (sinf(ang*(PI / 180.0f))) / 2, 0.0f) * Matrix4f::Rotate(0.0f, ang, 0.0f));
	}
	Globals::shapeManager.get("key").updateMat4Attribute(m_mtxKey);
	
	if (m_pickedKeyId > -1) {
		Globals::shapeManager.get("key").updateInstance(Matrix4f::Translate(m_playerPos[0], m_playerPos[1], m_playerPos[2]), m_pickedKeyId);
	}

	m_mtxCylinder.clear();
	const std::vector<Matrix4f>& instances = Globals::shapeManager.get("cylinder_key").getInstances();
	for (int i = 0; i < instances.size(); i++) {
		float x = instances[i][3][0];
		float y = instances[i][3][1];
		float z = instances[i][3][2];

		float dist = sqrt((m_playerPos[0] - x)*(m_playerPos[0] - x) + (m_playerPos[2] - z)*(m_playerPos[2] - z));

		m_mtxCylinder.push_back(dist / 100 < BEACON_MIN_RADIUS ? Matrix4f::Scale(BEACON_MIN_RADIUS, BEACON_HEIGHT - y, BEACON_MIN_RADIUS) : Matrix4f::Scale(dist / 100, BEACON_HEIGHT - y, dist / 100));		
	}
	Globals::shapeManager.get("cylinder_key").updateMat4Attribute(m_mtxCylinder);
}

//just call it once it will override the deployment state
const std::vector<KeySet::State>& KeySet::fromInstances(const std::vector<Matrix4f>& instances) {
	m_keyStates.clear();
	std::transform(instances.begin(), instances.end(), std::back_inserter(m_keyStates), [](const Matrix4f& p)-> State { return{false, {p[3][0], p[3][1] , p[3][2] } }; });
	return m_keyStates;
}