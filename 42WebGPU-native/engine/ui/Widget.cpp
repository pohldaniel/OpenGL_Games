#include "Widget.h"

Vector2f Widget::WorldPosition;
Vector2f Widget::WorldScale;
float Widget::WorldOrientation;
Widget* Widget::ActiveWidget = nullptr;

Widget::Widget() : Node(), Object2D(), 
m_create(nullptr), 
m_isDirty(true), 
m_hasFocus(false),
m_width(0.0f), 
m_height(0.0f), 
m_paddingX(0.0f), 
m_paddingY(0.0f), 
m_spacingX(0.0f), 
m_spacingY(0.0f), 
m_isLayoutDirty(true),
m_layout(Layout::HORIZONTAL){
	
}

Widget::Widget(const Widget& rhs) : Node(rhs), Object2D(rhs), m_create(rhs.m_create) {
	m_isDirty = rhs.m_isDirty;
	m_hasFocus = rhs.m_hasFocus;
	m_width = rhs.m_width;
	m_height = rhs.m_height;
	m_paddingX = rhs.m_paddingX;
	m_paddingY = rhs.m_paddingY;
	m_spacingX = rhs.m_spacingX;
	m_spacingY = rhs.m_spacingY;
	m_isLayoutDirty = rhs.m_isLayoutDirty;
	m_layout = rhs.m_layout;
}

Widget::Widget(Widget&& rhs) noexcept : Node(rhs), Object2D(rhs), m_create(std::move(rhs.m_create)) {
	m_isDirty = rhs.m_isDirty;
	m_hasFocus = rhs.m_hasFocus;
	m_width = rhs.m_width;
	m_height = rhs.m_height;
	m_paddingX = rhs.m_paddingX;
	m_paddingY = rhs.m_paddingY;
	m_spacingX = rhs.m_spacingX;
	m_spacingY = rhs.m_spacingY;
	m_isLayoutDirty = rhs.m_isLayoutDirty;
	m_layout = rhs.m_layout;
}

Widget::~Widget() {

}

void Widget::createTree() {
	updateLayout();
	if (m_create) {
		return m_create();
	}
	createDefault();
	createChildren();
}

void Widget::createChildren() {
	if (m_children.size() > 0) {
		for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
			static_cast<Widget*>((*it).get())->createTree();
		}
	}
}

void Widget::input(const int mouseX, const int mouseY, bool buttonLeft) {
	resetTree();
	if (Widget::ActiveWidget != nullptr) {
		Widget::ActiveWidget->inputDefault(mouseX, mouseY, buttonLeft);
		if (!buttonLeft) {
			Widget::ActiveWidget = nullptr;
			
		}
		return;
	}
	inputTree(mouseX, mouseY, buttonLeft);
	
}

void Widget::resetTree() {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		static_cast<Widget*>(it->get())->resetTree();
	}
	resetDefault();
}

bool Widget::inputTree(const int mouseX, const int mouseY, bool buttonLeft) {
	if(!isMouseOverDefault(mouseX, mouseY)) {
		return false;
	}

	if (!m_children.empty()) {
		for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
			Widget* child = static_cast<Widget*>(it->get());
			if (child->inputTree(mouseX, mouseY, buttonLeft)) {
				return true;
			}
		}
	}

	if(buttonLeft) {
		Widget::ActiveWidget = this;
		m_hasFocus = true;
		pushToFront();
	}

	return inputDefault(mouseX, mouseY, buttonLeft);
}

void Widget::OnTransformChanged() {
	if (m_isDirty) 
		return;

	m_isDirty = true;

	for (auto&& child : m_children) {
		static_cast<Widget*>(child.get())->OnTransformChanged();
	}	
}

