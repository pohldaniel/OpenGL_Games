#include "Spellbook.h"
#include "Constants.h"

Spellbook Spellbook::s_instance;

Spellbook& Spellbook::Get() {
	return s_instance;
}

Spellbook::Spellbook() : Widget(125, 50, 454, 672, 13, 15), m_floatingSpellSlot(SpellSlot(0, 0, 0, 0)) {
	
}

void Spellbook::setPlayer(Player* player) {
	m_player = player;
}

void Spellbook::init(unsigned int textureAtlas, std::vector<TextureRect> textures) {

	m_cooldownFont = &Globals::fontManager.get("verdana_11");
	m_textureAtlas = textureAtlas;
	m_textures = textures;

	spellSlot.push_back(SpellSlot(139, 539, 50, 50));
	spellSlot.push_back(SpellSlot(291, 539, 50, 50));

	spellSlot.push_back(SpellSlot(139, 422, 50, 50));
	spellSlot.push_back(SpellSlot(291, 422, 50, 50));

	spellSlot.push_back(SpellSlot(139, 307, 50, 50));
	spellSlot.push_back(SpellSlot(291, 307, 50, 50));

	spellSlot.push_back(SpellSlot(139, 190, 50, 50));
	spellSlot.push_back(SpellSlot(291, 190, 50, 50));

	nextPageButtonOffsetX = 386;
	nextPageButtonOffsetY = 53;

	previousPageButtonOffsetX = 63;
	previousPageButtonOffsetY = 53;

	pageButtonWidth = 32.0f;
	pageButtonHeight = 32.0f;

	curPage = 0;
	m_floatingSpell = NULL;

	Widget::addMoveableFrame(454, 22, 13, 665);
	Widget::addCloseButton(22, 22, 444, 666);
}

void Spellbook::draw() {
	if (!m_visible) return;
	TextureManager::BindTexture(m_textureAtlas, true, 0);
	// draw spellbook base
	TextureManager::DrawTextureBatched(m_textures[0], m_posX, m_posY, false, false);
	// draw placeholders, the actual spell symbol and title of the spell.
	for (unsigned int x = 0; x < spellSlot.size(); x++) {		
		TextureManager::DrawTextureBatched(m_textures[1], m_posX + spellSlot[x].posX, m_posY + spellSlot[x].posY, false, false);

		if (spellSlot[x].action != NULL){			
			spellSlot[x].action->drawSymbol(m_posX + spellSlot[x].posX + 2, m_posY + spellSlot[x].posY + 2, 46.0f, 46.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
			Fontrenderer::Get().addText(*m_cooldownFont, m_posX + spellSlot[x].posX + 25 - m_cooldownFont->getWidth(spellSlot[x].action->getName()) / 2, m_posY + spellSlot[x].posY - m_cooldownFont->lineHeight - 5, spellSlot[x].action->getName(), Vector4f(0.35f, 0.0f, 0.0f, 1.0f), false);
		}
	}

	// draw the right arrowsymbol if there are spells on the next page
	unsigned int numSlots = static_cast<unsigned int>(spellSlot.size());
	unsigned int numSpells = static_cast<unsigned int>(inscribedSpells.size());
	if (numSlots * (curPage + 1) < numSpells) {
		TextureManager::DrawTextureBatched(m_textures[3], m_posX + nextPageButtonOffsetX, m_posY + nextPageButtonOffsetY, pageButtonWidth, pageButtonHeight, false, false);
	}

	// if we are on another page than first page, draw left arrowsymbol to show that there are spells on a previous page.
	if (curPage > 0){
		TextureManager::DrawTextureBatched(m_textures[4], m_posX + previousPageButtonOffsetX, m_posY + previousPageButtonOffsetY, pageButtonWidth, pageButtonHeight, false, false);
	}
	TextureManager::DrawBuffer();	
}

void Spellbook::drawSpellTooltip(int mouseX, int mouseY) {
	if (!m_visible) return;
	int spellSlotId = getMouseOverSpellSlotId(mouseX, mouseY);
	if (spellSlotId >= 0 && spellSlot[spellSlotId].tooltip != NULL){
		spellSlot[spellSlotId].tooltip->draw(mouseX, mouseY);
	}
}

void Spellbook::drawFloatingSpell() {

	if (m_floatingSpell != NULL) {
		TextureManager::BindTexture(m_textureAtlas, true);
		m_floatingSpell->action->drawSymbolSingle(ViewPort::Get().getCursorPosX() + 2, ViewPort::Get().getCursorPosY() + 22, 46.0f, 46.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), true);
		Fontrenderer::Get().drawText(*m_cooldownFont, ViewPort::Get().getCursorPosX() + 25 - m_cooldownFont->getWidth(m_floatingSpell->action->getName()) / 2, ViewPort::Get().getCursorPosY() + 20 - m_cooldownFont->lineHeight - 5, m_floatingSpell->action->getName(), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), true);
		TextureManager::UnbindTexture(true);
	}
}

