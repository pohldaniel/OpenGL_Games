#include "Widget.h"

namespace ui
{
	Node::Node() : m_parent(nullptr), m_index(-1) {

	}

	Node::Node(const Node& rhs) {
		m_children = rhs.m_children;
		for (auto& children : m_children) {
			children->m_parent = this;
		}
		m_parent = rhs.m_parent;
		m_nameHash = rhs.m_nameHash;
		m_index = rhs.m_index;
	}

	Node::Node(Node&& rhs) {
		m_children = rhs.m_children;
		for (auto& children : m_children) {
			children->m_parent = this;
		}
		m_parent = rhs.m_parent;
		m_nameHash = rhs.m_nameHash;
		m_index = rhs.m_index;
	}

	Node::~Node() {
		eraseAllChildren();
	}

	std::list<std::shared_ptr<Node>>& Node::getChildren() const {
		return m_children;
	}

	void Node::eraseSelf() {
		if (m_parent)
			m_parent->eraseChild(this);
	}

	void Node::eraseChild(Node* child) {
		if (!child || child->m_parent != this)
			return;

		child->m_parent = nullptr;
		m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [child](const std::shared_ptr<Node>& node) { return node.get() == child; }), m_children.end());
	}

	void Node::eraseChild(const int index) {
		m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [index](const std::shared_ptr<Node>& node) { return node->getIndex() == index; }), m_children.end());
	}

	Node* Node::addChild(Node* node) {
		m_children.emplace_back(std::shared_ptr<Node>(node));
		m_children.back()->m_parent = this;
		return m_children.back().get();
	}

	Node* Node::getParent() const {
		return m_parent;
	}

	const int Node::getIndex() const {
		return m_index;
	}

	void Node::setParent(Node* node) {
		if (node && m_parent) {
			std::list<std::shared_ptr<Node>>::iterator it = std::find_if(m_parent->getChildren().begin(), m_parent->getChildren().end(), [node](std::shared_ptr<Node>& _node) { return _node.get() == node; });

			if (it != m_parent->getChildren().end()) {
				node->getChildren().splice(m_parent->getChildren().end(), m_parent->getChildren(), it);
			}
		}else if (node) {
			node->addChild(this);
		}else if (m_parent) {
			m_parent->eraseChild(this);
		}
	}

	void Node::setName(const std::string& name) {
		m_nameHash = StringHash(name);
	}

	void Node::setIndex(const int index) {
		m_index = index;
	}

	void Node::eraseAllChildren(size_t offset) {
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {

			if (!(*it)->getChildren().empty())
				(*it)->eraseAllChildren();
			(*it)->m_parent = nullptr;
		}
		m_children.erase(std::next(m_children.begin(), offset), m_children.end());
	}

	size_t Node::countNodes() {
		size_t num = m_children.size();
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			num += (*it)->countNodes();
		}
		return num;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	Vector2f Widget::WorldPosition;
	Vector2f Widget::WorldScale;
	float Widget::WorldOrientation;

	Widget::Widget() : Node(), Sprite(), m_isDirty(true), m_draw(nullptr) {

	}

	Widget::Widget(const Widget& rhs) : Node(rhs), Sprite(rhs), m_draw(rhs.m_draw) {
		m_isDirty = rhs.m_isDirty;
	}

	Widget::Widget(Widget&& rhs) : Node(rhs), Sprite(rhs), m_draw(std::move(rhs.m_draw)) {
		m_isDirty = rhs.m_isDirty;
	}

	Widget::~Widget() {

	}

	void Widget::draw() {
		if (m_draw) {
			return m_draw();
		}
		drawDefault();
		drawTree();
		
	}

	void Widget::drawTree() {
		if (m_children.size() > 0) {
			for (std::list<std::shared_ptr<Node>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
				std::static_pointer_cast<Widget>(*it)->draw();
			}
		}
	}

	void Widget::OnTransformChanged() {

		if (m_isDirty) {
			return;
		}

		for (auto& child : m_children) {
			std::static_pointer_cast<Widget>(child)->OnTransformChanged();
		}

		m_isDirty = true;
	}

	const Matrix4f& Widget::getWorldTransformation() const {
		if (m_isDirty) {
			if (m_parent) {
				m_modelMatrix = static_cast<Widget*>(m_parent)->getWorldTransformation() * getTransformationSOP();
			}else {
				m_modelMatrix = getTransformationSOP();
			}
			m_isDirty = false;
		}

		return m_modelMatrix;
	}

	const Matrix4f Widget::getWorldTransformationWithTranslation(const Vector2f& trans) const {
		Matrix4f mtx = getWorldTransformation();
		if (m_parent) {
			const Vector2f invScale = Vector2f::Inverse(static_cast<Widget*>(m_parent)->getWorldScale());
			float dx = trans[0] * invScale[0];
			float dy = trans[1] * invScale[1];
			mtx[3][0] = mtx[3][0] + dx * mtx[0][0] + dy * mtx[1][0];
			mtx[3][1] = mtx[3][1] + dx * mtx[0][1] + dy * mtx[1][1];
		}else {
			mtx[3][0] = mtx[3][0] + trans[0] * mtx[0][0] + trans[1] * mtx[1][0];
			mtx[3][1] = mtx[3][1] + trans[0] * mtx[0][1] + trans[1] * mtx[1][1];
		}
		return mtx;
	}

	const Matrix4f Widget::getWorldTransformationWithTranslation(float dx, float dy) const {
		Matrix4f mtx = getWorldTransformation();
		if (m_parent) {
			const Vector2f invScale = Vector2f::Inverse(static_cast<Widget*>(m_parent)->getWorldScale());
			dx = dx * invScale[0];
			dy = dy * invScale[1];
			mtx[3][0] = mtx[3][0] + dx * mtx[0][0] + dy * mtx[1][0];
			mtx[3][1] = mtx[3][1] + dx * mtx[0][1] + dy * mtx[1][1];
		}else {
			mtx[3][0] = mtx[3][0] + dx * mtx[0][0] + dy * mtx[1][0];
			mtx[3][1] = mtx[3][1] + dx * mtx[0][1] + dy * mtx[1][1];
		}
		return mtx;
	}

	const Matrix4f Widget::getWorldTransformationWithScale(float sx, float sy, bool relative) const {
		Matrix4f mtx = getWorldTransformation();
		if (m_parent && relative) {
			const Vector2f invScale = Vector2f::Inverse(static_cast<Widget*>(m_parent)->getWorldScale());
			sx = sx * invScale[0];
			sy = sy * invScale[1];
			mtx[0][0] = mtx[0][0] * sx;  mtx[1][0] = mtx[1][0] * sy;
			mtx[0][1] = mtx[0][1] * sx;  mtx[1][1] = mtx[1][1] * sy;
		}else {
			mtx[0][0] = mtx[0][0] * sx;  mtx[1][0] = mtx[1][0] * sy;
			mtx[0][1] = mtx[0][1] * sx;  mtx[1][1] = mtx[1][1] * sy;
		}
		return mtx;
	}

	const Matrix4f Widget::getWorldTransformationWithScale(const Vector2f& scale, bool relative) const {
		return getWorldTransformationWithScale(scale[0], scale[1], relative);
	}

	const Matrix4f Widget::getWorldTransformationWithScale(const float s, bool relative) const {
		return getWorldTransformationWithScale(s, s, relative);
	}

	const Matrix4f Widget::getWorldTransformationWithScaleAndTranslation(float sx, float sy, float dx, float dy, bool relative) const {
		Matrix4f mtx = getWorldTransformation();
		if (m_parent && relative) {
			const Vector2f invScale = Vector2f::Inverse(static_cast<Widget*>(m_parent)->getWorldScale());

			dx = dx * invScale[0];
			dy = dy * invScale[1];

			mtx[3][0] = mtx[3][0] + dx * mtx[0][0] + dy * mtx[1][0];
			mtx[3][1] = mtx[3][1] + dx * mtx[0][1] + dy * mtx[1][1];

			sx = sx * invScale[0];
			sy = sy * invScale[1];

			mtx[0][0] = mtx[0][0] * sx;  mtx[1][0] = mtx[1][0] * sy;
			mtx[0][1] = mtx[0][1] * sx;  mtx[1][1] = mtx[1][1] * sy;

		}
		else {
			mtx[3][0] = mtx[3][0] + dx * mtx[0][0] + dy * mtx[1][0];
			mtx[3][1] = mtx[3][1] + dx * mtx[0][1] + dy * mtx[1][1];

			mtx[0][0] = mtx[0][0] * sx;  mtx[1][0] = mtx[1][0] * sy;
			mtx[0][1] = mtx[0][1] * sx;  mtx[1][1] = mtx[1][1] * sy;
		}
		return mtx;
	}

	const Matrix4f Widget::getWorldTransformationWithScaleAndTranslation(const Vector2f& scale, const Vector2f& trans, bool relative) const {
		return getWorldTransformationWithScaleAndTranslation(scale[0], scale[1], trans[0], trans[1], relative);
	}

	void Widget::updateWorldTransformation() const {
		if (m_isDirty) {
			if (m_parent) {
				m_modelMatrix = static_cast<Widget*>(m_parent)->getWorldTransformation() * getTransformationSOP();
			}else {
				m_modelMatrix = getTransformationSOP();
			}
			m_isDirty = false;
		}
	}

	const Vector2f& Widget::getWorldPosition(bool update) const {
		if (update)
			WorldPosition = getWorldTransformation().getTranslation2D();
		return WorldPosition;
	}

	const Vector2f& Widget::getWorldScale(bool update) const {
		if (update)
			WorldScale = getWorldTransformation().getScale2D();
		return WorldScale;
	}

	const float Widget::getWorldOrientation(bool update) const {
		if (update)
			WorldOrientation = getWorldTransformation().getRotation2D().getRoll2D();
		return WorldOrientation;
	}

	void Widget::setScale(const float sx, const float sy) {
		Sprite::setScale(sx, sy);
		OnTransformChanged();
	}

	void Widget::setScale(const Vector2f& scale) {
		Sprite::setScale(scale);
		OnTransformChanged();
	}

	void Widget::setScale(const float s) {
		Sprite::setScale(s);
		OnTransformChanged();
	}

	void Widget::setScaleAbsolute(const float sx, const float sy) {
		if (m_parent) {
			const Vector2f invScale = Vector2f::Inverse(static_cast<Widget*>(m_parent)->getWorldScale());
			Sprite::setScale(sx * invScale[0], sy * invScale[1]);
		}else {
			Sprite::setScale(sx, sy);
		}
		OnTransformChanged();
	}

	void Widget::setScaleAbsolute(const Vector2f& scale) {
		if (m_parent) {
			const Vector2f invScale = Vector2f::Inverse(static_cast<Widget*>(m_parent)->getWorldScale());
			Sprite::setScale(scale * invScale);
		}else {
			Sprite::setScale(scale);
		}
		OnTransformChanged();
	}

	void Widget::setScaleAbsolute(const float s) {
		if (m_parent) {
			const Vector2f invScale = Vector2f::Inverse(static_cast<Widget*>(m_parent)->getWorldScale());
			Sprite::setScale(s * invScale[0], s * invScale[1]);
		}else {
			Sprite::setScale(s);
		}
		OnTransformChanged();
	}

	void Widget::setPosition(const float x, const float y) {
		Sprite::setPosition(x, y);
		OnTransformChanged();
	}

	void Widget::setPosition(const Vector2f& position) {
		Sprite::setPosition(position);
		OnTransformChanged();
	}

	void Widget::setOrigin(const float x, const float y) {
		Sprite::setOrigin(x, y);
		OnTransformChanged();
	}

	void Widget::setOrigin(const Vector2f& origin) {
		Sprite::setOrigin(origin);
		OnTransformChanged();
	}

	void Widget::setOrientation(float degrees) {
		Sprite::setOrientation(degrees);
		OnTransformChanged();
	}

	void Widget::translate(const Vector2f& trans) {
		Sprite::translate(trans);
		OnTransformChanged();
	}

	void Widget::translate(const float dx, const float dy) {
		Sprite::translate(dx, dy);
		OnTransformChanged();
	}

	void Widget::translateRelative(const Vector2f& trans) {
		translateRelative(trans[0], trans[1]);
	}

	void Widget::translateRelative(const float dx, const float dy) {
		if (m_parent) {
			const Vector2f invScale = Vector2f::Inverse(static_cast<Widget*>(m_parent)->getWorldScale());
			Sprite::translateRelative(dx * invScale[0], dy * invScale[1]);
		}else {
			Sprite::translateRelative(dx, dy);
		}
		OnTransformChanged();
	}

	void Widget::scale(const Vector2f& scale) {
		Sprite::scale(scale);
		OnTransformChanged();
	}

	void Widget::scale(const float sx, const float sy) {
		Sprite::scale(sx, sy);
		OnTransformChanged();
	}

	void Widget::scale(const float s) {
		Sprite::scale(s);
		OnTransformChanged();
	}

	void Widget::scaleAbsolute(const Vector2f& scale) {
		scaleAbsolute(scale[0], scale[1]);
	}

	void Widget::scaleAbsolute(const float s) {
		scaleAbsolute(s, s);
	}

	void Widget::scaleAbsolute(const float sx, const float sy) {
		if (m_parent) {
			const Vector2f invScale = Vector2f::Inverse(static_cast<Widget*>(m_parent)->getWorldScale());
			Sprite::scale(sx * invScale[0], sy * invScale[1]);
		}else {
			Sprite::scale(sx, sy);
		}
		OnTransformChanged();
	}

	void Widget::rotate(const float degrees) {
		Sprite::rotate(degrees);
		OnTransformChanged();
	}

	void Widget::setDrawFunction(std::function<void()> fun) {
		m_draw = fun;
	}
}