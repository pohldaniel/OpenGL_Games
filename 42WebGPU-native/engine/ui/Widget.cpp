#include "Widget.h"

Vector2f Widget::WorldPosition;
Vector2f Widget::WorldScale;
float Widget::WorldOrientation;
Widget* Widget::ActiveWidget = nullptr;

Widget::Widget() : Node(), Object2D(),
m_isDirty(true), 
m_hasFocus(false),
m_width(0.0f), 
m_height(0.0f), 
m_paddingX(0.0f), 
m_paddingY(0.0f), 
m_spacingX(0.0f), 
m_spacingY(0.0f), 
m_isLayoutDirty(true),
m_layout(Layout::HORIZONTAL),
m_isMovable(false),
m_border(0.0f){
	
}

Widget::Widget(const Widget& rhs) : Node(rhs), Object2D(rhs) {
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
	m_isMovable = rhs.m_isMovable;
	m_border = rhs.m_border;
}

Widget::Widget(Widget&& rhs) noexcept : Node(rhs), Object2D(rhs) {
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
	m_isMovable = rhs.m_isMovable;
	m_border = rhs.m_border;
}

Widget::~Widget() {

}

void Widget::draw() {	
	drawTree();
}

void Widget::drawTree() {
	bool openedNewLayer = false;
	if (m_isMovable) {
		UiLayer newLayer;
		uiContext.uiLayers.push_back(newLayer);
		uiContext.currentActiveLayer = &uiContext.uiLayers.back();
		openedNewLayer = true;
	}

	OnDraw();

	uint32_t prevSiccorX = uiContext.activeScissorX;
	uint32_t prevSiccorY = uiContext.activeScissorY;
	uint32_t prevSiccorW = uiContext.activeScissorWidth;
	uint32_t prevSiccorH = uiContext.activeScissorHeight;

	if (m_border > 0.0f) {	
		Vector2f pos = getWorldPosition(true);
		Vector2f scale = getWorldScale(true);
		float scaledBorder = m_border * m_scale[0];

		float currentX = std::max(pos[0], static_cast<float>(prevSiccorX));
		float currentY = std::max(pos[1], static_cast<float>(prevSiccorY));

		uiContext.activeScissorX = std::min(static_cast<uint32_t>(std::max(0.0f, currentX)), static_cast<uint32_t>(uiContext.width));
		uiContext.activeScissorY = std::min(static_cast<uint32_t>(std::max(0.0f, currentY)), static_cast<uint32_t>(uiContext.height));

		float parentEndX = static_cast<float>(prevSiccorX + prevSiccorW);
		float parentEndY = static_cast<float>(prevSiccorY + prevSiccorH);

		float maxAllowedX = parentEndX - scaledBorder;
		float maxAllowedY = parentEndY - scaledBorder;
		float finalEndX = std::min(pos[0] + (m_width - m_border) * scale[0], maxAllowedX);
		float finalEndY = std::min(pos[1] + (m_height - m_border) * scale[1], maxAllowedY);

		uint32_t clampedEndX = std::min(static_cast<uint32_t>(std::max(0.0f, finalEndX)), static_cast<uint32_t>(uiContext.width));
		uint32_t clampedEndY = std::min(static_cast<uint32_t>(std::max(0.0f, finalEndY)), static_cast<uint32_t>(uiContext.height));

		uiContext.activeScissorWidth = (clampedEndX > uiContext.activeScissorX) ? (clampedEndX - uiContext.activeScissorX) : 0u;
		uiContext.activeScissorHeight = (clampedEndY > uiContext.activeScissorY) ? (clampedEndY - uiContext.activeScissorY) : 0u;
	}

	if (m_children.size() > 0) {
		for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
			static_cast<Widget*>((*it).get())->drawTree();
		}
	}

	uiContext.activeScissorX = prevSiccorX;
	uiContext.activeScissorY = prevSiccorY;
	uiContext.activeScissorWidth = prevSiccorW;
	uiContext.activeScissorHeight = prevSiccorH;

	if (openedNewLayer) {
		if (uiContext.uiLayers.size() > 1) {
			uiContext.currentActiveLayer = &uiContext.uiLayers[uiContext.uiLayers.size() - 2];
		}else {
			uiContext.currentActiveLayer = nullptr;
		}
	}
}

void Widget::resetTree() {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		static_cast<Widget*>(it->get())->resetTree();
	}
	OnReset();
}

