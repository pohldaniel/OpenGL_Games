#pragma once

#include <assimp/types.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>
#include <string>
#include <vector>
#include "ShaderVo.h"
#include <sstream>
#include <cstdlib>
#include <ctime>
#include "Transformation.h"
#include <memory>
#include "engine/Camera.h"

struct MeshVertex 
{
public:
	MeshVertex() {};
	~MeshVertex() {};

	glm::vec3 m_Position;	
	glm::vec2 m_TexCoords;
	glm::vec3 m_Normal;
};

struct MeshTexture 
{
public:
	MeshTexture() {};
	~MeshTexture() {};

	GLuint m_id;
	std::string m_type;
	aiString m_path;
};

class Mesh 
{
public:
	Mesh(std::vector<MeshVertex> vertices, std::vector<GLuint> indices, std::vector<MeshTexture> textures, bool instancing);

	glm::mat4* GetModelMatIns() { return m_modelMatricesIns; }

	void SetTransform(TransformVo& transform) { m_transform = transform; }
	void Draw(Camera& camera, ShaderVo program, bool instancing, glm::vec3& pos = glm::vec3(1.0f), glm::vec3& rot = glm::vec3(1.0f), float amountOfRotation = 1.0f,
			  glm::vec3& scale = glm::vec3(1.0f), bool bDrawRelativeToCamera = false, bool bUseSpotlight = false);

private:
	GLuint m_vao, m_vbo, m_ebo;
	TransformVo m_transform;
	glm::mat4* m_modelMatricesIns;
	float m_totalObjectsIns;
	std::vector<MeshVertex> m_vertices;
	std::vector<GLuint> m_indices;
	std::vector<MeshTexture> m_textures;

	void CreateMesh(bool instancing);
};