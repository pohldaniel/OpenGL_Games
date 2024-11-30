#include "Surface.h"

Surface::Surface() : WidgetMH(),  m_color(Vector4f::ONE), m_edge(Edge::ALL), m_borderRadius(10.0f){

}

Surface::Surface(const Surface& rhs) :
	WidgetMH(rhs),
	m_color(rhs.m_color),
	m_edge(rhs.m_edge),
	m_borderRadius(rhs.m_borderRadius){
}

Surface::Surface(Surface&& rhs) :
	WidgetMH(rhs),
	m_color(rhs.m_color),
	m_edge(rhs.m_edge),
	m_borderRadius(rhs.m_borderRadius) {
}

Surface::~Surface() {

}

void Surface::setColor(const Vector4f& color) {
	m_color = color;
}

void Surface::setEdge(Edge edge) {
	m_edge = edge;
}

void Surface::setBorderRadius(float borderRadius) {
	m_borderRadius = 12.0f;
}

void Surface::setShader(Shader* shader) {
	m_shader = shader;
	Sprite::setShader(m_shader);
}

void Surface::drawDefault() {
 	const Vector2f& scale = getWorldScale();
	m_shader->use();
	m_shader->loadVector("u_dimensions", Vector2f(scale[0], scale[1]));
	m_shader->loadFloat("u_radius", m_borderRadius);
	m_shader->loadUnsignedInt("u_edge", m_edge);
	draw2(m_color, getWorldTransformation());
}