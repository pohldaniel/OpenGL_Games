#include "Bar.h"

#include "Globals.h"
#include "Surface.h"

namespace ui
{
	Bar::Bar(const TileSet& tileSet) : Widget(), tileSet(tileSet), m_color(Vector4f::ONE), m_bgColor(0.0f, 0.0f, 0.0f, 1.0f), m_height(5.0f), m_radius(5.0f), m_width(200.0f), m_value(0.0f), m_maxValue(200.0f) {

	}

	Bar::Bar(const Bar& rhs) :
		Widget(rhs),
		tileSet(rhs.tileSet),
		m_bgColor(rhs.m_bgColor),
		m_color(rhs.m_color),
		m_height(rhs.m_height),
		m_width(rhs.m_width),
		m_value(rhs.m_value),
		m_maxValue(rhs.m_maxValue) {
	}

	Bar::Bar(Bar&& rhs) :
		Widget(rhs),
		tileSet(std::move(rhs.tileSet)),
		m_bgColor(std::move(rhs.m_bgColor)),
		m_color(std::move(rhs.m_color)),
		m_height(std::move(rhs.m_height)),
		m_width(std::move(rhs.m_width)),
		m_value(std::move(rhs.m_value)),
		m_maxValue(std::move(rhs.m_maxValue)) {
	}

	Bar::~Bar() {

	}

	void Bar::setValue(const float value) {
		m_value = value;
	}

	void Bar::setMaxValue(const float maxValue) {
		m_maxValue = maxValue;
	}

	void Bar::setWidth(const float width) {
		m_width = width;
	}

	void Bar::setHeight(const float height) {
		m_height = height;
	}

	void Bar::setRadius(const float radius) {
		m_radius = radius;
	}

	void Bar::setColor(const Vector4f& color) {
		m_color = color;
	}

	void Bar::setBgColor(const Vector4f& bgColor) {
		m_bgColor = bgColor;
	}

	void Bar::drawDefault() {
		float ratio = m_width / m_maxValue;
		float progress = std::max(0.0f, std::min(m_width, m_value * ratio));

		if (m_height == 5.0f) {
			const TextureRect& bgRect = tileSet.getTextureRects()[static_cast<int>(m_width)];
			const TextureRect& rect = tileSet.getTextureRects()[static_cast<int>(progress)];
			tileSet.bind();
			Sprite::drawTransformed(bgRect, m_bgColor, getWorldTransformationWithScale(bgRect.width, bgRect.height));
			Sprite::drawTransformed(rect, m_color, getWorldTransformationWithScale(rect.width, rect.height));

		}else {
			Sprite::setShader(SurfaceShader.get());
			auto shader = SurfaceShader.get();
			shader->use();
			shader->loadFloat("u_radius", m_radius);
			shader->loadUnsignedInt("u_edge", m_radius == 0.0f ? Edge::EDGE_NONE : Edge::ALL);

			shader->loadVector("u_dimensions", Vector2f(m_width, m_height));
			Sprite::drawTransformed(m_bgColor, getWorldTransformationWithScale(m_width, m_height));

			shader->loadVector("u_dimensions", Vector2f(progress, m_height));
			Sprite::drawTransformed(m_color, getWorldTransformationWithScale(progress, m_height));
		}
	}
}