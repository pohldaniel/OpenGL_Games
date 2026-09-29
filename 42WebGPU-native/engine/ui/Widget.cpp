#include "Widget.h"

Vector2f Widget::WorldPosition;
Vector2f Widget::WorldScale;
float Widget::WorldOrientation;
std::set<Widget*> Widget::DirtyWidgets;

Widget::Widget() : Node(), Object2D(), m_create(nullptr), m_isDirty(true), m_width(0.0f), m_height(0.0f), m_paddingX(0.0f), m_paddingY(0.0f), m_isLayoutDirty(true){
	MarkAsDirty(this);
}

Widget::Widget(const Widget& rhs) : Node(rhs), Object2D(rhs), m_create(rhs.m_create) {
	m_isDirty = rhs.m_isDirty;
	m_width = rhs.m_width;
	m_height = rhs.m_height;
	m_paddingX = rhs.m_paddingX;
	m_paddingY = rhs.m_paddingY;
	m_isLayoutDirty = rhs.m_isLayoutDirty;
}

Widget::Widget(Widget&& rhs) noexcept : Node(rhs), Object2D(rhs), m_create(std::move(rhs.m_create)) {
	m_isDirty = rhs.m_isDirty;
	m_width = rhs.m_width;
	m_height = rhs.m_height;
	m_paddingX = rhs.m_paddingX;
	m_paddingY = rhs.m_paddingY;
	m_isLayoutDirty = rhs.m_isLayoutDirty;
}

Widget::~Widget() {

}

void Widget::createTree() {
	//ProcessLayoutQueue();
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

void Widget::inputTree(const int mouseX, const int mouseY, bool buttonLeft) {
	if (m_input) {
		return m_input(mouseX, mouseY, buttonLeft);
	}
	inputDefault(mouseX, mouseY, buttonLeft);
	inputChildren(mouseX, mouseY, buttonLeft);
}

void Widget::inputChildren(int mouseX, int mouseY, bool buttonLeft) {
	if (m_children.size() > 0) {
		for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
			static_cast<Widget*>((*it).get())->inputTree(mouseX, mouseY, buttonLeft);
		}
	}
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
	
	MarkAsDirty(this);

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

void Widget::setInputFunction(std::function<void(const int mouseX, const int mouseY, bool buttonLeft)> fun) {
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
		Vector2f scale = m_parent ? static_cast<Widget*>(m_parent)->getWorldScale() : getScale();
		Vector2f position = getPosition();

		float width = 0.0f;
		float height = 0.0f;

		for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
			Widget* child = static_cast<Widget*>((*it).get());
			width += child->getWidth();
			height = std::max(height, child->getHeight());
		}

		setWidth(width + (m_paddingX * 2.0f), true);
		setHeight(height + (m_paddingY * 2.0f), true);
		setScale(m_width / scale[0], m_height / scale[1]);
		scale = getWorldScale();

		float posX = m_paddingX;
		for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
			Widget* child = static_cast<Widget*>((*it).get());
			float posY = (getHeight() - child->getHeight()) * 0.5f;
			child->setPosition(posX / scale[0], posY / scale[1]);
			posX += child->getWidth();
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

void Widget::MarkAsDirty(Widget* widget) {
	DirtyWidgets.insert(widget);
}

void Widget::ProcessLayoutQueue() {

	if (DirtyWidgets.empty()) 
		return;

	auto it = DirtyWidgets.begin();
	Widget* commonParent = *it;

	++it;

	for (; it != DirtyWidgets.end(); ++it) {
		Widget* nextWidget = *it;
		if (!nextWidget) continue;

		std::unordered_set<Widget*> ancestors;
		Widget* tracer = commonParent;
		while (tracer) {
			ancestors.insert(tracer);
			tracer = static_cast<Widget*>(tracer->m_parent);
		}

		tracer = nextWidget;
		while (tracer) {
			if (ancestors.count(tracer) > 0) {
				commonParent = tracer;
				break;
			}
			tracer = static_cast<Widget*>(tracer->m_parent);
		}
	}

	if (commonParent) {
		commonParent->updateLayout();
	}

	DirtyWidgets.clear();
	
}