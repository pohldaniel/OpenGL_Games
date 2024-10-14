#pragma once
#include <functional>
#include <engine/Shader.h>
#include <engine/Rect.h>
#include <UI/WidgetMH.h>

class Icon: public WidgetMH {

public:

	Icon(const TextureRect& textureRect);
	Icon(const Icon& rhs);
	Icon& operator=(const Icon& rhs);
	Icon(Icon&& rhs);
	Icon& operator=(Icon&& rhs);
	virtual ~Icon();

	void draw() override;
	void setDrawFunction(std::function<void()> fun);
	void setColor(const Vector4f& color);
	void setShader(Shader* shader);
	void setSpriteSheet(const unsigned int& spriteSheet);
	void setTextureRect(const TextureRect& textureRect);

private:

	void drawDefault();
	std::function<void()> m_draw;
	Vector4f m_color;
	Shader* m_shader;
	unsigned int m_spriteSheet;
	TextureRect textureRect;
};

class IconAnimated : public WidgetMH {

public:

	IconAnimated(const std::vector<TextureRect>& textureRects);
	IconAnimated(const IconAnimated& rhs);
	IconAnimated& operator=(const IconAnimated& rhs);
	IconAnimated(IconAnimated&& rhs);
	IconAnimated& operator=(IconAnimated&& rhs);
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