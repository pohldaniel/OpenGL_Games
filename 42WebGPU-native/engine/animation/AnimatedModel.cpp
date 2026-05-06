#include "AnimatedModel.h"

AnimatedModel::AnimatedModel() {

}

AnimatedModel::~AnimatedModel() {
	for (AnimatedMesh* mesh : m_meshes) {
		delete mesh;
	}
}

AnimatedMesh::AnimatedMesh(AnimatedModel* model) : m_model(model) {
	m_model = model;
}

AnimatedMesh::~AnimatedMesh() {
}