#pragma once
#include <list>
#include <vector>
#include <memory>
#include <functional>

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

	virtual void draw();
	virtual void input(int mouseX, int mouseY, bool buttonLeft = false);

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

	void setDrawFunction(std::function<void()> fun);
	void setInputFunction(std::function<void(int mouseX, int mouseY, bool buttonLeft)> fun);

protected:

	void OnTransformChanged();
	void drawTree();
	void inputTree(int mouseX, int mouseY, bool buttonLeft = false);

	void addWidget(UiPipelineType type, const UiInstance& instance);
	std::function<void()> m_draw;
	std::function<void(const int mouseX, const int mouseY, bool buttonLeft)> m_input;

private:

	virtual void drawDefault() = 0;
	virtual void inputDefault(int mouseX, int mouseY, bool buttonLeft = false) = 0;

	mutable Matrix4f m_modelMatrix;
	mutable bool m_isDirty;

	static Vector2f WorldPosition;
	static Vector2f WorldScale;
	static float WorldOrientation;
};

