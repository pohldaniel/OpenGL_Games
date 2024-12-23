#include <engine/Fontrenderer.h>
#include <engine/Spritesheet.h>
#include "Icon.h"

namespace ui
{
	Icon::Icon(const TextureRect& textureRect) : Widget(), textureRect(textureRect), m_color(Vector4f::ONE), m_spriteSheet(0), m_aligned(false), m_flipped(false) {

	}

	Icon::Icon(const Icon& rhs) :
		Widget(rhs),
		m_color(rhs.m_color),
		textureRect(rhs.textureRect),
		m_spriteSheet(rhs.m_spriteSheet),
		m_aligned(rhs.m_aligned) {
	}

	Icon::Icon(Icon&& rhs) :
		Widget(rhs),
		m_color(std::move(rhs.m_color)),
		textureRect(std::move(rhs.textureRect)),
		m_spriteSheet(std::move(rhs.m_spriteSheet)),
		m_aligned(std::move(rhs.m_aligned)) {
	}

	Icon::~Icon() {

	}

	void Icon::setColor(const Vector4f& color) {
		m_color = color;
	}

	void Icon::setShader(Shader* shader) {
		m_shader = shader;
		Sprite::setShader(m_shader);
	}

	void Icon::setSpriteSheet(const unsigned int& spriteSheet) {
		m_spriteSheet = spriteSheet;
	}

	void Icon::setAligned(bool aligned) {
		m_aligned = aligned;
	}

	void Icon::setFlipped(bool flipped) {
		m_flipped = flipped;
	}

	void Icon::drawDefault() {

		Spritesheet::Bind(m_spriteSheet);
		if (m_aligned)
			Sprite::drawTransformed(textureRect, m_color, getWorldTransformationWithScaleAndTranslation(textureRect.width, textureRect.height, -0.5f * textureRect.width, -0.5f * textureRect.height), m_flipped);
		else
			Sprite::drawTransformed(textureRect, m_color, getWorldTransformation(), m_flipped);
	}
	////////////////////////////////////////////////////////////////
	IconAnimated::IconAnimated(const std::vector<TextureRect>& textureRects) : Widget(), textureRects(textureRects), m_color(Vector4f::ONE), m_spriteSheet(0), m_currentFrame(-1), m_aligned(false), m_flipped(false), m_offsetX(0.0f), m_offsetY(0.0f) {

	}

	IconAnimated::IconAnimated(const IconAnimated& rhs) :
		Widget(rhs),
		m_color(rhs.m_color),
		textureRects(rhs.textureRects),
		m_spriteSheet(rhs.m_spriteSheet),
		m_currentFrame(rhs.m_currentFrame),
		m_aligned(rhs.m_aligned),
		m_flipped(rhs.m_flipped),
		m_offsetX(rhs.m_offsetX),
		m_offsetY(rhs.m_offsetY) {
	}

	IconAnimated::IconAnimated(IconAnimated&& rhs) :
		Widget(rhs),
		m_color(std::move(rhs.m_color)),
		textureRects(std::move(rhs.textureRects)),
		m_spriteSheet(std::move(rhs.m_spriteSheet)),
		m_currentFrame(std::move(rhs.m_currentFrame)),
		m_aligned(std::move(rhs.m_aligned)),
		m_flipped(std::move(rhs.m_flipped)),
		m_offsetX(std::move(rhs.m_offsetX)),
		m_offsetY(std::move(rhs.m_offsetY)) {
	}

	IconAnimated::~IconAnimated() {

	}

	void IconAnimated::setColor(const Vector4f& color) {
		m_color = color;
	}

	void IconAnimated::setShader(Shader* shader) {
		m_shader = shader;
		Sprite::setShader(m_shader);
	}

	void IconAnimated::setSpriteSheet(const unsigned int& spriteSheet) {
		m_spriteSheet = spriteSheet;
	}

	void IconAnimated::setAligned(bool aligned) {
		m_aligned = aligned;
	}

	void IconAnimated::setFlipped(bool flipped) {
		m_flipped = flipped;
	}

	void IconAnimated::setCurrentFrame(const int currentFrame) {
		m_currentFrame = currentFrame;
	}

	void IconAnimated::setOffsetX(const float offsetX) {
		m_offsetX = offsetX;
	}

	void IconAnimated::setOffsetY(const float offsetY) {
		m_offsetY = offsetY;
	}

	void IconAnimated::drawDefault() {
		if (m_currentFrame < 0)
			return;

		Spritesheet::Bind(m_spriteSheet);
		const TextureRect& rect = textureRects[m_currentFrame];

		if (m_aligned)
			Sprite::drawTransformed(rect, m_color, getWorldTransformationWithScaleAndTranslation(rect.width, rect.height, -0.5f * rect.width + m_offsetX, -0.5f * rect.height + m_offsetY), m_flipped);
		else if (m_offsetX || m_offsetY)
			Sprite::drawTransformed(rect, m_color, getWorldTransformationWithTranslation(m_offsetX, m_offsetY), m_flipped);
		else
			Sprite::drawTransformed(rect, m_color, getWorldTransformation(), m_flipped);

	}
}