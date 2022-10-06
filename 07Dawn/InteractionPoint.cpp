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
		TextureAtlasCreator::get().init(1024, 1024);
		TextureManager::Loadimage("res/interaction/talk0.tga", 0, m_interactionTextures);
		TextureManager::Loadimage("res/interaction/talk1.tga", 1, m_interactionTextures);		
		m_textureAtlas = TextureAtlasCreator::get().getAtlas();
		break;
	case Enums::InteractionType::Shop:
		TextureAtlasCreator::get().init(1024, 1024);
		TextureManager::Loadimage("res/interaction/shop0.tga", 0, m_interactionTextures);
		TextureManager::Loadimage("res/interaction/shop1.tga", 1, m_interactionTextures);
		m_textureAtlas = TextureAtlasCreator::get().getAtlas();
		
		break;
	case Enums::InteractionType::Zone:
		TextureAtlasCreator::get().init(1024, 1024);
		TextureManager::Loadimage("res/interaction/zone0.tga", 0, m_interactionTextures);
		TextureManager::Loadimage("res/interaction/zone1.tga", 1, m_interactionTextures);
		m_textureAtlas = TextureAtlasCreator::get().getAtlas();		
		break;
	}
}

void InteractionPoint::setBackgroundTexture(std::string texturename) {
	// We explicitely want to allow a background texture to change
	//if (backgroundTexture != NULL) {
		//delete backgroundTexture;
	//}


	TextureAtlasCreator::get().init(1024, 1024);
	TextureManager::Loadimage(texturename, 0, m_backgroundTextures);
	m_textureAtlas = TextureAtlasCreator::get().getAtlas();
}

void InteractionPoint::setInteractionCode(std::string interactionCode)
{
	this->interactionCode = interactionCode;
}

bool InteractionPoint::isMouseOver(int mouseX, int mouseY) const {
	if (mouseX + ViewPort::get().getPosition()[0] > posX
		&& mouseX + ViewPort::get().getPosition()[0] < posX + width
		&& mouseY + ViewPort::get().getPosition()[1] > posY
		&& mouseY + ViewPort::get().getPosition()[1] < posY + height) {
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

	//DrawingHelpers::mapTextureToRect(backgroundTexture->getTexture(0), posX, width, posY, height);
}

void InteractionPoint::drawInteractionSymbol(int mouseX, int mouseY, int characterXpos, int characterYpos) {

	if (markedAsDeletable) {
		return;
	}

	if (!isMouseOver(mouseX, mouseY)) {
		return;
	}

	uint8_t available_symbol = 0;


	if (isInRange(characterXpos, characterYpos))
	{
		available_symbol = 1;
	}

	//DrawingHelpers::mapTextureToRect(interactionTexture->getTexture(available_symbol),
	//	mouseX + world_x,
	//	interactionTexture->getTexture(available_symbol).width,
	//	mouseY + world_y,
	//	interactionTexture->getTexture(available_symbol).height);
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

CharacterInteractionPoint::CharacterInteractionPoint(Character *character_) : interactionCharacter(character_) {
}

bool CharacterInteractionPoint::isMouseOver(int mouseX, int mouseY) const {
	int posX = interactionCharacter->getXPos();
	int posY = interactionCharacter->getYPos();
	int width = interactionCharacter->getWidth();
	int height = interactionCharacter->getHeight();

	if (mouseX + ViewPort::get().getPosition()[0] > posX
		&& mouseX + ViewPort::get().getPosition()[0] < posX + width
		&& mouseY + ViewPort::get().getPosition()[1] > posY
		&& mouseY + ViewPort::get().getPosition()[1] < posY + height) {
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