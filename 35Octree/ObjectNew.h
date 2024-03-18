#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <memory>
#include <engine/Camera.h>
#include "Mesh.h"

//object class containing references to a material and a mesh as well as model scale, rotation and position in the form of a model matrix
//also contains functions to update and draw the object
class ObjectNew
{
private:
	Mesh* m_mesh;
	unsigned int* m_deltaTime;
	glm::mat4 m_modelMatrix;
	glm::vec3 m_scale;
	glm::vec3 m_position;
	glm::vec3 m_rotation;
public:

	ObjectNew(Mesh* _mesh, glm::vec3 _scale, glm::vec3 _position, glm::vec3 _rotation);
	void Draw(const Camera& camera, const glm::vec3& rotation, const glm::vec3& lightPos);
	void Draw2(const Camera& camera, const glm::vec3& rotation, const glm::vec3& lightPos);
	void Update();
};

#endif