void Widget::input(const int mouseX, const int mouseY, bool buttonLeft) {
	resetTree();
	if (Widget::ActiveWidget != nullptr) {
		Widget::ActiveWidget->OnInput(mouseX, mouseY, buttonLeft);
		if (!buttonLeft) {
			Widget::ActiveWidget = nullptr;
			
		}
		return;
	}
	inputTree(mouseX, mouseY, buttonLeft);
	updateLayout();
}

bool Widget::inputTree(const int mouseX, const int mouseY, bool buttonLeft) {
	if(!OnMouseOver(mouseX, mouseY)) {
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

	return OnInput(mouseX, mouseY, buttonLeft);
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

void Widget::pushWidget(UiPipelineType type, const UiInstance& instance) {
	if (!uiContext.currentActiveLayer) {
		if (uiContext.uiLayers.empty()) {
			UiLayer defaultLayer;
			uiContext.uiLayers.push_back(defaultLayer);
		}
		uiContext.currentActiveLayer = &uiContext.uiLayers.back();
	}

	auto& currentBatches = uiContext.currentActiveLayer->batches;

	uint32_t targetX = uiContext.activeScissorX;
    uint32_t targetY = uiContext.activeScissorY;
    uint32_t targetW = uiContext.activeScissorWidth;
    uint32_t targetH = uiContext.activeScissorHeight;

    bool scissorChanged = false;
    if (!currentBatches.empty()) {
        const auto& lastBatch = currentBatches.back();
        if (lastBatch.scissorX != targetX || lastBatch.scissorY != targetY || lastBatch.scissorWidth != targetW || lastBatch.scissorHeight != targetH) {
            scissorChanged = true;
        }
    }

    if (currentBatches.empty() || currentBatches.back().pipelineType != type || scissorChanged) {
        UiBatch uiBatch;
        uiBatch.pipelineType = type;
        uiBatch.startIndex = static_cast<uint32_t>(uiContext.uiInstances.size());
        uiBatch.instanceCount = 0;
        
		uiBatch.scissorX = targetX;
		uiBatch.scissorY = targetY;
		uiBatch.scissorWidth = targetW;
		uiBatch.scissorHeight = targetH;

        currentBatches.push_back(uiBatch);
    }

    uiContext.uiInstances.push_back(instance);
    currentBatches.back().instanceCount++;
}

void Widget::updateLayout() {
	for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
		static_cast<Widget*>((*it).get())->updateLayout();
	}
	OnLayoutChanged();
}

void Widget::OnLayoutChanged() {
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
		}else if (m_layout == Layout::GRID) {
			const int widgetCount = static_cast<int>(getChildren().size());
			const int maxRows = static_cast<int>(std::ceil(std::sqrt(widgetCount)));
			const int maxCols = static_cast<int>(std::ceil(static_cast<float>(widgetCount) / maxRows));
			struct SortedWidget {
				Widget* widget;
				float area;
			};
			std::vector<SortedWidget> sortedChildren;
			sortedChildren.reserve(getChildren().size());

			for (const auto& childNode : getChildren()) {
				Widget* child = static_cast<Widget*>(childNode.get());
				float area = child->getWidth() * child->getHeight();
				sortedChildren.push_back({ child, area });
			}

			std::sort(sortedChildren.begin(), sortedChildren.end(), [](const SortedWidget& a, const SortedWidget& b) {
				return a.area > b.area;
			});

			struct PlacedWidget {
				Widget* widget;
				int row;
				int col;
			};
			std::vector<PlacedWidget> placedWidgets;
			placedWidgets.reserve(sortedChildren.size());

			std::vector<float> colWidths(maxCols, 0.0f);
			std::vector<float> rowHeights(maxRows, 0.0f);

			int currentIdx = 0;
			for (const auto& item : sortedChildren) {
				if (currentIdx >= maxCols * maxRows) break;

				int r = currentIdx / maxCols;
				int c = currentIdx % maxCols;

				placedWidgets.push_back({ item.widget, r, c });
				colWidths[c] = std::max(colWidths[c], item.widget->getWidth());
				rowHeights[r] = std::max(rowHeights[r], item.widget->getHeight());

				currentIdx++;
			}

			float totalWidth = 0.0f;
			for (float w : colWidths) totalWidth += w;
			totalWidth += std::max(0, maxCols - 1) * m_spacingX;

			float totalHeight = 0.0f;
			for (float h : rowHeights) totalHeight += h;
			totalHeight += std::max(0, maxRows - 1) * m_spacingY;

			setWidth(totalWidth + (m_paddingX * 2.0f), true);
			setHeight(totalHeight + (m_paddingY * 2.0f), true);

			std::vector<float> colOffsets(maxCols, 0.0f);
			float currentX = m_paddingX;
			for (int c = 0; c < maxCols; ++c) {
				colOffsets[c] = currentX;
				currentX += colWidths[c] + m_spacingX;
			}

			std::vector<float> rowOffsets(maxRows, 0.0f);
			float currentY = m_paddingY;
			for (int r = 0; r < maxRows; ++r) {
				rowOffsets[r] = currentY;
				currentY += rowHeights[r] + m_spacingY;
			}

			for (const auto& placed : placedWidgets) {
				Widget* child = placed.widget;
				int r = placed.row;
				int c = placed.col;
				float posX = colOffsets[c];
				float targetHeight = rowHeights[r];
				float posY = rowOffsets[r] + (targetHeight - child->getHeight()) * 0.5f;

				child->setPosition(Vector2f(posX, posY));
			}
		}else if (m_layout == Layout::MASONRY) {
			const int widgetCount = static_cast<int>(getChildren().size());
			if (widgetCount == 0) return;

			std::vector<Widget*> sortedWidgets;
			sortedWidgets.reserve(widgetCount);
			for (const auto& childNode : getChildren()) {
				sortedWidgets.push_back(static_cast<Widget*>(childNode.get()));
			}

			std::sort(sortedWidgets.begin(), sortedWidgets.end(), [](Widget* a, Widget* b) {
				return a->getWidth() > b->getWidth();
			});

			float baseWidth = sortedWidgets.front()->getWidth();
			float minWidth = sortedWidgets.back()->getWidth();
			float targetQuaderWidth = baseWidth + minWidth + m_spacingX;

			struct PlacedRow {
				Widget* leftWidget = nullptr;
				Widget* rightWidget = nullptr;
				float height = 0.0f;
			};
			std::vector<PlacedRow> rows;

			int leftIdx = 0;
			int rightIdx = widgetCount - 1;

			while (leftIdx <= rightIdx) {
				PlacedRow row;
				row.leftWidget = sortedWidgets[leftIdx];
				float rowHeight = row.leftWidget->getHeight();
				leftIdx++;

				if (leftIdx <= rightIdx) {
					row.rightWidget = sortedWidgets[rightIdx];
					rowHeight = std::max(rowHeight, row.rightWidget->getHeight());
					rightIdx--;
				}

				row.height = rowHeight;
				rows.push_back(row);
			}

			float maxQuaderWidth = 0.0f;
			for (const auto& row : rows) {
				float leftWidth = row.leftWidget ? row.leftWidget->getWidth() : 0.0f;
				float rightWidth = row.rightWidget ? row.rightWidget->getWidth() : 0.0f;

				float rowWidth = leftWidth + rightWidth + m_spacingX;
				maxQuaderWidth = std::max(maxQuaderWidth, rowWidth);
			}

			float totalWidth = maxQuaderWidth + (m_paddingX * 2.0f);

			float totalHeight = 0.0f;
			for (const auto& row : rows) {
				totalHeight += row.height + m_spacingY;
			}
			if (!rows.empty()) {
				totalHeight -= m_spacingY;
			}
			totalHeight += (m_paddingY * 2.0f);

			setWidth(totalWidth, true);
			setHeight(totalHeight, true);

			float currentY = m_paddingY;

			for (const auto& row : rows) {
				if (row.leftWidget) {
					float posX = m_paddingX;
					float posY = currentY ;
					row.leftWidget->setPosition(Vector2f(posX, posY));
				}

				if (row.rightWidget) {
					float posX = m_paddingX + maxQuaderWidth - row.rightWidget->getWidth();
					float posY = currentY ;
					row.rightWidget->setPosition(Vector2f(posX, posY));
				}

				currentY += row.height + m_spacingY;
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

void Widget::setBorder(float border) {
	m_border = border;
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

bool Widget::OnMouseOver(int mouseX, int mouseY) {
	Vector2f scale = getWorldScale(true);
	float visualWidth = m_width * scale[0];
	float visualHeight = m_height * scale[1];
	Vector2f position = getWorldPosition(true);

	bool check = (mouseX > position[0] && mouseX < position[0] + visualWidth &&
		mouseY > position[1] && mouseY < position[1] + visualHeight);

	return check;
}

void Widget::OnReset() {
	m_hasFocus = false;
}

bool Widget::OnInput(int mouseX, int mouseY, bool buttonLeft) {
	return false;
}