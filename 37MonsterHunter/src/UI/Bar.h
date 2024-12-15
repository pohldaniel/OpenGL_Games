#pragma once
#include <UI/Widget.h>
#include <engine/TileSet.h>

namespace ui
{
	class Bar : public Widget {

	public:

		Bar(const TileSet& tileSet);
		Bar(const Bar& rhs);
		Bar(Bar&& rhs);
		virtual ~Bar();

		void setValue(const float value);
		void setMaxValue(const float maxValue);
		void setWidth(const float width);
		void setHeight(const float height);
		void setRadius(const float radius);
		void setColor(const Vector4f& color);
		void setBgColor(const Vector4f& bgColor);

	private:

		void drawDefault() override;

		const TileSet& tileSet;
		float m_value;
		float m_maxValue;
		float m_height;
		float m_width;
		Vector4f m_bgColor;
		Vector4f m_color;
		float m_radius;
	};
}