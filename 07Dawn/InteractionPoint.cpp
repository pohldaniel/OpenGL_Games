#include "InteractionPoint.h"
#include "Player.h"
#include "Zone.h"
#include "Constants.h"

std::vector<TextureRect> InteractionPoint::Textures;
std::unordered_map<std::string, TextureRect> InteractionPoint::BackgroundTextures;

InteractionPoint::InteractionPoint()
	: interactionCode(""),
	posX(0),
	posY(0),
	width(0),
	height(0),
	markedAsDeletable(false) {
}

InteractionPoint::~InteractionPoint() {
}

void InteractionPoint::setPosition(int posX, int posY, int width, int height) {
	this->posX = posX;
	this->posY = posY;
	this->width = width;
	this->height = height;
}

int InteractionPoint::getPosX() {
	return posX;
}

int InteractionPoint::getPosY() {
	return posY;
}

int InteractionPoint::getWidth() {
	return width;
}

int InteractionPoint::getHeight() {
	return height;
}

std::string InteractionPoint::getBackgroundTextureName() {
	return m_file;
}

bool InteractionPoint::getBackgroundTextureTransparent() {
	return m_transparent;
}

Enums::InteractionType InteractionPoint::getInteractionType() {
	return interactionType;
}

std::string InteractionPoint::getInteractionCode() {
	return interactionCode;
}

void InteractionPoint::setInteractionType(Enums::InteractionType interactionType) {
	this->interactionType = interactionType;

	switch (interactionType){

	case Enums::InteractionType::Quest:
		symbolIndex = 0;
		break;
	case Enums::InteractionType::Shop:
		symbolIndex = 2;	
		break;
	case Enums::InteractionType::Zone:
		symbolIndex = 4;
		break;
	}
}

void InteractionPoint::setBackgroundTexture(std::string texturename, bool transparent) {
	m_file = texturename;
	m_transparent = transparent;
	if (BackgroundTextures.count(texturename) > 0) {
		m_backgroundTexture = BackgroundTextures[texturename];
		return;
	}
	
	TextureAtlasCreator::Get().init("background", 128, 128);
	BackgroundTextures[texturename] = TextureManager::Loadimage(texturename, transparent);
	BackgroundTextures[texturename].frame = BackgroundTextures.size() - 1;

	m_backgroundTexture = BackgroundTextures[texturename];
	m_backgroundAtlas = Spritesheet::Merge(Globals::spritesheetManager.getAssetPointer("background")->getAtlas(), TextureAtlasCreator::Get().getAtlas(), false, false);
	Globals::spritesheetManager.getAssetPointer("background")->setAtlas(m_backgroundAtlas);
}

void InteractionPoint::setInteractionCode(std::string interactionCode) {
	this->interactionCode = interactionCode;
}

bool InteractionPoint::isMouseOver(int mouseX, int mouseY) const {
	if (mouseX > posX
		&& mouseX < posX + width
		&& mouseY > posY
		&& mouseY < posY + height) {
		return true;
	}
	return false;
}

bool InteractionPoint::isInRange(int characterXpos, int characterYpos) const {
	if (characterXpos > posX - 60
		&& characterXpos < posX + width + 60
		&& characterYpos > posY - 60
		&& characterYpos < posY + height + 60) {
		return true;
	}
	return false;
}

void InteractionPoint::draw() {

	if (markedAsDeletable) {
		return;
	}

	TextureManager::BindTexture(Globals::spritesheetManager.getAssetPointer("background")->getAtlas(), true, 7);
	TextureManager::DrawTextureBatched(m_backgroundTexture, posX, posY, width, height, true, true, 7u);
}

void InteractionPoint::drawInteractionSymbol(int mouseX, int mouseY, int characterXpos, int characterYpos) {

	if (markedAsDeletable) {
		return;
	}

	if (!isMouseOver(mouseX, mouseY)) {
		return;
	}

	uint8_t available_symbol = symbolIndex;


	if (isInRange(characterXpos, characterYpos)){
		available_symbol += 1;
	}

	TextureManager::DrawTextureBatched(Textures[available_symbol], mouseX, mouseY,  true, true);

}

void InteractionPoint::startInteraction(int characterXpos, int characterYpos) {
	if (markedAsDeletable) {
		return;
	}

	if (isInRange(characterXpos, characterYpos)) {
		LuaFunctions::executeLuaScript(interactionCode);
	}
}

bool InteractionPoint::isMarkedDeletable() const {
	return markedAsDeletable;
}

