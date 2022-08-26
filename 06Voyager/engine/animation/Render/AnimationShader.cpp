#include <iostream>
#include <fstream>

#include "AnimationShader.h"
#include "../AnimatedModel/AnimatedModel.h"
#include "../AssimpAnimatedModel.h"

AnimationShader::AnimationShader(const std::string& vertex, const std::string& fragment) : Shader(vertex, fragment) {

}

void AnimationShader::update(const AnimatedModel& model, const Camera& camera, std::vector<Matrix4f> jointVector){

	loadMatrixArray("jointTransforms", jointVector, jointVector.size() < MAX_JOINTS ? jointVector.size() : MAX_JOINTS);
	loadMatrix("u_model", model.getTransformationMatrix());
	loadMatrix("u_view", camera.getViewMatrix());
	loadMatrix("u_projection", camera.getProjectionMatrix());
}

void AnimationShader::update(const AssimpAnimatedModel& model, const Camera& camera, std::vector<Matrix4f> jointVector) {

	/*std::cout << "Bone Matrices: ##############################################################" << std::endl;
	for (int i = 0; i < jointVector.size(); ++i) {
		std::cout << jointVector[i][0][0] << "  " << jointVector[i][0][1] << "  " << jointVector[i][0][2] << "  " << jointVector[i][0][3] << std::endl;
		std::cout << jointVector[i][1][0] << "  " << jointVector[i][1][1] << "  " << jointVector[i][1][2] << "  " << jointVector[i][1][3] << std::endl;
		std::cout << jointVector[i][2][0] << "  " << jointVector[i][2][1] << "  " << jointVector[i][2][2] << "  " << jointVector[i][2][3] << std::endl;
		std::cout << jointVector[i][3][0] << "  " << jointVector[i][3][1] << "  " << jointVector[i][3][2] << "  " << jointVector[i][3][3] << std::endl;
		std::cout << "--------------" << std::endl;
	}*/

	loadMatrixArray("jointTransforms", jointVector, jointVector.size() < MAX_JOINTS ? jointVector.size() : MAX_JOINTS);
	loadMatrix("u_model", model.getTransformationMatrix());
	loadMatrix("u_view", camera.getViewMatrix());
	loadMatrix("u_projection", camera.getProjectionMatrix());
}

void AnimationShader::bind(){
	glUseProgram(m_program);
}

AnimationShader::~AnimationShader(){
	glDeleteProgram(m_program);
}