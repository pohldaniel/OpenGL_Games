#include "SpellActionBase.h"

#include "Luafunctions.h"
#include "TextureManager.h"
#include"Character.h"

CSpellActionBase::CSpellActionBase()
	: boundToCreator(false),
	finished(true),
	instant(false),
	requiredClass(Enums::CharacterClass::NOCLASS),
	requiredLevel(1),
	requiredWeapons(0),
	rank(1),
	luaID(""),
	needTarget(false){
	characterStateEffects.first = Enums::CharacterStates::NOEFFECT;
	characterStateEffects.second = 1.0f;
}

CSpellActionBase::~CSpellActionBase() {
	if (boundToCreator) {
		creator->curSpellAction = NULL;
		creator->isPreparing = false;
	}
}

void CSpellActionBase::unbindFromCreator() {

	if (boundToCreator) {
		creator->curSpellAction = NULL;
		creator->isPreparing = false;
		boundToCreator = false;
		uint16_t spellCost = getSpellCost();

		// if we're confused while casting, we add 20% more to the spell cost.
		if (creator->isConfused() == true) {
			spellCost *= 1.20;
		}


		if (creator->getArchType() == Enums::CharacterArchType::Fighter) {
			creator->modifyCurrentFatigue(-spellCost);
		}
		else {
			creator->modifyCurrentMana(-spellCost);
		}
	}
}

bool CSpellActionBase::isBoundToCreator() const {
	return boundToCreator;
}

void CSpellActionBase::beginPreparationOfSpellAction()
{
	boundToCreator = true;
}

void CSpellActionBase::markSpellActionAsFinished()
{
	unbindFromCreator();
	finished = true;
}

bool CSpellActionBase::isEffectComplete() const
{
	return finished;
}

void CSpellActionBase::drawSymbol(int left, int bottom, int width, int height, Vector4f color) const {
	TextureRect* texture = getSymbol();
	if (texture != NULL) {
		TextureManager::DrawTextureBatched(*texture, left, bottom, width, height, color, false, false);
	}
}

std::string CSpellActionBase::getID() const {

	if (luaID.size() == 0) {
		luaID = LuaFunctions::getIDFromLuaTable("spellDatabase", this);
	}
	return luaID;
}

void CSpellActionBase::unsetLuaID() {
	luaID = "";
}

void CSpellActionBase::addAdditionalSpellOnTarget(CSpellActionBase *spell, double chanceToExecute) {

	additionalSpellsOnTarget.push_back(std::pair<CSpellActionBase*, double>(spell, chanceToExecute));
}

void CSpellActionBase::addAdditionalSpellOnCreator(CSpellActionBase *spell, double chanceToExecute) {

	additionalSpellsOnCreator.push_back(std::pair<CSpellActionBase*, double>(spell, chanceToExecute));
}

void CSpellActionBase::setRequiredClass(Enums::CharacterClass requiredClass) {

	this->requiredClass = requiredClass;
}

void CSpellActionBase::addRequiredWeapon(Enums::WeaponType weaponType) {

	requiredWeapons |= weaponType;
}

uint32_t CSpellActionBase::getRequiredWeapons() const {

	return requiredWeapons;
}

Enums::CharacterClass CSpellActionBase::getRequiredClass() const {

	return requiredClass;
}

void CSpellActionBase::setRequiredLevel(uint8_t requiredLevel) {
	this->requiredLevel = requiredLevel;
}

uint8_t CSpellActionBase::getRequiredLevel() const {
	return requiredLevel;
}

void CSpellActionBase::setRank(uint8_t rank) {
	this->rank = rank;
}

uint8_t CSpellActionBase::getRank() const {
	return rank;
}

void CSpellActionBase::setInstant(bool instant) {
	this->instant = instant;
}

bool CSpellActionBase::getInstant() const {
	return instant;
}

void CSpellActionBase::setSoundSpellCasting(std::string soundSpellCasting) {
	this->soundSpellCasting = soundSpellCasting;
}

void CSpellActionBase::setSoundSpellStart(std::string soundSpellStart) {
	this->soundSpellStart = soundSpellStart;
}

void CSpellActionBase::setSoundSpellHit(std::string soundSpellHit) {
	this->soundSpellHit = soundSpellHit;
}

void CSpellActionBase::playSoundSpellCasting() {
	if (soundSpellCasting != "") {
		//SoundEngine::playSound(soundSpellCasting);
	}
}

void CSpellActionBase::stopSoundSpellCasting() {
	if (soundSpellCasting != "") {
		//SoundEngine::stopSound(soundSpellCasting);
	}
}

void CSpellActionBase::playSoundSpellStart() {
	if (soundSpellStart != "") {
		//SoundEngine::playSound(soundSpellStart);
	}
}

void CSpellActionBase::stopSoundSpellStart() {
	if (soundSpellStart != "") {
		//SoundEngine::stopSound(soundSpellStart);
	}
}

void CSpellActionBase::playSoundSpellHit() {
	if (soundSpellHit != "") {
		//SoundEngine::playSound(soundSpellHit);
	}
}

void CSpellActionBase::stopSoundSpellHit() {
	if (soundSpellHit != "") {
		//SoundEngine::stopSound(soundSpellHit);
	}
}

bool CSpellActionBase::isSpellHostile() const {
	return hostileSpell;
}

void CSpellActionBase::setCharacterState(Enums::CharacterStates characterState, float value) {

	characterStateEffects.first = characterState;
	characterStateEffects.second = value;
	hostileSpell = Enums::isStateConsideredHarmfull(characterState, value);
}

std::pair<Enums::CharacterStates, float> CSpellActionBase::getCharacterState() const {
	return characterStateEffects;
}

void CSpellActionBase::setSymbolTextureRect(TextureRect& textureRect) {
	spellSymbol = &textureRect;
}


const TextureRect CSpellActionBase::ConvertRect(const Animation2D::TextureRect& rect) {
	return{ rect.textureOffsetX , rect.textureOffsetY, rect.textureWidth , rect.textureHeight, rect.height, rect.width, rect.frame };
}

const Animation2D::TextureRect CSpellActionBase::ConvertRect(const TextureRect& rect) {
	return{ rect.textureOffsetX , rect.textureOffsetY, rect.textureWidth , rect.textureHeight, rect.height, rect.width, rect.frame };
}

void CSpellActionBase::setNeedTarget(bool needTarget) {
	CSpellActionBase::needTarget = needTarget;
}

bool CSpellActionBase::getNeedTarget() {
	return needTarget;
}