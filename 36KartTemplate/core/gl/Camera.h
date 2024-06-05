#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include <memory> // For std::unique_ptr

//For User Input on Windows
#include <SDL.h>

#include"shaderClass.h"

class Camera
{
public:
	// Stores the main vectors of the camera
	bool DEBUG;
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 LookAt;
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 viewProjection;

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 1.0f;
	float sensitivity = 50.0f;

	// Camera constructor to set up initial values
	Camera(int width, int height, glm::vec3 position);

	// Updates and exports the camera matrix to the Vertex Shader
	void Matrix(float FOVdeg, float nearPlane, float farPlane, std::shared_ptr<Shader>& shader, const char* uniform);
	
	void Inputs(SDL_Window* window, bool isMouse);
	void ProcessMouseLook(int mouseXRel, int mouseYRel, SDL_Window* window);

	void VehicleFollowCamera(float pX, float pY, float pZ);
};
#endif