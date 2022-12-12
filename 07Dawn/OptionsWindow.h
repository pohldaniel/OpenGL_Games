#pragma once

#include "Widget.h"
#include "Dialog.h"

class OptionsWindow : public Widget{

public:

	~OptionsWindow();
	void draw() override;
	void processInput() override;
	void resize(int deltaW, int deltaH) override;

	void setTextureDependentPositions();
	void init(std::vector<TextureRect> textures);

	static OptionsWindow& Get();

private:

	OptionsWindow();
	std::vector<TextureRect> m_textures;
	CharacterSet* m_font;
	static OptionsWindow s_instance;
};