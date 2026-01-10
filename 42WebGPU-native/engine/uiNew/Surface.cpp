#include "Surface.h"

namespace ui
{
	Surface::Surface() : Widget(), m_color(Vector4f::ONE), m_edge(Edge::ALL), m_borderRadius(12.0f), m_shader(nullptr) {
		Sprite::setShader(SurfaceShader.get());
	}

	Surface::Surface(const Surface& rhs) :
		Widget(rhs),
		m_color(rhs.m_color),
		m_edge(rhs.m_edge),
		m_borderRadius(rhs.m_borderRadius),
		m_shader(rhs.m_shader) {
	}

	Surface::Surface(Surface&& rhs) :
		Widget(rhs),
		m_color(std::move(rhs.m_color)),
		m_edge(std::move(rhs.m_edge)),
		m_borderRadius(std::move(rhs.m_borderRadius)),
		m_shader(std::move(rhs.m_shader)) {
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
		m_borderRadius = borderRadius;
	}

	void Surface::setShader(Shader* shader) {
		m_shader = shader;
		Sprite::setShader(m_shader);
	}

	void Surface::drawDefault() {
		
		const Vector2f& scale = getWorldScale();
		auto shader = m_shader ? m_shader : SurfaceShader.get();

		shader->use();
		shader->loadVector("u_dimensions", Vector2f(scale[0], scale[1]));
		shader->loadFloat("u_radius", m_borderRadius);
		shader->loadUnsignedInt("u_edge", m_edge);
		drawTransformed(m_color, getWorldTransformation());
	}
}