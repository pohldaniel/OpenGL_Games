#pragma once
#include <list>
#include <vector>
#include <memory>
#include <functional>
#include <set>
#include <unordered_set>

#include <webgpu.h>
#include "../ui/UiContext.h"
#include "../scene/Node.h"
#include "../Vector.h"
#include "../Object.h"

class Widget : public Node, public Object2D {

public:

	Widget();
	Widget(const Widget& rhs);
	Widget(Widget&& rhs) noexcept;
	virtual ~Widget();

	void inputTree(int mouseX, int mouseY, bool buttonLeft = false);
	void updateLayout();
	void createTree();

	void setScale(float sx, float sy) override;
	void setScale(const Vector2f& scale) override;
	void setScale(float s) override;

	void setScaleAbsolute(float sx, float sy);
	void setScaleAbsolute(const Vector2f& scale);
	void setScaleAbsolute(float s);

	void setPosition(float x, float y) override;
	void setPosition(const Vector2f& position) override;

	void setOrigin(float x, float y) override;
	void setOrigin(const Vector2f& origin);

	void setOrientation(float degrees) override;

	void translate(const Vector2f& trans) override;
	void translate(float dx, float dy) override;

	void translateRelative(const Vector2f& trans) override;
	void translateRelative(float dx, float dy) override;

	void scale(const Vector2f& scale) override;
	void scale(float sx, float sy) override;
	void scale(float s) override;

	void rotate(float degrees) override;

	const Matrix4f& getWorldTransformation() const;
	const Vector2f& getWorldPosition(bool update = true) const;
	const Vector2f& getWorldScale(bool update = true) const;
	const float getWorldOrientation(bool update = true) const;
	void updateWorldTransformation() const;
	
	void setCreateFunction(std::function<void()> fun);
	void setInputFunction(std::function<void(int mouseX, int mouseY, bool buttonLeft)> fun);
	
	float getWidth();
	float getHeight();
	float getPadding();

	void setWidth(float width, bool silent = false);
	void setHeight(float height, bool silent = false);
	virtual void setPadding(float padding, bool silent = false);
	

protected:

	void OnTransformChanged();
	void OnInvalidate();

	void createChildren();
	void inputChildren(int mouseX, int mouseY, bool buttonLeft = false);
			
	void pushWidget(UiPipelineType type, const UiInstance& instance);
	std::function<void()> m_create;
	std::function<void(const int mouseX, const int mouseY, bool buttonLeft)> m_input;
	mutable bool m_isLayoutDirty;
	float m_width;
	float m_height;
	float m_padding;

private:

	virtual void inputDefault(int mouseX, int mouseY, bool buttonLeft = false) = 0;
	virtual void layoutDefault();
	virtual void createDefault() = 0;
	
	mutable Matrix4f m_modelMatrix;
	mutable bool m_isDirty;
	
	static void MarkAsDirty(Widget* widget);
	static void ProcessLayoutQueue();

	static Vector2f WorldPosition;
	static Vector2f WorldScale;
	static float WorldOrientation;
	static std::set<Widget*> DirtyWidgets;
};

