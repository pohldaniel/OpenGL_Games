#pragma once

#include <functional>

#include "engine/CharacterSet.h"
#include "engine/Batchrenderer.h"

#include "Widget.h"

#include "Constants.h"

class Label : public Widget {

public:
	Label() = default;
	Label(const CharacterSet& characterSet, std::string text = "");
	~Label() = default;

	Label(Label const& rhs);
	Label& operator=(const Label& rhs);

	void draw();
	void draw(std::string text);
	void update(int mouseX, int mouseY);

	void setFunction(std::function<void()> fun);

	std::string m_text;
	const CharacterSet* m_characterSet;

	virtual int getWidth() const override;
	virtual int getHeight() const override;

	bool m_hover = false;
	std::function<void()> m_fun = 0;

	Vector4f defaultColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4f hoverColor = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);

	Vector4f activeColor;
};