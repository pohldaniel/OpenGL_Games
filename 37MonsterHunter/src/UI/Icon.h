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

	void setColor(const Vector4f& color);
	void setShader(Shader* shader);
	void setSpriteSheet(const unsigned int& spriteSheet);
	void setAlign(bool align);

private:

	void drawDefault() override;

	const TextureRect& textureRect;
	Vector4f m_color;
	Shader* m_shader;
	unsigned int m_spriteSheet;	
	bool m_align;
};

class IconAnimated : public WidgetMH {

public:

	IconAnimated(const std::vector<TextureRect>& textureRects);
	IconAnimated(const IconAnimated& rhs);
	IconAnimated(IconAnimated&& rhs);
	virtual ~IconAnimated();

	void setColor(const Vector4f& color);
	void setShader(Shader* shader);
	void setSpriteSheet(const unsigned int& spriteSheet);
	void setCurrentFrame(const ptrdiff_t currentFrame);
	void setAlign(bool align);

private:

	void drawDefault() override;

	const std::vector<TextureRect>& textureRects;
	Vector4f m_color;
	Shader* m_shader;
	unsigned int m_spriteSheet;
	ptrdiff_t m_currentFrame;
	bool m_align;
};