#include "Key.h"
#include "Constants.h"

Key::Key(RenderableObject& player) : m_playerPos(player.getPosition()){
	ang = 0.0f;
	deployed = false;
	m_pickedKeyId = -1;
}

Key::~Key() {

}

void Key::draw(const Camera& camera) {

	auto shader = Globals::shaderManager.getAssetPointer("key");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_model", getTransformationO());
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

void Key::addInstances(const std::vector<Matrix4f>& values) {
	m_instances = values;
}

const std::vector<Matrix4f>& Key::getInstances() const{
	return m_instances;
}

/*void Key::replaceInstance(const Matrix4f& value, unsigned int index) {
	m_instances[index] = value;
}

void Key::updateShape() {
	Globals::shapeManager.get("key").updateInstances(m_instances);
}*/

void Key::updateCylinderShape() {
	std::vector<float> heights;
	const std::vector<Matrix4f>& instances = Globals::shapeManager.get("cylinder_key").getInstances();
	for (int i = 0; i < instances.size(); i++) {
		float y = instances[i][3][1];
		heights.push_back(BEACON_HEIGHT - y);
	}
	Globals::shapeManager.get("cylinder_key").addFloatAttribute(heights);
}

void Key::setPickedKeyId(int value) {
	m_pickedKeyId = value;
}

void Key::update(const float dt, const float dist) {
	ang = fmod(ang + LEVITATION_SPEED, 360.0f);
	setOrientation(0.0f, ang, 0.0f);
	
	m_mtxKey.clear();
	for (int i = 0; i < m_instances.size(); i++) {
		m_mtxKey.push_back(i == m_pickedKeyId ? Matrix4f::Translate(0.0f, 0.7f, 0.0f) : Matrix4f::Translate(0.0f, 0.5f + (sinf(ang*(PI / 180.0f))) / 2, 0.0f));
	}
	Globals::shapeManager.get("key").updateMat4Attribute(m_mtxKey);
	
	if (m_pickedKeyId > 0) {
		m_instances[m_pickedKeyId] = Matrix4f::Translate(m_playerPos[0], m_playerPos[1], m_playerPos[2]);
		Globals::shapeManager.get("key").updateInstances(m_instances);
	}

	m_mtxCylinder.clear();
	const std::vector<Matrix4f>& instances = Globals::shapeManager.get("cylinder_key").getInstances();
	for (int i = 0; i < instances.size(); i++) {
		float y = instances[i][3][1];
		if (dist / 100 < BEACON_MIN_RADIUS) {
			m_mtxCylinder.push_back(Matrix4f::Scale(BEACON_MIN_RADIUS, BEACON_HEIGHT - y, BEACON_MIN_RADIUS));
		}else {
			m_mtxCylinder.push_back(Matrix4f::Scale(dist / 100, BEACON_HEIGHT - y, dist / 100));
		}
	}
	Globals::shapeManager.get("cylinder_key").updateMat4Attribute(m_mtxCylinder);
}