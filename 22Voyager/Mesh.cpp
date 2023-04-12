#include "Mesh.h"
#include "Player.h"
#include <iostream>

Mesh::Mesh(std::vector<MeshVertex> vertices, std::vector<GLuint> indices, std::vector<MeshTexture> textures, bool instancing)
{
	m_vertices = vertices;
	m_indices = indices;
	m_textures = textures;
	CreateMesh(instancing);
}

void Mesh::Draw(Camera& camera, ShaderVo shaderProgram, bool instancing, glm::vec3& pos, glm::vec3& rot, float amountOfRotation, glm::vec3& scale, bool bDrawRelativeToCamera, bool bUseSpotlight)
{
	shaderProgram.ActivateProgram();

	glm::mat4 model(1.0f);
	glm::mat4 translation = glm::translate(pos);
	glm::mat4 rotation = glm::rotate(amountOfRotation, rot);
	glm::mat4 scaleMat = glm::scale(scale);

	if (bDrawRelativeToCamera)
	{
		Matrix4f _invView = camera.getInvViewMatrix();

		glm::mat4 invViewMat;


		invViewMat[0][0] = _invView[0][0]; invViewMat[0][1] = _invView[0][1]; invViewMat[0][2] = _invView[0][2]; invViewMat[0][3] = _invView[0][3];
		invViewMat[1][0] = _invView[1][0]; invViewMat[1][1] = _invView[1][1]; invViewMat[1][2] = _invView[1][2]; invViewMat[1][3] = _invView[1][3];
		invViewMat[2][0] = _invView[2][0]; invViewMat[2][1] = _invView[2][1]; invViewMat[2][2] = _invView[2][2]; invViewMat[2][3] = _invView[2][3];
		invViewMat[3][0] = _invView[3][0]; invViewMat[3][1] = _invView[3][1]; invViewMat[3][2] = _invView[3][2]; invViewMat[3][3] = _invView[3][3];

		model = invViewMat * translation * rotation * scaleMat;
	}
	else
	{
		model = translation * rotation * scaleMat;
	}

	shaderProgram.SetMat4("model", model);
	//shaderProgram.SetVec3("lightPos", glm::vec3(cam.GetCameraPos().x, cam.GetCameraPos().y + 5.0f, cam.GetCameraPos().z));
	//shaderProgram.SetVec3("viewPos", glm::vec3(cam.GetCameraPos().x, cam.GetCameraPos().y, cam.GetCameraPos().z));
	shaderProgram.loadVector("lightPos", Vector3f(camera.getPosition()[0], camera.getPosition()[1] + 5.0f, camera.getPosition()[2]));
	shaderProgram.loadVector("viewPos", camera.getPosition());

	shaderProgram.SetBool("EnableSpotlight", bUseSpotlight);

	if (Player::GetInstance().GetSpotLight() != nullptr && bUseSpotlight)
	{
		shaderProgram.loadVector("spotlight.position", Player::GetInstance().GetSpotLight()->getPosition());
		shaderProgram.loadVector("spotlight.direction", Player::GetInstance().GetSpotLight()->getDirection());
		shaderProgram.loadVector("spotlight.diffuse", Player::GetInstance().GetSpotLight()->getDiffuse());
		shaderProgram.loadVector("spotlight.specular", Player::GetInstance().GetSpotLight()->getSpecular());
		shaderProgram.SetFloat("spotlight.constant", Player::GetInstance().GetSpotLight()->getConstant());
		shaderProgram.SetFloat("spotlight.linear", Player::GetInstance().GetSpotLight()->getLinear());
		shaderProgram.SetFloat("spotlight.quadratic", Player::GetInstance().GetSpotLight()->getQuadratic());
		shaderProgram.SetFloat("spotlight.cutOff", glm::cos(glm::radians(Player::GetInstance().GetSpotLight()->getCutOff())));
		shaderProgram.SetFloat("spotlight.outerCutOff", glm::cos(glm::radians(Player::GetInstance().GetSpotLight()->getOuterCutOff())));
	}

	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	
	for (unsigned int i = 0; i < m_textures.size(); ++i){
		glActiveTexture(GL_TEXTURE0 + i);
	
		std::string number;
		std::string name = m_textures[i].m_type;
	
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
	
		
		shaderProgram.loadMatrix("projection", camera.getPerspectiveMatrix());
		shaderProgram.loadMatrix("view", camera.getViewMatrix());

		glUniform1i(glGetUniformLocation(shaderProgram.GetShaderProgram(), (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i].m_id);
	}
	
	glBindVertexArray(m_vao);

	if (instancing)
	{
		glDrawElementsInstanced(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0, m_totalObjectsIns);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);

	// Unbind textures 
	for (GLuint i = 0; i < m_textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::CreateMesh(bool instancing)
{
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);
 
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);
 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)offsetof(MeshVertex, m_Normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (GLvoid*)offsetof(MeshVertex, m_TexCoords));

	if (instancing)
	{
		m_totalObjectsIns = 20000;
		unsigned int amount = m_totalObjectsIns;
		m_modelMatricesIns = new glm::mat4[amount];
		float radius = 400.0f;
		float offset = 60.0f;

		for (unsigned int i = 0; i < amount; ++i)
		{
			glm::mat4 model(1.0f);
			float x = 0.0f, y = 0.0f, z = 0.0f;

			float angle = (float)i / (float)amount * 360.0f;
			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset + 300.0f;
			x = sin(angle) * radius + displacement;

			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset + 40.0f;
			y = displacement * 3.7f;

			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset + 390.0f;
			z = cos(angle) * radius + displacement;

			model = glm::translate(model, glm::vec3(x, y, z));

			float scale = (rand() % 25) / 100.0f + 0.35f;
			model = glm::scale(model, glm::vec3(scale));

			float rotAngle = (rand() % 360);
			model = glm::rotate(model, rotAngle, glm::vec3(0.5f, 0.7f, 0.9f));

			m_modelMatricesIns[i] = model;
		}

		unsigned int instanceVBO;
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * amount, &m_modelMatricesIns[0], GL_STATIC_DRAW);

		for (unsigned int i = 0; i < 1; ++i)
		{
			glBindVertexArray(m_vao);

			GLsizei mat4Size = sizeof(glm::mat4);
			glEnableVertexAttribArray(3);
			glEnableVertexAttribArray(4);
			glEnableVertexAttribArray(5);
			glEnableVertexAttribArray(6);

			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, mat4Size, (GLvoid*)0);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, mat4Size, (GLvoid*)(sizeof(glm::vec4)));
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, mat4Size, (GLvoid*)(2 * sizeof(glm::vec4)));
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, mat4Size, (GLvoid*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}
	}

	glBindVertexArray(0);
}