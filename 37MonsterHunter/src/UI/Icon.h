#pragma once
#include <functional>
#include <engine/Shader.h>
#include <engine/Rect.h>
#include <UI/WidgetMH.h>

namespace ui
{
	class Icon : public Widget {

	public:

		Icon(const TextureRect& textureRect);
		Icon(const Icon& rhs);
		Icon(Icon&& rhs);
		virtual ~Icon();

		void setColor(const Vector4f& color);
		void setShader(Shader* shader);
		void setSpriteSheet(const unsigned int& spriteSheet);
		void setAligned(bool aligned);
		void setFlipped(bool flipped);

	private:

		void drawDefault() override;

		const TextureRect& textureRect;
		Vector4f m_color;
		Shader* m_shader;
		unsigned int m_spriteSheet;
		bool m_aligned;
		bool m_flipped;
	};

	class IconAnimated : public Widget {

	public:

		IconAnimated(const std::vector<TextureRect>& textureRects);
		IconAnimated(const IconAnimated& rhs);
		IconAnimated(IconAnimated&& rhs);
		virtual ~IconAnimated();

		void setColor(const Vector4f& color);
		void setShader(Shader* shader);
		void setSpriteSheet(const unsigned int& spriteSheet);
		void setCurrentFrame(const int currentFrame);
		void setAligned(bool aligned);
		void setFlipped(bool flipped);
		void setOffsetX(const float offsetX);
		void setOffsetY(const float offsetY);

	private:

		void drawDefault() override;

		const std::vector<TextureRect>& textureRects;
		Vector4f m_color;
		Shader* m_shader;
		unsigned int m_spriteSheet;
		int m_currentFrame;
		bool m_aligned;
		bool m_flipped;
		float m_offsetX, m_offsetY;
	};
}