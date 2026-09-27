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

protected:

	const CharacterSet& characterSet;
	std::string m_text;
	Vector4f m_color;

private:

	virtual void createDefault() override;
	void inputDefault(int mouseX, int mouseY, bool buttonLeft) override;
};