void Widget::OnInvalidate() {
	
	if (m_isLayoutDirty) {
		return;
	}
	m_isLayoutDirty = true;
	
	if(m_parent)
		static_cast<Widget*>(m_parent)->OnInvalidate();
	
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

void Widget::setCreateFunction(std::function<void()> fun) {
	m_create = fun;
}

void Widget::setInputFunction(std::function<bool(const int mouseX, const int mouseY, bool buttonLeft)> fun) {
	m_input = fun;
}

void Widget::pushWidget(UiPipelineType type, const UiInstance& instance) {
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

void Widget::updateLayout() {
	for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
		static_cast<Widget*>((*it).get())->updateLayout();
	}
	layoutDefault();
}

void Widget::layoutDefault() {	
	if (!m_isLayoutDirty)
		return;

	if (!m_children.empty()) {
		if (m_layout == Layout::HORIZONTAL) {
			float totalWidth = 0.0f;
			float maxHeight = 0.0f;

			for (const auto& childNode : getChildren()) {
				Widget* child = static_cast<Widget*>(childNode.get());
				totalWidth += child->getWidth() + m_spacingX;
				maxHeight = std::max(maxHeight, child->getHeight());
			}

			totalWidth -= m_spacingX;

			setWidth(totalWidth + (m_paddingX * 2.0f), true);
			setHeight(maxHeight + (m_paddingY * 2.0f), true);
			float posX = m_paddingX;

			for (const auto& childNode : getChildren()) {
				Widget* child = static_cast<Widget*>(childNode.get());
				float posY = (getHeight() - child->getHeight()) * 0.5f;
				child->setPosition(Vector2f(posX, posY));
				posX += child->getWidth() + m_spacingX;
			}
		}else if (m_layout == Layout::VERTICAL) {
			
			float totalHeight = 0.0f;
			float maxWidth = 0.0f;

			for (const auto& childNode : getChildren()) {
				Widget* child = static_cast<Widget*>(childNode.get());
				totalHeight += child->getHeight() + m_spacingY;
				maxWidth = std::max(maxWidth, child->getWidth());
			}

			totalHeight -= m_spacingY;
			setWidth(maxWidth + (m_paddingX * 2.0f), true);
			setHeight(totalHeight + (m_paddingY * 2.0f), true);
			float posY = m_paddingY;
			float posX = m_paddingX;

			for (const auto& childNode : getChildren()) {
				Widget* child = static_cast<Widget*>(childNode.get());
				child->setPosition(Vector2f(posX, posY));
				posY += child->getHeight() + m_spacingY;
			}
		}
	}
	m_isLayoutDirty = false;
}

float Widget::getWidth(){
	return m_width;
}

float Widget::getHeight() {
	return m_height;
}

void Widget::setWidth(float width, bool silent) {
	m_width = width;
	if(!silent)
		OnInvalidate();
}

void Widget::setHeight(float height, bool silent) {
	m_height = height;
	if (!silent)
		OnInvalidate();
}

void Widget::setPadding(float paddingX, float paddingY, bool silent) {
	m_paddingX = paddingX;
	m_paddingY = paddingY;
	if (!silent)
		OnInvalidate();
}

void Widget::setSpacing(float spacingX, float spacingY, bool silent) {
	m_spacingX = spacingX;
	m_spacingY = spacingY;
	if (!silent)
		OnInvalidate();
}

void Widget::setLayout(Layout layout, bool silent) {
	m_layout = layout;
	if (!silent)
		OnInvalidate();
}

void Widget::pushToFront() {	
	if (m_parent) {
		auto& siblings = m_parent->getChildren();
		for (auto it = siblings.begin(); it != siblings.end(); ++it) {
			if ((*it).get() == this) {
				siblings.splice(siblings.end(), siblings, it);
				break;
			}
		}
		static_cast<Widget*>(m_parent)->pushToFront();
	}
}

bool Widget::isMouseOverDefault(int mouseX, int mouseY) {
	Vector2f scale = getWorldScale(true);
	float visualWidth = m_width * scale[0];
	float visualHeight = m_height * scale[1];
	Vector2f position = getWorldPosition(true);

	bool check = (mouseX > position[0] && mouseX < position[0] + visualWidth &&
		mouseY > position[1] && mouseY < position[1] + visualHeight);

	return check;
}

void Widget::resetDefault() {
	m_hasFocus = false;
}