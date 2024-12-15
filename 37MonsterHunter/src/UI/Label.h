#pragma once
#include <functional>
#include <engine/CharacterSet.h>
#include <engine/Shader.h>
#include <UI/Widget.h>

namespace ui
{
	class Label : public Widget {

	public:

		Label(const CharacterSet& characterSet);
		Label(const Label& rhs);
		Label(Label&& rhs);
		virtual ~Label();

		void setLabel(const std::string& label);
		void setTextColor(const Vector4f& textColor);
		void setSize(const float size);
		void setOffsetX(const float offsetX);
		void setOffsetY(const float offsetY);

	protected:

		const CharacterSet& characterSet;
		float m_offsetX, m_offsetY;
		std::string m_label;
		Vector4f m_textColor;
		float m_size;

	private:

		virtual void drawDefault() override;
	};

	enum Edge : int;
	class TextField : public Label {

	public:

		TextField(const CharacterSet& characterSet);
		TextField(const TextField& rhs);
		TextField(TextField&& rhs);
		virtual ~TextField();

		void setShader(Shader* shader);
		void setBackgroundColor(const Vector4f& color);
		void setEdge(Edge edge);
		void setBorderRadius(float borderRadius);
		void setPaddingX(const float paddingX);
		void setPaddingY(const float paddingY);

	private:

		void drawDefault() override;

		Vector4f m_backgroundColor;
		float m_paddingX, m_paddingY;
		float m_borderRadius;
		Edge m_edge;
		Shader* m_shader;
	};
}