#pragma once
#include <functional>
#include <engine/Shader.h>
#include <engine/Rect.h>
#include <UI/WidgetMH.h>

class Icon: public WidgetMH {

public:

	Icon(const TextureRect& textureRect);
	Icon(const Icon& rhs);
	Icon(Icon&& rhs);
	virtual ~Icon();

	void draw() override;
	void setDrawFunction(std::function<void()> fun);
	void setColor(const Vector4f& color);
	void setShader(Shader* shader);
	void setSpriteSheet(const unsigned int& spriteSheet);

private:

	void drawDefault();
	std::function<void()> m_draw;
	Vector4f m_color;
	Shader* m_shader;
	unsigned int m_spriteSheet;
	const TextureRect& textureRect;
};

class IconAnimated : public WidgetMH {

public:

	IconAnimated(const std::vector<TextureRect>& textureRects);
	IconAnimated(const IconAnimated& rhs);
	IconAnimated(IconAnimated&& rhs);
	virtual ~IconAnimated();

	void draw() override;
	void setDrawFunction(std::function<void()> fun);
	void setColor(const Vector4f& color);
	void setShader(Shader* shader);
	void setSpriteSheet(const unsigned int& spriteSheet);
	void setCurrentFrame(const size_t currentFrame);

private:

	void drawDefault();
	std::function<void()> m_draw;
	Vector4f m_color;
	Shader* m_shader;
	unsigned int m_spriteSheet;
	size_t m_currentFrame;
	const std::vector<TextureRect>& textureRects;
};