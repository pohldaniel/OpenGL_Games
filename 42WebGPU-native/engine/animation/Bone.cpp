#include <functional>
#include <algorithm>

#include "Bone.h"

thread_local Matrix4f Bone::Transformation;

Bone::Bone() : m_parent(nullptr), m_numChildBones(0u), m_isRootBone(false), m_isDirty(true), m_animationEnabled(true) {

}

Bone::~Bone() {
	eraseAllChildren();
}

void Bone::OnTransformChanged() {
	if (m_isDirty) {
		return;
	}

	std::for_each(m_children.begin(), m_children.end(),
		[](std::unique_ptr<Bone>& bone) { bone->OnTransformChanged(); }
	);

	m_isDirty = true;
}

const Matrix4f& Bone::getWorldTransformation() const {
	if (m_isDirty) {
		m_modelMatrix = getTransformationSOP();
		if (m_parent)
			m_modelMatrix = m_parent->getWorldTransformation() * m_modelMatrix;

		m_isDirty = false;
	}
	return m_modelMatrix;
}

Bone* Bone::addChild(Bone* node) {
	m_children.emplace_back(std::unique_ptr<Bone>(node));
	m_children.back()->m_parent = this;
	return m_children.back().get();
}

void Bone::eraseChild(Bone* child) {
	if (!child || child->m_parent != this)
		return;

	child->m_parent = nullptr;
	m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [child](const std::unique_ptr<Bone>& node) { return node.get() == child; }), m_children.end());
}

void Bone::eraseAllChildren(size_t offset) {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		Bone* child = (*it).release();
		child->m_parent = nullptr;
		delete child;
		child = nullptr;
	}
	m_children.erase(std::next(m_children.begin(), offset), m_children.end());
}

Bone* Bone::findChild(const std::string& name, bool recursive) const {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		Bone* child = (*it).get();
		if (!child) {
			continue;
		}

		if (child->m_name == name && dynamic_cast<Bone*>(child) != nullptr)
			return dynamic_cast<Bone*>(child);
		else if (recursive && child->m_children.size()) {
			Bone* result = child->findChild(name, recursive);
			if (result)
				return dynamic_cast<Bone*>(result);
		}
	}
	return nullptr;
}

const Matrix4f& Bone::getTransformationSOP() const {
	Transformation.translate(m_position);
	Transformation *= Matrix4f::Rotate(m_orientation);
	Transformation *= Matrix4f::Scale(m_scale);
	return Transformation;
}

const std::string& Bone::getName() const {
	return m_name;
}

const Vector3f& Bone::getScale() const {
	return m_scale;
}

void Bone::setPosition(const Vector3f& position) {
	m_position = position;
	OnTransformChanged();
}

void Bone::setOrientation(const Quaternion& orientation) {
	m_orientation = orientation;
	OnTransformChanged();
}

void Bone::setScale(const Vector3f& scale) {
	m_scale = scale;
	OnTransformChanged();
}

void Bone::setName(const std::string& name) {
	m_name = name;
}

void Bone::setParent(Bone* node) {

	if (node && m_parent) {
		std::list<std::unique_ptr<Bone>>::iterator it = std::find_if(m_parent->m_children.begin(), m_parent->m_children.end(), [node](std::unique_ptr<Bone>& _node) { return _node.get() == node; });
		if (it != m_parent->m_children.end()) {
			node->m_children.splice(m_parent->m_children.end(), m_parent->m_children, it);
		}
	}
	else if (node) {
		node->addChild(this);
	}
	else if (m_parent) {
		m_parent->eraseChild(this);
		m_parent = nullptr;
	}
}

void Bone::setIsRootBone(bool rootBone) {
	m_isRootBone = rootBone;
}

const bool Bone::isRootBone() const {
	return m_isRootBone;
}

bool Bone::animationEnabled() const {
	return m_animationEnabled;
}

size_t Bone::getNumChildBones() const {
	return m_numChildBones;
}

void Bone::setTransformSilent(const Vector3f& position, const Quaternion& rotation, const Vector3f& scale) {
	m_position = position;
	m_orientation = rotation;
	m_scale = scale;
}

void Bone::countChildBones() {
	m_numChildBones = 0;
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		if (dynamic_cast<Bone*>((*it).get()))
			++m_numChildBones;
	}
}

void Bone::rotate(const float pitch, const float yaw, const float roll) {
	m_orientation.rotate(pitch, yaw, roll);
	OnTransformChanged();
}