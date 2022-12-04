#include "SpellActionBase.h"

#include "Luafunctions.h"
#include "TextureManager.h"
#include"Character.h"

SpellActionBase::SpellActionBase()
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

SpellActionBase::~SpellActionBase() {
	if (boundToCreator) {
		creator->curSpellAction = NULL;
		creator->isPreparing = false;
	}
}

void SpellActionBase::unbindFromCreator() {

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
		}else {
			creator->modifyCurrentMana(-spellCost);
		}
	}
}

bool SpellActionBase::isBoundToCreator() const {
	return boundToCreator;
}

void SpellActionBase::beginPreparationOfSpellAction() {
	boundToCreator = true;
}

void SpellActionBase::markSpellActionAsFinished() {
	unbindFromCreator();
	finished = true;
}

bool SpellActionBase::isEffectComplete() const {
	return finished;
}

void SpellActionBase::drawSymbol(int left, int bottom, float width, float height, Vector4f color, bool updateView) const {
	TextureRect *texture = getSymbol();
	TextureManager::DrawTextureBatched(*texture, left, bottom, width, height, color, false, updateView);
	//Batchrenderer::Get().addQuadAA(Vector4f(static_cast< float >(left), static_cast< float >(bottom), width, height), Vector4f(texture->textureOffsetX, texture->textureOffsetY, texture->textureWidth, texture->textureHeight), color, updateView, 0u, texture->frame);
}

void SpellActionBase::drawSymbolSingle(int left, int bottom, float width, float height, Vector4f color, bool updateView) const {
	TextureRect* texture = getSymbol();
	TextureManager::DrawTexture(*texture, left, bottom, width, height, color, false, updateView);
	//Batchrenderer::Get().drawSingleQuadAA(Vector4f(static_cast< float >(left), static_cast< float >(bottom), width, height), Vector4f(texture->textureOffsetX, texture->textureOffsetY, texture->textureWidth, texture->textureHeight), color, updateView, 0u, texture->frame);
}

std::string SpellActionBase::getID() const {
	if (luaID.size() == 0) {
		luaID = LuaFunctions::getIDFromLuaTable("spellDatabase", this);
	}
	return luaID;
}

void SpellActionBase::unsetLuaID() {
	luaID = "";
}

void SpellActionBase::addAdditionalSpellOnTarget(SpellActionBase *spell, double chanceToExecute) {
	additionalSpellsOnTarget.push_back(std::pair<SpellActionBase*, double>(spell, chanceToExecute));
}

void SpellActionBase::addAdditionalSpellOnCreator(SpellActionBase *spell, double chanceToExecute) {
	additionalSpellsOnCreator.push_back(std::pair<SpellActionBase*, double>(spell, chanceToExecute));
}

void SpellActionBase::setRequiredClass(Enums::CharacterClass requiredClass) {
	this->requiredClass = requiredClass;
}

void SpellActionBase::addRequiredWeapon(Enums::WeaponType weaponType) {
	requiredWeapons |= weaponType;
}

uint32_t SpellActionBase::getRequiredWeapons() const {
	return requiredWeapons;
}

Enums::CharacterClass SpellActionBase::getRequiredClass() const {
	return requiredClass;
}

void SpellActionBase::setRequiredLevel(uint8_t requiredLevel) {
	this->requiredLevel = requiredLevel;
}

uint8_t SpellActionBase::getRequiredLevel() const {
	return requiredLevel;
}

void SpellActionBase::setRank(uint8_t rank) {
	this->rank = rank;
}

uint8_t SpellActionBase::getRank() const {
	return rank;
}

void SpellActionBase::setInstant(bool instant) {
	this->instant = instant;
}

bool SpellActionBase::getInstant() const {
	return instant;
}

void SpellActionBase::setSoundSpellCasting(std::string soundSpellCasting) {
	this->soundSpellCasting = soundSpellCasting;
}

void SpellActionBase::setSoundSpellStart(std::string soundSpellStart) {
	this->soundSpellStart = soundSpellStart;
}

void SpellActionBase::setSoundSpellHit(std::string soundSpellHit) {
	this->soundSpellHit = soundSpellHit;
}

void SpellActionBase::playSoundSpellCasting() {
	if (soundSpellCasting != "") {
		//SoundEngine::playSound(soundSpellCasting);
	}
}

void SpellActionBase::stopSoundSpellCasting() {
	if (soundSpellCasting != "") {
		//SoundEngine::stopSound(soundSpellCasting);
	}
}

void SpellActionBase::playSoundSpellStart() {
	if (soundSpellStart != "") {
		//SoundEngine::playSound(soundSpellStart);
	}
}

void SpellActionBase::stopSoundSpellStart() {
	if (soundSpellStart != "") {
		//SoundEngine::stopSound(soundSpellStart);
	}
}

void SpellActionBase::playSoundSpellHit() {
	if (soundSpellHit != "") {
		//SoundEngine::playSound(soundSpellHit);
	}
}

void SpellActionBase::stopSoundSpellHit() {
	if (soundSpellHit != "") {
		//SoundEngine::stopSound(soundSpellHit);
	}
}

bool SpellActionBase::isSpellHostile() const {
	return hostileSpell;
}

void SpellActionBase::setCharacterState(Enums::CharacterStates characterState, float value) {
	characterStateEffects.first = characterState;
	characterStateEffects.second = value;
	hostileSpell = Enums::isStateConsideredHarmfull(characterState, value);
}

std::pair<Enums::CharacterStates, float> SpellActionBase::getCharacterState() const {
	return characterStateEffects;
}

void SpellActionBase::setSymbolTextureRect(TextureRect& textureRect) {
	spellSymbol = &textureRect;
}

void SpellActionBase::setNeedTarget(bool needTarget) {
	SpellActionBase::needTarget = needTarget;
}

bool SpellActionBase::getNeedTarget() {
	return needTarget;
}