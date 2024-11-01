#pragma once
#include <functional>
#include <engine/CharacterSet.h>
#include <engine/Shader.h>
#include <UI/WidgetMH.h>

class Label : public WidgetMH {

public:

	Label(const CharacterSet& characterSet);
	Label(const Label& rhs);
	Label(Label&& rhs);
	virtual ~Label();

	void draw() override;

	void setDrawFunction(std::function<void()> fun);
	void setLabel(const std::string& label);
	void setColor(const Vector4f& color);
	void setSize(const float size);
	void setOffsetX(const float offsetX);
	void setOffsetY(const float offsetY);

protected:

	const CharacterSet& characterSet;
	float m_offsetX, m_offsetY;
	std::string m_label;
	Vector4f m_color;
	float m_size;

private:

	virtual void drawDefault();
	std::function<void()> m_draw;
};

enum Edge : int;
class TextFieldMH : public Label {

public:

	TextFieldMH(const CharacterSet& characterSet);
	TextFieldMH(const TextFieldMH& rhs);
	TextFieldMH(TextFieldMH&& rhs);
	virtual ~TextFieldMH();

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