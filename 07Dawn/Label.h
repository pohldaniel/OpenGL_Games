#pragma once

#include <functional>

#include "engine/CharacterSet.h"
#include "engine/Fontrenderer.h"

#include "Widget.h"



class Label : public Widget {

public:
	Label() = default;
	Label(const CharacterSet& characterSet, std::string text = "");
	~Label() = default;

	Label(Label const& rhs);
	Label& operator=(const Label& rhs);

	void draw() override;
	void processInput() override;

	void setFunction(std::function<void()> fun);

	std::string m_text;
	const CharacterSet* m_characterSet;

	virtual int getWidth() const override;
	virtual int getHeight() const override;

private:
	void draw(std::string text);
	bool m_hover = false;
	std::function<void()> m_fun = 0;

	Vector4f defaultColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4f hoverColor = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);

	Vector4f activeColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
};