#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <engine/Camera.h>
#include "ObjectNew.h"
#include "Mesh.h"
#include "Flame.h"

class Scene {

private:
	std::vector<ObjectNew*> m_objects;
	ObjectNew* m_grass;
	std::vector<Flame*> m_fires;
	
	Mesh *m_logMesh;
	Mesh *m_grassMesh;
	glm::vec3 m_cameraRotation;
	glm::vec4 m_lightPosition;
	unsigned int m_deltaTime;
	unsigned int m_previousTime;
	bool m_fireUp;
	bool m_rotateRight;
	bool m_rotateLeft;
public:
	Scene(void);
	void Draw(const Camera& camera);
	void Update(float dt);
	void SetDeltaTime(unsigned int _deltaTime){m_deltaTime = _deltaTime;}
	void SetRotateRight(bool _rotate){m_rotateRight = _rotate;}
	void SetRotateleft(bool _rotate){m_rotateLeft = _rotate;}
};

#endif