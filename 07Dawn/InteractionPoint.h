#pragma once
#include <vector>
#include <string>

#include "Enums.h"
#include "Luainterface.h"
#include "ViewPort.h"
#include "TextureManager.h"

class Character;

class InteractionPoint {

public:
	InteractionPoint();
	~InteractionPoint();

	virtual void draw();
	virtual bool isMouseOver(int mouseX, int mouseY) const;
	virtual bool isInRange(int characterXpos, int characterYpos) const;

	void setPosition(int posX, int posY, int width, int height);
	void setInteractionType(Enums::InteractionType interactionType);
	void setBackgroundTexture(std::string texturename, bool transparent = false);
	void setInteractionCode(std::string interactionCode);
	
	void drawInteractionSymbol(int mouseX, int mouseY, int characterXpos, int characterYpos);
	void startInteraction(int characterXpos, int characterYpos);

	void markAsDeletable();
	bool isMarkedDeletable() const;

	virtual std::string getLuaSaveText() const;

	static void AddInteractionPoint(InteractionPoint* interactionPoint);
	static  std::vector<InteractionPoint*>& InteractionPoint::GetInteractionPoints();

protected:

	std::vector<TextureRect> m_interactionTextures;
	unsigned int m_textureAtlas;

	std::vector<TextureRect> m_backgroundTextures;
	unsigned int m_textureAtlas2;
	
	std::string interactionCode;
	Enums::InteractionType interactionType;

private:

	int posX;
	int posY;
	int width;
	int height;

	bool markedAsDeletable;
};

class CharacterInteractionPoint : public InteractionPoint {

public:
	CharacterInteractionPoint(Character *character);

	void draw() override;
	bool isMouseOver(int mouseX, int mouseY) const override;
	bool isInRange(int characterXpos, int characterYpos) const override;
	
	std::string getLuaSaveText() const override;
	Character *getCharacter() const;

	static void AddCharacterInteractionPoint(CharacterInteractionPoint *characterInteractionPoint);

private:

	Character *interactionCharacter;
};
