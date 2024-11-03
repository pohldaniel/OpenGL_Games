#pragma once
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
	Surface(Surface&& rhs);
	virtual ~Surface();

	void setDrawFunction(std::function<void()> fun);	
	void setColor(const Vector4f& color);
	void setShader(Shader* shader);
	void setEdge(Edge edge);
	void setBorderRadius(float borderRadius);

private:

	void drawDefault() override;
	
	Vector4f m_color;
	Shader* m_shader;
	Edge m_edge;
	float m_borderRadius;
};