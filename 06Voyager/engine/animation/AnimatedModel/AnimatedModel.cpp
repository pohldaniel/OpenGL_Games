#include <iostream>
#include <fstream>

#include "..\..\Texture.h"
#include "AnimatedModel.h"

AnimatedModel::AnimatedModel() : m_shader("./res/shader/animationShader.vert", "./res/shader/animationShader.frag") {
	m_animator = std::make_shared<Animator>(this);
	m_transform = Transform();
}

void AnimatedModel::loadModel(const std::string &filename, const std::string &texture){
	
	ColladaLoader loader(filename);

	m_meshes.push_back(std::make_shared<AnimatedMesh>(loader));
	
	m_animator->addAnimation(loader);
	m_texture = std::make_shared<Texture>(texture);
}


void AnimatedModel::update(double elapsedTime){
	m_animator->update(elapsedTime);
}

void AnimatedModel::draw(Camera camera){
	
	m_shader.bind();
	for (auto mesh : m_meshes){
		
		m_texture->bind(0);
		m_shader.update(*this, camera, mesh->getBoneArray());

		mesh->draw();
	}
}


void AnimatedModel::setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz) {
	m_transform.setRotPos(axis, degrees, dx, dy, dz);
}

void AnimatedModel::rotate(const Vector3f &axis, float degrees) {
	m_transform.rotate(axis, degrees);
}

void AnimatedModel::translate(float dx, float dy, float dz) {
	m_transform.translate(dx, dy, dz);
}

void AnimatedModel::scale(float a, float b, float c) {
	m_transform.scale(a, b, c);
}

void AnimatedModel::scale(float s) {
	m_transform.scale(s, s, s);
}

const Matrix4f &AnimatedModel::getTransformationMatrix() const {
	return m_transform.getTransformationMatrix();
}

const Matrix4f &AnimatedModel::getInvTransformationMatrix() {
	return m_transform.getInvTransformationMatrix();
}
