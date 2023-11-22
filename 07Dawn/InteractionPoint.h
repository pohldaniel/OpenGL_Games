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
	int getPosX();
	int getPosY();
	int getWidth();
	int getHeight();
	std::string getBackgroundTextureName();
	bool getBackgroundTextureTransparent();
	Enums::InteractionType getInteractionType();
	std::string getInteractionCode();

	void drawInteractionSymbol(int mouseX, int mouseY, int characterXpos, int characterYpos);
	void startInteraction(int characterXpos, int characterYpos);

	void markAsDeletable();
	bool isMarkedDeletable() const;
	std::string toStringForLua(Enums::InteractionType interactionType) const;

	virtual std::string getLuaSaveText() const;

	static void AddInteractionPoint(InteractionPoint* interactionPoint);
	static  std::vector<InteractionPoint*>& InteractionPoint::GetInteractionPoints();
	static void DrawSymbols();
	static void Draw();
	static void Init(std::vector<TextureRect> textures);

protected:

	unsigned short symbolIndex;

	TextureRect m_backgroundTexture;
	unsigned int m_backgroundAtlas;
	
	std::string interactionCode;
	Enums::InteractionType interactionType;
	std::string m_file;
	bool m_transparent;
	static std::vector<TextureRect> Textures;

private:

	int posX;
	int posY;
	int width;
	int height;

	bool markedAsDeletable;

	
	static std::unordered_map<std::string, TextureRect> BackgroundTextures;
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
