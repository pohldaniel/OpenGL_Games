#pragma once
#include <vector>
#include <string>

#include "Enums.h"
#include "Luainterface.h"
#include "ViewPort.h"
#include "TextureManager.h"

class InteractionPoint;
class CharacterInteractionPoint;
class Character;

class InteractionPoint {
public:
	~InteractionPoint();
	void setPosition(int posX, int posY, int width, int height);
	void setInteractionType(Enums::InteractionType interactionType);
	void setBackgroundTexture(std::string texturename);
	void setInteractionCode(std::string interactionCode);

	virtual bool isMouseOver(int mouseX, int mouseY) const;
	virtual bool isInRange(int characterXpos, int characterYpos) const;
	virtual void draw();
	virtual void drawInteractionSymbol(int mouseX, int mouseY, int characterXpos, int characterYpos);
	void startInteraction(int characterXpos, int characterYpos);

	bool isMarkedDeletable() const;

	virtual std::string getLuaSaveText() const;

	
protected:
	InteractionPoint();

	std::vector<TextureRect> m_interactionTextures;
	unsigned int m_textureAtlas;

	std::vector<TextureRect> m_backgroundTextures;
	unsigned int m_textureAtlas2;
	
	std::string interactionCode;
	Enums::InteractionType interactionType;

private:
	friend const InteractionPoint& DawnInterface::addInteractionPoint();
	friend void DawnInterface::removeInteractionPoint(InteractionPoint *pointToRemove);

	void markAsDeletable();

	int posX;
	int posY;
	int width;
	int height;

	bool markedAsDeletable;
};

class CharacterInteractionPoint : public InteractionPoint {
public:
	virtual bool isMouseOver(int mouseX, int mouseY) const;
	virtual bool isInRange(int characterXpos, int characterYpos) const;
	virtual void draw();
	virtual Character *getCharacter() const;

	virtual std::string getLuaSaveText() const;
private:
	friend const InteractionPoint& DawnInterface::addCharacterInteractionPoint(Character *character);

	CharacterInteractionPoint(Character *character);
	Character *interactionCharacter;
};