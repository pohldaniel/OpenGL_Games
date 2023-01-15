#include <iostream>
#include <fstream>

#include "AnimationShader.h"
#include "../AnimatedModel/AnimatedModel.h"
#include "../AssimpAnimatedModel.h"

AnimationShader::AnimationShader(const std::string& vertex, const std::string& fragment) : Shader(vertex.c_str(), fragment.c_str()) {

}

void AnimationShader::update(const AnimatedModel& model, const Camera& camera, std::vector<Matrix4f> jointVector){

	loadMatrixArray("jointTransforms", jointVector, jointVector.size() < MAX_JOINTS ? jointVector.size() : MAX_JOINTS);
	loadMatrix("u_model", model.getTransformationMatrix());
	loadMatrix("u_view", camera.getViewMatrix());
	loadMatrix("u_projection", camera.getProjectionMatrix());
}

void AnimationShader::update(const AssimpAnimatedModel& model, const Camera& camera, std::vector<Matrix4f> jointVector) {
	loadMatrixArray("jointTransforms", jointVector, jointVector.size() < MAX_JOINTS ? jointVector.size() : MAX_JOINTS);
	loadMatrix("u_model", model.getTransformationMatrix());
	loadMatrix("u_view", camera.getViewMatrix());
	loadMatrix("u_projection", camera.getProjectionMatrix());
	loadVector("u_light", Vector3f(1.0f, 1.0f, 1.0f));
}

void AnimationShader::bind(){
	glUseProgram(m_program);
}

AnimationShader::~AnimationShader(){
	glDeleteProgram(m_program);
}