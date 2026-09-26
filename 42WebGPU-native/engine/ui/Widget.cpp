#include "Widget.h"

Vector2f Widget::WorldPosition;
Vector2f Widget::WorldScale;
float Widget::WorldOrientation;

Widget::Widget() : Node(), Object2D(), m_isDirty(true), m_draw(nullptr) {

}

Widget::Widget(const Widget& rhs) : Node(rhs), Object2D(rhs), m_draw(rhs.m_draw) {
	m_isDirty = rhs.m_isDirty;
}

Widget::Widget(Widget&& rhs) noexcept : Node(rhs), Object2D(rhs), m_draw(std::move(rhs.m_draw)) {
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
		for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
			static_cast<Widget*>((*it).get())->draw();
		}
	}
}

void Widget::input(const int mouseX, const int mouseY, bool buttonLeft) {
	if (m_input) {
		return m_input(mouseX, mouseY, buttonLeft);
	}
	inputDefault(mouseX, mouseY, buttonLeft);
	inputTree(mouseX, mouseY, buttonLeft);
}

void Widget::inputTree(int mouseX, int mouseY, bool buttonLeft) {
	if (m_children.size() > 0) {
		for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
			static_cast<Widget*>((*it).get())->input(mouseX, mouseY, buttonLeft);
		}
	}
}

void Widget::OnTransformChanged() {

	if (m_isDirty) {
		return;
	}

	for (auto&& child : m_children) {
		static_cast<Widget*>(child.get())->OnTransformChanged();
	}

	m_isDirty = true;
}

const Matrix4f& Widget::getWorldTransformation() const {
	if (m_isDirty) {
		m_modelMatrix = getTransformationSOP();
		if (m_parent)
			m_modelMatrix = static_cast<Widget*>(m_parent)->getWorldTransformation() * m_modelMatrix;

		m_isDirty = false;
	}

	return m_modelMatrix;
}

void Widget::updateWorldTransformation() const {
	if (m_isDirty) {
		m_modelMatrix = getTransformationSOP();
		if (m_parent)
			m_modelMatrix = static_cast<Widget*>(m_parent)->getWorldTransformation() * m_modelMatrix;

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

void Widget::setScale(float sx, float sy) {
	Object2D::setScale(sx, sy);
	OnTransformChanged();
}

void Widget::setScale(const Vector2f& scale) {
	Object2D::setScale(scale);
	OnTransformChanged();
}

void Widget::setScale(float s) {
	Object2D::setScale(s);
	OnTransformChanged();
}

void Widget::setPosition(float x, float y) {
	Object2D::setPosition(x, y);
	OnTransformChanged();
}

void Widget::setPosition(const Vector2f& position) {
	Object2D::setPosition(position);
	OnTransformChanged();
}

void Widget::setOrigin(float x, float y) {
	Object2D::setOrigin(x, y);
	OnTransformChanged();
}

void Widget::setOrigin(const Vector2f& origin) {
	Object2D::setOrigin(origin);
	OnTransformChanged();
}

void Widget::setOrientation(float degrees) {
	Object2D::setOrientation(degrees);
	OnTransformChanged();
}

void Widget::translate(const Vector2f& trans) {
	Object2D::translate(trans);
	OnTransformChanged();
}

void Widget::translate(const float dx, const float dy) {
	Object2D::translate(dx, dy);
	OnTransformChanged();
}

void Widget::translateRelative(const Vector2f& trans) {
	translateRelative(trans[0], trans[1]);
}

void Widget::translateRelative(const float dx, const float dy) {
	Object2D::translateRelative(dx, dy);
	OnTransformChanged();
}

void Widget::scale(const Vector2f& scale) {
	Object2D::scale(scale);
	OnTransformChanged();
}

void Widget::scale(float sx, float sy) {
	Object2D::scale(sx, sy);
	OnTransformChanged();
}

void Widget::scale(float s) {
	Object2D::scale(s);
	OnTransformChanged();
}

void Widget::rotate(float degrees) {
	Object2D::rotate(degrees);
	OnTransformChanged();
}

void Widget::setDrawFunction(std::function<void()> fun) {
	m_draw = fun;
}

void Widget::setInputFunction(std::function<void(const int mouseX, const int mouseY, bool buttonLeft)> fun) {
	m_input = fun;
}

void Widget::addWidget(UiPipelineType type, const UiInstance& instance) {
	if (uiContext.uiBatches.empty() || uiContext.uiBatches.back().pipelineType != type) {
		UiBatch uiBatch;
		uiBatch.pipelineType = type;
		uiBatch.startIndex = static_cast<uint32_t>(uiContext.uiInstances.size());
		uiBatch.instanceCount = 0;
		uiContext.uiBatches.push_back(uiBatch);
	}
	uiContext.uiInstances.push_back(instance);
	uiContext.uiBatches.back().instanceCount++;
}