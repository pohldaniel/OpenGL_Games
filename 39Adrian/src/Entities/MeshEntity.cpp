#include "MeshEntity.h"

MeshEntity::MeshEntity(const AssimpModel& model) : MeshNode(model), Entity() {

}

MeshEntity::~MeshEntity() {

}

void MeshEntity::draw() {
	MeshEntity::drawRaw();
}