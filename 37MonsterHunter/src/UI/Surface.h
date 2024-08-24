#pragma once
#include <functional>
#include <engine/Shader.h>
#include <UI/WidgetMH.h>

enum Edge {
	ALL,
	BOTTOM_LEFT,
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_RIGHT,
	EDGE_RIGHT,
	EDGE_LEFT,
	TOP,
	BOTTOM,
	EDGE_NONE
};

class Surface : public WidgetMH {

public:

	Surface();
	Surface(const Surface& rhs);
	Surface& operator=(const Surface& rhs);
	Surface(Surface&& rhs);
	Surface& operator=(Surface&& rhs);
	virtual ~Surface();

	void draw() override;
	void setDrawFunction(std::function<void()> fun);	
	void setColor(const Vector4f& color);
	void setShader(Shader* shader);
	void setEdge(Edge edge);
	void setBorderRadius(float borderRadius);

private:

	void drawDefault();
	std::function<void()> m_draw;
	Vector4f m_color;
	Shader* m_shader;
	Edge m_edge;
	float m_borderRadius;
};