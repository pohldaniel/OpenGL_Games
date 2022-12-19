#pragma once
#include <vector>

#include "engine/Vector.h"
#include "engine/animation/Animation2D.h"
#include "engine/Clock.h"

namespace Enums {
	enum WeaponType;
	enum CharacterStates : int;
	enum EffectType;
	enum ElementType;
	enum StatsType;
	enum CharacterClass;
}

class Character;
struct TextureRect;

class SpellActionBase {
protected:
	SpellActionBase();
public:
	// Question: What about different target types, such as position (for region damage spells)
	/// \brief Creates a spell to really cast as a copy from this one with a fixed target and creator.
	virtual SpellActionBase* cast(Character *creator, Character *target, bool child) = 0;
	virtual SpellActionBase* cast(Character *creator, int x, int y) = 0;
	virtual ~SpellActionBase();

	/// Info about the spell, both for the game (mana cost, etc) and for the player (name, info, etc)

	/// \brief Returns the time needed to cast a spell.
	/// \todo Should the spell meta information be put in a separate info object?
	virtual uint16_t getCastTime() const = 0;
	virtual uint16_t getCooldown() const = 0;
	virtual uint16_t getSpellCost() const = 0;
	virtual uint16_t getDuration() const = 0;
	virtual uint16_t getRadius() const = 0;
	virtual int16_t getX() const = 0;
	virtual int16_t getY() const = 0;
	virtual bool isInRange(double distance) const = 0;
	virtual std::string getName() const = 0;
	virtual std::string getID() const;
	virtual std::string getInfo() const = 0;
	virtual Enums::EffectType getEffectType() const = 0;

	/// Note: The following functions may only be called on spells that where really created,
	///       i.e. for spells created by cast function.

	/// \brief Draws the graphical representation of the spell effect.
	virtual void draw() = 0;

	virtual void update(float deltatime) {};
	virtual void startAnimation() {};
	virtual const bool waitForAnimation() const { return false; };
	/// \brief Called directly after creation of the spell and is thought for an initial effect.
	virtual void startEffect() = 0;
	/// \brief Called while the spell is not yet completed and thought for continuous effects.
	/// Needs to mark the spell as completed unless that has already been done,
	/// e.g. because the effect is not continuous.
	virtual void inEffect(float deltatime) = 0;

	/// \brief Whether the spell has done its work and the object can be destroyed.
	bool isEffectComplete() const;
	void unbindFromCreator();
	bool isBoundToCreator() const;
	void beginPreparationOfSpellAction();
	void markSpellActionAsFinished();

	/// \brief Functions for defining and playing sounds.
	void setSoundSpellCasting(std::string soundSpellCasting);
	void setSoundSpellStart(std::string soundSpellStart);
	void setSoundSpellHit(std::string soundSpellHit);
	void playSoundSpellCasting();
	//void playSoundSpellStart();
	//void playSoundSpellHit();
	void stopSoundSpellCasting();
	//void stopSoundSpellStart();
	//void stopSoundSpellHit();

	/// add additional spells to this spell, to be executed based on chance when the spell is finished.
	void addAdditionalSpellOnTarget(SpellActionBase *spell, double chanceToExecute);
	void addAdditionalSpellOnCreator(SpellActionBase *spell, double chanceToExecute);

	/// which class can use the spell
	void setRequiredClass(Enums::CharacterClass requiredClass);
	Enums::CharacterClass getRequiredClass() const;

	/// which equipment (weapon) is required to cast the spell.
	void addRequiredWeapon(Enums::WeaponType weaponType);
	uint32_t getRequiredWeapons() const;

	/// what level we can use the spell
	void setRequiredLevel(uint8_t requiredLevel);
	uint8_t getRequiredLevel() const;

	/// rank of the spell
	void setRank(uint8_t rank);
	uint8_t getRank() const;

	/// instant spells, doesn't require spell to "hit" the target to do damge (ranged + bolt spells)
	void setInstant(bool instant);
	bool getInstant() const;

	/// characterStates which affects the target. Stunned, charmed etc...
	void setCharacterState(Enums::CharacterStates characterState, float value = 1.0f);
	std::pair<Enums::CharacterStates, float> getCharacterState() const;

	void setNeedTarget(bool needTarget);
	bool getNeedTarget();

	bool isSpellHostile() const;

	void drawSymbol(int left, int bottom, float width, float height, Vector4f color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f), bool updateView = false) const;
	void drawSymbolSingle(int left, int bottom, float width, float height, Vector4f color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f), bool updateView = false) const;
	/// this resets the luaID to empty. It is only to be used in copySpell-function because there we want to create a new spell which
	/// will be inscribed in the spell database so it will get an ID.
	void unsetLuaID();
	void setSymbolTextureRect(TextureRect& textureRect);
	TextureRect* getSymbol() const;
	Clock m_timer;

protected:
	Character *creator = nullptr;
	Character *target = nullptr;
	bool boundToCreator;
	bool finished;
	bool hostileSpell;
	bool instant; // makes the spell instant, doesn't require the spell to "hit" the target. (ranged + bolt spells)
	mutable std::string luaID; // for caching the id and not having to get it every time again
	Enums::CharacterClass requiredClass;
	std::string soundSpellCasting;
	std::string soundSpellStart;
	std::string soundSpellHit;
	uint8_t requiredLevel;
	uint32_t requiredWeapons;
	uint8_t rank;
	std::vector< std::pair<SpellActionBase*, double> > additionalSpellsOnTarget;
	std::vector< std::pair<SpellActionBase*, double> > additionalSpellsOnCreator;
	std::pair<Enums::CharacterStates, float> characterStateEffects;

	TextureRect* spellSymbol = nullptr;
	bool needTarget;
};