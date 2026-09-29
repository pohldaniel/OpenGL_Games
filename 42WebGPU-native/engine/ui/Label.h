#pragma once

#include <engine/ui/Widget.h>
#include <engine/CharacterSet.h>

class Label : public Widget {

public:

	Label(const CharacterSet& characterSet);
	Label(const Label& rhs);
	Label(Label&& rhs) noexcept;
	~Label();

	void setText(const std::string& text);
	void setColor(const Vector4f& color);
	void setPadding(float paddingX, float paddingY, bool silent = false) override;

protected:

	const CharacterSet& characterSet;
	std::string m_text;
	Vector4f m_color;

private:

	void inputDefault(int mouseX, int mouseY, bool buttonLeft) override;
	void layoutDefault() override;
	void createDefault() override;
};