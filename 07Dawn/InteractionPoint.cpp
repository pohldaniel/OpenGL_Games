#include "InteractionPoint.h"
#include "Character.h"
#include "Zone.h"

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

void InteractionPoint::setInteractionType(Enums::InteractionType interactionType) {
	// We explicitely want to allow an interaction texture to change
	//if (interactionTexture != NULL) {
		//delete interactionTexture;
	//}
	this->interactionType = interactionType;
	//interactionTexture = new CTexture();
	switch (interactionType){

	case Enums::InteractionType::Quest:
		TextureAtlasCreator::Get().init("interaction", 1024, 1024);
		TextureManager::Loadimage("res/interaction/talk0.tga", 0, m_interactionTextures, true);
		TextureManager::Loadimage("res/interaction/talk1.tga", 1, m_interactionTextures, true);
		m_textureAtlas = TextureAtlasCreator::Get().getAtlas();
		break;
	case Enums::InteractionType::Shop:
		TextureAtlasCreator::Get().init("interaction", 1024, 1024);
		TextureManager::Loadimage("res/interaction/shop0.tga", 0, m_interactionTextures, true);
		TextureManager::Loadimage("res/interaction/shop1.tga", 1, m_interactionTextures, true);
		m_textureAtlas = TextureAtlasCreator::Get().getAtlas();
		
		break;
	case Enums::InteractionType::Zone:
		TextureAtlasCreator::Get().init("interaction", 1024, 1024);
		TextureManager::Loadimage("res/interaction/zone0.tga", 0, m_interactionTextures, true);
		TextureManager::Loadimage("res/interaction/zone1.tga", 1, m_interactionTextures, true);
		m_textureAtlas = TextureAtlasCreator::Get().getAtlas();		
		break;
	}
}

void InteractionPoint::setBackgroundTexture(std::string texturename, bool transparent) {
	TextureAtlasCreator::Get().init("background", 1024, 1024);
	TextureManager::Loadimage(texturename, 0, m_backgroundTextures, transparent);
	m_textureAtlas2 = TextureAtlasCreator::Get().getAtlas();
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
	TextureManager::BindTexture(m_textureAtlas2, true);
	TextureManager::DrawTexture(m_backgroundTextures[0], posX, posY, width, height, true, true);
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

	TextureManager::BindTexture(m_textureAtlas, true);
	TextureManager::DrawTexture(m_interactionTextures[available_symbol], mouseX, mouseY,  true, true);
	TextureManager::UnbindTexture(true);
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

void InteractionPoint::processInput(int mouseX, int mouseY, int characterXpos, int characterYpos) {
	Mouse &mouse = Mouse::instance();
	if (mouse.buttonPressed(Mouse::BUTTON_RIGHT)) {
		if (isMouseOver(mouseX, mouseY)){
			startInteraction(characterXpos, characterYpos);
		}
	}
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