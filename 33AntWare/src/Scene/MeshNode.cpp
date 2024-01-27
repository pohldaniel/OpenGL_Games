#include <Objects/MeshNew.h>

#include "MeshNode.h"

void MeshNode::OnWorldBoundingBoxUpdate() const{
	if (m_mesh)
		worldBoundingBox = m_mesh->getLocalBoundingBox().transformed(getTransformation());
	else
		worldBoundingBox.define(m_position);
}