#include "Scene.h"
#include "Globals.h"

//constructor of the scene, used to generate the scene (further expansion could be done here to load a scene from a file)
Scene::Scene(void)
{
	//set the initial light position
	m_lightPosition = glm::vec4(0,0.1,0,1);
	//start the light moving up
	m_fireUp = true;
	m_rotateRight = false;
	m_rotateLeft = false;
	
	//create meshes and materials and push objects to the back of the list
	m_grassMesh = new Mesh("res/models/Grass.obj");
	m_deltaTime = 0;
	//m_grass = new ObjectNew(m_grassMesh, m_grassMat, &m_deltaTime, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, -0.3f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	m_objects.push_back(new ObjectNew(m_grassMesh, glm::vec3(0.5,0.5,0.5), glm::vec3(0,-0.3,0), glm::vec3(0,0,0)));
	
	m_logMesh = new Mesh("res/models/Log_pine.obj");

	m_objects.push_back(new ObjectNew(m_logMesh, glm::vec3(0.1f,0.1f,0.1f), glm::vec3(-3.0f,0.1,0), glm::vec3(3.1415,0,0)));
	m_objects.push_back(new ObjectNew(m_logMesh, glm::vec3(0.1f,0.1f,0.1f), glm::vec3(3.0f,0.1,0), glm::vec3(3.1415,0,0)));
	m_objects.push_back(new ObjectNew(m_logMesh, glm::vec3(0.1f,0.1f,0.1f), glm::vec3(0,0.1,-3.0f), glm::vec3(3.1415,1.57079632679f,0)));
	m_objects.push_back(new ObjectNew(m_logMesh, glm::vec3(0.1f,0.1f,0.1f), glm::vec3(0,0.1,3.0f), glm::vec3(3.1415,1.57079632679f,0)));
	m_fires.push_back(new Flame());
	
	//set initial rotation and deltatime
	m_cameraRotation = glm::vec3(0,0,0);
	m_previousTime = Globals::clock.getElapsedTimeMilli();
	
}

//function to update each object in the scene and rotate the camera and light
void Scene::Update(float dt)
{
	//rotate the camera based on input
	if (m_rotateRight)
	{
		m_cameraRotation.y -= 0.4f*dt;
	}
	if (m_rotateLeft)
	{
		m_cameraRotation.y += 0.4f*dt;
	}

	//move the fire up or down to simulate flickering on surroundings
	if(m_fireUp)
	{
		m_lightPosition.y += 0.6f*dt;
		if (m_lightPosition.y>2.0f)
		{
			m_fireUp = false;
		}
	}
	else
	{
		m_lightPosition.y -= 0.6f*dt;
		if (m_lightPosition.y<1.5f)
		{
			m_fireUp = true;
		}
	}

	//loop through all objcts and fires and update them
	for( auto i : m_objects)
	{
		i->Update();
	}
	for (auto i : m_fires)
	{
		i->Update(dt);
	}
	
}

//function to draw all objects and flames
void Scene::Draw(const Camera& camera){

	//clear the buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//m_grass->Draw2(camera, m_cameraRotation, m_lightPosition);
	//loop through all objects and fires and draw them

	m_objects[0]->Draw2(camera, m_cameraRotation, m_lightPosition);

	for (int i = 1; i < m_objects.size(); i++){
		m_objects[i]->Draw(camera, m_cameraRotation, m_lightPosition);
	}

	for (auto i : m_fires){
		i->Draw(camera, m_cameraRotation, m_lightPosition);
	}
}