#include <engine/Sprite.h>

#include <States/StateMachine.h>
#include <Entities/Md2Entity.h>
#include "MiniMap.h"
#include "Globals.h"

MiniMap::MiniMap(const IsometricCamera& camera, const Scene& scene, const std::vector<Md2Entity*>& entities) : camera(camera), scene(scene), entities(entities), m_vao(0u), m_vbo(0u){
	
}

MiniMap::~MiniMap() {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0u;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0u;
	}
}

void MiniMap::init() {
	updateEntitiePositions();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float) * m_entitiesPosCol.size(), NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MiniMap::draw() {

	auto shader = Globals::shaderManager.getAssetPointer("shape_color");
	shader->use();
	Globals::textureManager.get("ground").bind(0);
	shader->loadMatrix("u_projection", Sprite::GetOrthographic());
	shader->loadMatrix("u_view", Matrix4f::IDENTITY);
	shader->loadVector("u_color", Vector4f(0.8f, 0.8f, 0.8f, 1.0f));
	shader->loadMatrix("u_model", Matrix4f::Translate((565.0f / 640.0f) * 1024.0f, (75.0f / 480.0f) * 768.0f, 0.0f) *
		Matrix4f::Rotate(Vector3f(0.0f, 0.0f, -1.0f), camera.getAngle() * _180_ON_PI) *
		Matrix4f::Scale(1000.0f / scene.xconvfactor, 1000.0f / scene.yconvfactor, 0.0f));
	Globals::shapeManager.get("quad_xy").drawRaw();


	shader->loadVector("u_color", Vector4f::ONE);
	for (int i = 0; i < scene.buildingsMiniMap.size(); i++) {
		shader->loadMatrix("u_model", Matrix4f::Translate(scene.buildingsMiniMap[i][0] / scene.xconvfactor, scene.buildingsMiniMap[i][1] / scene.yconvfactor, 0.0f) *
			Matrix4f::Translate((565.0f / 640.0f) * 1024.0f, (75.0f / 480.0f) * 768.0f, 0.0f) *
			Matrix4f::Rotate(Vector3f(0.0f, 0.0f, -1.0f), camera.getAngle() * _180_ON_PI, Vector3f(-scene.buildingsMiniMap[i][0] / scene.xconvfactor, -scene.buildingsMiniMap[i][1] / scene.yconvfactor, 0.0f)) *
			Matrix4f::Scale(scene.buildingsMiniMap[i][2] / scene.xconvfactor, scene.buildingsMiniMap[i][3] / scene.yconvfactor, 0.0f));
		Globals::shapeManager.get("quad_xy_nt").drawRaw();
	}

	Globals::textureManager.get("null").bind(0);
	updateEntitiePositions();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * sizeof(float) * m_entitiesPosCol.size(), &m_entitiesPosCol[0]);
	shader = Globals::shaderManager.getAssetPointer("points");
	shader->use();
	shader->loadMatrix("u_projection", Sprite::GetOrthographic());
	shader->loadMatrix("u_view", Matrix4f::IDENTITY);
	shader->loadMatrix("u_model", Matrix4f::Translate((565.0f / 640.0f) * 1024.0f, (75.0f / 480.0f) * 768.0f, 0.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	shader->loadFloat("u_size", 0.01f);
	shader->loadFloat("u_ratio", camera.getAspect(true));

	glDrawArrays(GL_POINTS, 0, m_entitiesPosCol.size());
	shader->unuse();

	glLineWidth(2.0f);
	float hm = (21.33f / 640.0f) * 1024.0f;
	float vm = (21.33f / 480.0f) * 768.0f;

	Vector3f pos = Matrix4f::RotateVec(Vector3f(0.0f, -1.0f, 0.0f), camera.getAngle() * _180_ON_PI, camera.getPosition());
	shader = Globals::shaderManager.getAssetPointer("view");
	shader->use();
	shader->loadMatrix("u_projection", Sprite::GetOrthographic());
	shader->loadMatrix("u_view", Matrix4f::IDENTITY);
	shader->loadMatrix("u_model", Matrix4f::Translate(pos[0] / scene.xconvfactor, -pos[2] / scene.yconvfactor, 0.0f) *
		Matrix4f::Translate((565.0f / 640.0f) * 1024.0f, (75.0f / 480.0f) * 768.0f, 0.0f) *
		Matrix4f::Scale(hm, vm, 0.0f));
	shader->loadVector("u_color", Vector4f::ONE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Globals::shapeManager.get("quad_xy").drawRaw();

	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
	shader->unuse();
}

void MiniMap::updateEntitiePositions() {
	m_entitiesPosCol.clear();

	Matrix4f rot = Matrix4f::Rotate(Vector3f(0.0f, -1.0f, 0.0f), camera.getAngle() * _180_ON_PI);
	Vector3f pos = entities[0]->getWorldPosition() * rot;
	m_entitiesPosCol.push_back({ pos[0] / scene.xconvfactor, -pos[2] / scene.yconvfactor, 1.0f, 1.0f, 1.0f, 1.0f });

	std::transform(entities.begin() + 1, entities.end(), std::back_inserter(m_entitiesPosCol), [this, &rot = rot](Md2Entity* p)->std::array<float, 6> {
		Vector3f pos = p->getWorldPosition() * rot;
		return   { pos[0] / scene.xconvfactor, -pos[2] / scene.yconvfactor, 1.0f, 0.0f, 0.0f, 1.0f };
	});
}