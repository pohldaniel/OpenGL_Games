#include "RespawnPointSet.h"
#include "Terrain.h"
#include "Globals.h"

RespawnPointSet::RespawnPointSet(const Vector3f& playerPos) : m_playerPos(playerPos){

}

RespawnPointSet::~RespawnPointSet() {

}

void RespawnPointSet::init(const Terrain& terrain) {
	std::vector<Matrix4f> instances = { Matrix4f::Translate(TERRAIN_SIZE / 2, terrain.heightAt(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2), TERRAIN_SIZE / 2),
										Matrix4f::Translate(256.0f, terrain.heightAt(256.0f, 160.0f), 160.0f),
										Matrix4f::Translate(840.0f, terrain.heightAt(840.0f, 184.0f), 184.0f),
										Matrix4f::Translate(552.0f, terrain.heightAt(552.0f, 760.0f), 760.0f),
										Matrix4f::Translate(791.0f, terrain.heightAt(791.0f, 850.0f), 850.0f),
										Matrix4f::Translate(152.0f, terrain.heightAt(152.0f, 832.0f), 832.0f),
										Matrix4f::Translate(448.0f, terrain.heightAt(448.0f, 944.0f), 944.0f),
										Matrix4f::Translate(816.0f, terrain.heightAt(816.0f, 816.0f), 816.0f) };

	addInstances(instances);

	Globals::shapeManager.get("quad_rp").setInstances(instances);
	Globals::shapeManager.get("cylinder").setInstances(instances);
	Globals::shapeManager.get("disk").setInstances(instances);

	glGenBuffers(1, &Globals::colorUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, Globals::colorUbo);
	glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, Globals::colorBinding, Globals::colorUbo, 0, 128);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &Globals::activateUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, Globals::activateUbo);
	glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, Globals::activateBinding, Globals::activateUbo, 0, 128);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//not neccessary: The block binding indices can be set directly from the shader:
	//layout(std140, binding = 0) uniform u_color {
	//	vec4 color[8];
	//};
	//auto shader = Globals::shaderManager.getAssetPointer("instance");
	//glUniformBlockBinding(shader->m_program, glGetUniformBlockIndex(shader->m_program, "u_color"), Globals::colorBinding);
	//glUniformBlockBinding(shader->m_program, glGetUniformBlockIndex(shader->m_program, "u_activate"), Globals::activeBinding);


	m_respawnId = 0;
	m_colors.resize(instances.size());
	m_activate.resize(instances.size());
	updateUbo();
}

void RespawnPointSet::draw(const Camera& camera){
	
	glEnable(GL_BLEND);

	auto shader = Globals::shaderManager.getAssetPointer("respawn");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix()));
	Globals::spritesheetManager.getAssetPointer("circle")->bind(0);
	Globals::shapeManager.get("quad_rp").drawRawInstanced();
	shader->unuse();

	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	shader = Globals::shaderManager.getAssetPointer("cylinder");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix()));
	shader->loadFloat("hmax", 3.0f);
	Globals::shapeManager.get("cylinder").drawRawInstanced();
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("disk");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	Globals::shapeManager.get("disk").drawRawInstanced();
	shader->unuse();

	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void RespawnPointSet::addInstances(const std::vector<Matrix4f>& values) {
	m_states = fromInstances(values);
}

void RespawnPointSet::update(const float dt) {
	bool update = false;

	for (unsigned int i = 0; i < m_states.size(); i++) {
		if ((m_playerPos - m_states[i].position).length() <= RADIUS + CIRCLE_RADIUS) {
			update = i != m_respawnId;
			m_respawnId = i;
		}
	}

	if (update) {
		updateUbo();
		Globals::soundManager.get("game").playChannel(2u);
	}

}

const std::vector<RespawnPointSet::State>& RespawnPointSet::fromInstances(const std::vector<Matrix4f>& instances) {
	m_states.clear();
	std::transform(instances.begin(), instances.end(), std::back_inserter(m_states), [](const Matrix4f& p)-> State { return{ { p[3][0], p[3][1] , p[3][2]} }; });
	return m_states;
}

//1. sizeof(bool) = 1 --> we have a padwidth of 16 to match the std 140 layout
//2. I highly recomand to use the fourth componente of color and set the 0.6 inside the shader insteed of passing a second bool array
void RespawnPointSet::updateUbo() {

	std::fill(m_colors.begin(), m_colors.end(), Vector4f(0.5f, 0.5f, 1.0f, 0.6f));
	m_colors[m_respawnId] = Vector4f(1.0f, 0.4f, 0.0f, 0.6f);

	std::fill(m_activate.begin(), m_activate.end(), std::array<bool, 16>{ false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false });
	m_activate[m_respawnId] = { true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };

	glBindBuffer(GL_UNIFORM_BUFFER, Globals::colorUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 128, &m_colors[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, Globals::activateUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 128, &m_activate[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

unsigned short& RespawnPointSet::getRespawnId() {
	return m_respawnId;
}

const Vector3f& RespawnPointSet::getActivePoistion() const{
	return m_states[m_respawnId].position;
}

void RespawnPointSet::deploy() {
	Globals::soundManager.get("game").playChannel(2u);
	m_respawnId = 0;
	updateUbo();
}