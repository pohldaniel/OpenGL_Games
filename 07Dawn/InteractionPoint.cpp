#include "InteractionPoint.h"
#include "Character.h"
#include "Zone.h"

unsigned int InteractionPoint::TextureAtlas;
std::vector<TextureRect> InteractionPoint::Textures;

InteractionPoint::InteractionPoint()
	: interactionCode(""),
	posX(0),
	posY(0),
	width(0),
	height(0),
	markedAsDeletable(false) {
	m_interactionTextures.reserve(2);
}

InteractionPoint::~InteractionPoint() {
}

void InteractionPoint::setPosition(int posX, int posY, int width, int height) {
	this->posX = posX;
	this->posY = posY;
	this->width = width;
	this->height = height;
}

void InteractionPoint::setInteractionType(Enums::InteractionType interactionType) {
	this->interactionType = interactionType;

	switch (interactionType){

	case Enums::InteractionType::Quest:
		m_interactionTextures[0] = Textures[0];
		m_interactionTextures[1] = Textures[1];
		break;
	case Enums::InteractionType::Shop:
		m_interactionTextures[0] = Textures[2];
		m_interactionTextures[1] = Textures[3];
		
		break;
	case Enums::InteractionType::Zone:
		m_interactionTextures[0] = Textures[4];
		m_interactionTextures[1] = Textures[5];
		break;
	}
}

void InteractionPoint::setBackgroundTexture(std::string texturename, bool transparent) {
	TextureAtlasCreator::Get().init("background", 128, 128);
	m_backgroundTexture = TextureManager::Loadimage(texturename, transparent);
	m_backgroundAtlas = TextureAtlasCreator::Get().getAtlas();
}

void InteractionPoint::setInteractionCode(std::string interactionCode)
{
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
	TextureManager::BindTexture(m_backgroundAtlas, true);
	TextureManager::DrawTexture(m_backgroundTexture, posX, posY, width, height, true, true);
	TextureManager::UnbindTexture(true);
}

void InteractionPoint::drawInteractionSymbol(int mouseX, int mouseY, int characterXpos, int characterYpos) {

	if (markedAsDeletable) {
		return;
	}

	if (!isMouseOver(mouseX, mouseY)) {
		return;
	}

	uint8_t available_symbol = 0;


	if (isInRange(characterXpos, characterYpos)){
		available_symbol = 1;
	}

	TextureManager::DrawTextureBatched(m_interactionTextures[available_symbol], mouseX, mouseY,  true, true);

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

std::string toStringForLua(Enums::InteractionType interactionType) {
	switch (interactionType) {
		case Enums::InteractionType::Quest:
			return "InteractionType.Quest";
		case Enums::InteractionType::Shop:
			return "InteractionType.Shop";
		case Enums::InteractionType::Zone:
			return "InteractionType.Zone";
	}
}

std::string InteractionPoint::getLuaSaveText() const {
	std::ostringstream oss;
	std::string objectName = "curInteractionPoint";

	oss << "local " << objectName << " = DawnInterface.addInteractionPoint();" << std::endl;
	oss << objectName << ":setPosition( " << posX << ", " << posY << ", " << width << ", " << height << " );" << std::endl;
	//if (backgroundTexture != NULL) {
	//	oss << objectName << ":setBackgroundTexture( \"" << backgroundTexture->getTexture(0).textureFile << "\" );" << std::endl;
	//}
	//if (interactionTexture != NULL) {
	//	oss << objectName << ":setInteractionType( " << toStringForLua(interactionType) << " );" << std::endl;
	//}
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

void InteractionPoint::Init(unsigned int textureAtlas, std::vector<TextureRect> textures) {
	TextureAtlas = textureAtlas;
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

Character *CharacterInteractionPoint::getCharacter() const{
	return interactionCharacter;
}

void CharacterInteractionPoint::draw() {
	// no drawing since the character is what is drawn
}

std::string CharacterInteractionPoint::getLuaSaveText() const {
	std::ostringstream oss;
	std::string objectName = "curInteractionPoint";
	std::string characterReference = DawnInterface::getItemReferenceRestore(interactionCharacter);
	oss << "local " << objectName << " = DawnInterface.addCharacterInteractionPoint( " << characterReference << " );" << std::endl;

	//if (backgroundTexture != NULL)
	//{
	//	oss << objectName << ":setBackgroundTexture( \"" << backgroundTexture->getTexture(0).textureFile << "\" );" << std::endl;
	//}

	//if (interactionTexture != NULL)
	//{
	//	oss << objectName << ":setInteractionType( " << toStringForLua(interactionType) << " );" << std::endl;
	//}
	oss << objectName << ":setInteractionCode( [[" << interactionCode << "]] );" << std::endl;

	return oss.str();
}

void CharacterInteractionPoint::AddCharacterInteractionPoint(CharacterInteractionPoint *characterInteractionPoint) {
	ZoneManager::Get().getCurrentZone()->addCharacterInteractionPoint(characterInteractionPoint);
}