short Spellbook::getMouseOverSpellSlotId(int mouseX, int mouseY) const {
	for (short index = 0; index < spellSlot.size(); index++) {
		if (mouseX > spellSlot[index].posX + m_posX &&
			mouseX < spellSlot[index].posX + spellSlot[index].width + m_posX &&
			mouseY > spellSlot[index].posY + m_posY &&
			mouseY < spellSlot[index].posY + spellSlot[index].height + m_posY) {
			return index;
		}
	}
	return -1;
}

SpellSlot* Spellbook::getFloatingSpell() const {
	return m_floatingSpell;
}

void Spellbook::setFloatingSpell(SpellActionBase* newFloatingSpell) {
	m_floatingSpellSlot.action = newFloatingSpell;
	m_floatingSpell = &m_floatingSpellSlot;
}

void Spellbook::unsetFloatingSpell() {
	m_floatingSpell = NULL;
}

bool Spellbook::hasFloatingSpell() const {
	return m_floatingSpell != NULL;
}

void Spellbook::processInput() {
	if (!m_visible) return;
	Widget::processInput();

	Mouse &mouse = Mouse::instance();
	if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
		int spellSlotIndex = getMouseOverSpellSlotId(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());
		if (spellSlotIndex >= 0 && spellSlot[spellSlotIndex].action != NULL) {
			m_floatingSpell = &spellSlot[spellSlotIndex];
		}else if (isMouseOverNextPageArea(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY())) {
			nextPage();
		}else if (isMouseOverPreviousPageArea(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY())) {
			previousPage();

		}else {
			m_floatingSpell = NULL;
		}
	}
}

bool Spellbook::isMouseOverNextPageArea(int mouseX, int mouseY) {
	if (mouseX > m_posX + nextPageButtonOffsetX &&
		mouseX < m_posX + nextPageButtonOffsetX + pageButtonWidth &&
		mouseY > m_posY + nextPageButtonOffsetY &&
		mouseY < m_posY + nextPageButtonOffsetY + pageButtonHeight) {
		return true;
	}

	return false;
}

bool Spellbook::isMouseOverPreviousPageArea(int mouseX, int mouseY) {
	if (mouseX > m_posX + previousPageButtonOffsetX &&
		mouseX < m_posX + previousPageButtonOffsetX + pageButtonWidth &&
		mouseY > m_posY + previousPageButtonOffsetY &&
		mouseY < m_posY + previousPageButtonOffsetY + pageButtonHeight) {
		return true;
	}

	return false;
}

void Spellbook::reloadSpellsFromPlayer() {
	inscribedSpells = m_player->getSpellbook();
	refreshPage();
}

void Spellbook::nextPage() {
	// don't go to a page with no spells on it
	size_t numSlots = spellSlot.size();
	size_t numSpells = inscribedSpells.size();
	if (numSlots * (curPage + 1) >= numSpells) {
		return;
	}
	++curPage;
	refreshPage();
}

void Spellbook::previousPage() {
	// don't go further than first page
	if (curPage == 0) {
		return;
	}
	--curPage;
	refreshPage();
}

void Spellbook::clear() {
	inscribedSpells.clear();
	refreshPage();
}

void Spellbook::refreshPage() {
	unsigned int spellsOnPreviousPages = curPage * static_cast<unsigned int>(spellSlot.size());
	
	m_floatingSpell = NULL;

	for (unsigned int curSlot = 0; curSlot<spellSlot.size(); ++curSlot) {
		// reset slot to empty
		spellSlot[curSlot].action = NULL;
		if (spellSlot[curSlot].tooltip != NULL) {
			delete spellSlot[curSlot].tooltip;
		}
		spellSlot[curSlot].tooltip = NULL;
		// fill slot with spells from this page
		unsigned int curSpellNr = spellsOnPreviousPages + curSlot;
		if (curSpellNr < inscribedSpells.size()) {
			spellSlot[curSlot].action = inscribedSpells[curSpellNr];
			spellSlot[curSlot].tooltip = new SpellTooltip(inscribedSpells[curSpellNr], m_player);
		}
	}
}