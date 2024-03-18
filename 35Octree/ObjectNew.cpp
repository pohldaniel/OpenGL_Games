#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ObjectNew.h"
#include "Globals.h"

ObjectNew::ObjectNew(Mesh* _mesh, glm::vec3 _scale, glm::vec3 _position, glm::vec3 _rotation)
{
	//assign attributes based on input parameters
	m_mesh = _mesh;
	m_modelMatrix = glm::mat4();
	m_rotation = _rotation;
	m_position = _position;
	m_scale = _scale;
}

void ObjectNew::Draw(const Camera& camera, const glm::vec3& rotation, const glm::vec3& lightPos){

	glm::mat4 viewMatrix = glm::rotate(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5f, -7.0f)), 0.2f, glm::vec3(1, 0, 0)), rotation.y + 3.1415f, glm::vec3(0, 1, 0));
	glm::mat4 projMatrix = glm::perspective(45.0f, 1.7f, 0.1f, 100.0f);

	auto shader = Globals::shaderManager.getAssetPointer("model");
	shader->use();
	shader->loadMatrix("projMat", glm::value_ptr(projMatrix));
	shader->loadMatrix("viewMat", glm::value_ptr(viewMatrix));
	shader->loadMatrix("modelMat", glm::value_ptr(m_modelMatrix));
	shader->loadVector("worldSpaceLightPosition", lightPos.x, lightPos.y, lightPos.z, 1.0f);
	shader->loadInt("colourTexture", 0);
	shader->loadInt("normalTexture", 1);

	Globals::textureManager.get("pine").bind(0u);
	Globals::textureManager.get("pine_normal").bind(1u);
	m_mesh->Draw();
	shader->unuse();
}

void ObjectNew::Draw2(const Camera& camera, const glm::vec3& rotation, const glm::vec3& lightPos) {

	glm::mat4 viewMatrix = glm::rotate(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5f, -7.0f)), 0.2f, glm::vec3(1, 0, 0)), rotation.y + 3.1415f, glm::vec3(0, 1, 0));
	glm::mat4 projMatrix = glm::perspective(45.0f, 1.7f, 0.1f, 100.0f);

	auto shader = Globals::shaderManager.getAssetPointer("model");
	shader->use();
	shader->loadMatrix("projMat", glm::value_ptr(projMatrix));
	shader->loadMatrix("viewMat", glm::value_ptr(viewMatrix));
	shader->loadMatrix("modelMat", glm::value_ptr(m_modelMatrix));
	shader->loadVector("worldSpaceLightPosition", lightPos.x, lightPos.y, lightPos.z, 1.0f);
	shader->loadInt("colourTexture", 0);
	shader->loadInt("normalTexture", 1);

	Globals::textureManager.get("grass").bind(0u);
	Globals::textureManager.get("grass_normal").bind(1u);
	m_mesh->Draw();
	shader->unuse();
}

void ObjectNew::Update()
{
	//update the model matrix
	m_modelMatrix = glm::translate(glm::mat4(1.0f),m_position);
	m_modelMatrix = glm::rotate(m_modelMatrix, m_rotation.y,glm::vec3(0,1.0f,0));
	m_modelMatrix = glm::rotate(m_modelMatrix, m_rotation.x,glm::vec3(1.0f,0,0));
	m_modelMatrix = glm::scale(m_modelMatrix,m_scale);
}