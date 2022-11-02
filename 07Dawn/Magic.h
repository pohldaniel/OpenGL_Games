#pragma once

#include "SpellActionBase.h"
#include "Npc.h"

class CMagic {

public:
	CMagic(SpellActionBase *spell);
	SpellActionBase *getSpell();
	void setCreator(Character *c);

	void process();
	bool isDone();
	bool collision(Npc *npc);

private:
	SpellActionBase *spell;
	Character *creator;
};