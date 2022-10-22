#pragma once

#include "SpellActionBase.h"
#include "Npc.h"

class CMagic {

public:
	CMagic(CSpellActionBase *spell);
	CSpellActionBase *getSpell();
	void setCreator(Character *c);

	void process();
	bool isDone();
	bool collision(Npc *npc);

private:
	CSpellActionBase *spell;
	Character *creator;
};