void InteractionPoint::markAsDeletable() {
	markedAsDeletable = true;
}

std::string InteractionPoint::toStringForLua(Enums::InteractionType interactionType) const {
	switch (interactionType) {
		case Enums::InteractionType::Quest:
			return "Enums.Quest";
		case Enums::InteractionType::Shop:
			return "Enums.Shop";
		case Enums::InteractionType::Zone:
			return "Enums.Zone";
	}
}

std::string InteractionPoint::getLuaSaveText() const {
	std::ostringstream oss;
	std::string objectName = "curInteractionPoint";

	oss << "local " << objectName << " = DawnInterface.addInteractionPoint();" << std::endl;
	oss << objectName << ":setPosition( " << posX << ", " << posY << ", " << width << ", " << height << " );" << std::endl;
	if (m_backgroundTexture.width >  0) {
		oss << objectName << ":setBackgroundTexture( \"" << m_file << "\" );" << std::endl;
	}

	if (Textures.size() > 0) {
		oss << objectName << ":setInteractionType( " << toStringForLua(interactionType) << " );" << std::endl;
	}
	oss << objectName << ":setInteractionCode( [[" << interactionCode << "]] );" << std::endl;

	return oss.str();
}

void InteractionPoint::AddInteractionPoint(InteractionPoint* interactionPoint) {
	ZoneManager::Get().getCurrentZone()->addInteractionPoint(interactionPoint);
}

std::vector<InteractionPoint*>& InteractionPoint::GetInteractionPoints() {
	return ZoneManager::Get().getCurrentZone()->getInteractionPoints();
}

void InteractionPoint::Draw() {
	std::vector<InteractionPoint*> interactionPoints = GetInteractionPoints();
	for (size_t curInteractionNr = 0; curInteractionNr < interactionPoints.size(); ++curInteractionNr) {
		InteractionPoint *curInteraction = interactionPoints[curInteractionNr];
		curInteraction->draw();
	}
}

void InteractionPoint::DrawSymbols() {
	std::vector<InteractionPoint*> interactionPoints = GetInteractionPoints();
	for (size_t curInteractionNr = 0; curInteractionNr < interactionPoints.size(); ++curInteractionNr) {
		InteractionPoint *curInteraction = interactionPoints[curInteractionNr];
		curInteraction->drawInteractionSymbol(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY(), Player::Get().getXPos(), Player::Get().getYPos());
	}
}

void InteractionPoint::Init(std::vector<TextureRect> textures) {
	Textures = textures;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CharacterInteractionPoint::CharacterInteractionPoint(Character *character_) : interactionCharacter(character_) {
}

bool CharacterInteractionPoint::isMouseOver(int mouseX, int mouseY) const {
	int posX = interactionCharacter->getXPos();
	int posY = interactionCharacter->getYPos();
	int width = interactionCharacter->getWidth();
	int height = interactionCharacter->getHeight();

	if (mouseX > posX
		&& mouseX < posX + width
		&& mouseY > posY
		&& mouseY < posY + height) {
		return true;
	}
	return false;
}

bool CharacterInteractionPoint::isInRange(int characterXpos, int characterYpos) const {
	int posX = interactionCharacter->getXPos();
	int posY = interactionCharacter->getYPos();

	if (sqrt(pow(characterXpos - posX, 2) + pow(characterYpos - posY, 2)) > 120) {
		return false;
	}
	return true;
}

Character* CharacterInteractionPoint::getCharacter() const{
	return interactionCharacter;
}

void CharacterInteractionPoint::draw() {
	// no drawing since the character is what is drawn
}

std::string CharacterInteractionPoint::getLuaSaveText() const {

	std::ostringstream oss;
	std::string objectName = "curInteractionPoint";
	std::string characterReference = DawnInterface::getItemReferenceRestore("", interactionCharacter);

	oss << "local " << objectName << " = DawnInterface.addCharacterInteractionPoint( " << characterReference << " );" << std::endl;

	if (Textures.size() > 0) {
		oss << objectName << ":setInteractionType( " << toStringForLua(interactionType) << " );" << std::endl;
	}
	oss << objectName << ":setInteractionCode( [[" << interactionCode << "]] );" << std::endl;

	return oss.str();
}

void CharacterInteractionPoint::AddCharacterInteractionPoint(CharacterInteractionPoint *characterInteractionPoint) {
	ZoneManager::Get().getCurrentZone()->addCharacterInteractionPoint(characterInteractionPoint);
}