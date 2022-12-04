#include <memory>

#include "engine/Fontrenderer.h"

#include "Quest.h"
#include "Widget.h"
#include "TextureManager.h"
#include "TextWindow.h"
#include "Zone.h"
#include "Utils.h"

QuestCanvas QuestCanvas::s_instance;

QuestCanvas& QuestCanvas::Get() {
	return s_instance;
}

QuestCanvas::QuestCanvas() : Widget(20, 100, 375, 376, 20, 20) {
	selectedQuestNr = -1;	
}

QuestCanvas::~QuestCanvas() {
	
}

void QuestCanvas::init() {

	font = &Globals::fontManager.get("verdana_14");
	TextureAtlasCreator::get().init("quest", 1024, 1024);
	TextureManager::Loadimage("res/interface/QuestScreen/questscreen.tga", 0, m_textures);
	m_textureAtlas = TextureAtlasCreator::get().getAtlas();

	addMoveableFrame(375, 22, 20, 374);
	addCloseButton(22, 22, 373, 376);
}

void QuestCanvas::draw() {
	if (!m_visible) return;
	TextureManager::BindTexture(m_textureAtlas, true, 0);
	TextureManager::DrawTexture(m_textures[0], m_posX,  m_posY, false, false);

	int textX = m_posX + 64;
	int textY = m_posY + m_height - 24 - font->lineHeight;

	Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Fontrenderer::Get().bindTexture(Globals::fontManager.get("verdana_14"));
	for (size_t curQuestNameNr = 0; curQuestNameNr<quests.size(); ++curQuestNameNr) {
		if (static_cast<int>(curQuestNameNr) == selectedQuestNr) {
			// draw selected text in yellow
			color = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
		}
		Fontrenderer::Get().addText(*font, textX, textY, quests[curQuestNameNr]->getName(), color, false);
		

		if (static_cast<int>(curQuestNameNr) == selectedQuestNr){
			// reset color after drawing selected text
			color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
		textY -= font->lineHeight * 1.5;
	}

	int topOfDescriptionBox = 192;
	textY = m_posY + topOfDescriptionBox - font->lineHeight;

	if (selectedQuestNr >= 0 && static_cast<size_t>(selectedQuestNr) < questDescriptions.size()) {
		// draw description of currently selected quest
		std::vector<std::string> selectedQuestDescription = questDescriptions[selectedQuestNr];
		for (size_t curLineNr = 0; curLineNr<selectedQuestDescription.size(); ++curLineNr) {
			std::string curLine = selectedQuestDescription[curLineNr];

			Fontrenderer::Get().addText(*font, textX, textY, curLine, color, false);
			textY -= font->lineHeight * 1.5;
		}
	}
	Fontrenderer::Get().drawBuffer(false);
}

bool QuestCanvas::anyQuestNeedThis(Item *item) const {
	for (size_t curQuest = 0; curQuest < quests.size(); curQuest++) {
		if (quests[curQuest]->isItemRequiredInQuest(item) == true) {
			return true;
		}
	}
	return false;
}

void QuestCanvas::addQuest(Quest* quest) {
	quests.push_back(quest);
	size_t newQuestNr = questDescriptions.size();
	questDescriptions.push_back(std::vector<std::string>());
	TextWindow::FormatMultilineText(quest->getDescription(), questDescriptions[newQuestNr], m_width - 88, font);

	GLfloat green[] = { 0.15f, 1.0f, 0.15f };
	DawnInterface::addTextToLogWindow(green, "Quest accepted: %s.", quest->getName().c_str());

	if (selectedQuestNr == -1 && quests.size() > 0){
		selectedQuestNr = 0;
	}
}

void QuestCanvas::tryToPurgeQuests() {
	for (size_t curQuest = 0; curQuest < questsToBeRemoved.size(); curQuest++) {
		removeQuest(questsToBeRemoved[curQuest]);
	}
}

void QuestCanvas::addQuestToBeRemoved(Quest* quest) {
	questsToBeRemoved.push_back(quest);
}

void QuestCanvas::removeQuest(Quest* quest) {

	for (size_t foundQuestNr = 0; foundQuestNr<quests.size(); ++foundQuestNr) {

		if (quests[foundQuestNr] == quest) {

			quests.erase(quests.begin() + foundQuestNr);
			questDescriptions.erase(questDescriptions.begin() + foundQuestNr);

			if (selectedQuestNr == static_cast<int>(foundQuestNr)) {
				selectedQuestNr = -1;
			}else if (selectedQuestNr > static_cast<int>(foundQuestNr)) {
				--selectedQuestNr;
			}

			GLfloat green[] = { 0.15f, 1.0f, 0.15f };
			DawnInterface::addTextToLogWindow(green, "Quest completed: %s.", quest->getName().c_str());
			delete quest;

			if (selectedQuestNr == -1 && quests.size() > 0) {
				selectedQuestNr = 0;
			}
		}
	}
}

void QuestCanvas::removeAllQuests() {
	for (size_t questIndex = 0; questIndex < quests.size(); questIndex++) {
		delete quests[questIndex];
	}

	quests.clear();
	questDescriptions.clear();
	selectedQuestNr = -1;
}

void QuestCanvas::changeQuestDescription(Quest *quest, std::string newDescription) {
	size_t foundQuestNr = 0;
	for (foundQuestNr = 0; foundQuestNr<quests.size(); ++foundQuestNr) {
		if (quests[foundQuestNr] == quest) {
			break;
		}
	}

	if (foundQuestNr < quests.size()) {
		// found the quest
		questDescriptions[foundQuestNr].clear();
		TextWindow::FormatMultilineText(newDescription, questDescriptions[foundQuestNr], m_width - 88, font);

		GLfloat green[] = { 0.15f, 1.0f, 0.15f };
		DawnInterface::addTextToLogWindow(green, "Quest updated: %s.", quest->getName().c_str());
	}
}

void QuestCanvas::processInput() {
	if (!m_visible) return;
	Widget::processInput();

	Mouse &mouse = Mouse::instance();
	if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {

		if (!isMouseOnFrame(mouse.xPosAbsolute(), mouse.yPosAbsolute())) {
			return;
		}
		size_t curEntryNr = (m_posY + m_height - 24 - ViewPort::get().getCursorPosRelY()) / (font->lineHeight * 1.5);
		if (curEntryNr < quests.size()) {
			selectedQuestNr = curEntryNr;
		}
	}
}

std::vector<Quest*>& QuestCanvas::getQuests() {
	return quests;
}

///////////////////////////////////////////////////////////////////////////////////////
Quest::Quest(std::string name, std::string description) : name(name),
	m_questCanvas(QuestCanvas::Get()),
	description(description),
	experienceReward(0),
	itemReward(NULL),
	coinReward(0),
	questFinished(false)
{
}

Quest::~Quest() {
}

void Quest::addRequiredItemForCompletion(Item* requiredItem, int quantity) {
	requiredItems.push_back(std::pair<Item*, int8_t>(requiredItem, quantity));
}

bool Quest::isItemRequiredInQuest(Item* item) const {
	for (size_t curItem = 0; curItem < requiredItems.size(); curItem++) {
		if (requiredItems[curItem].first == item) {
			return true;
		}
	}
	return false;
}

void Quest::setExperienceReward(uint16_t experienceReward)
{
	this->experienceReward = experienceReward;
}

uint16_t Quest::getExperienceReward() const {
	return experienceReward;
}

void Quest::setCoinReward(uint16_t coinReward) {
	this->coinReward = coinReward;
}

uint16_t Quest::getCoinReward() const {
	return coinReward;
}

void Quest::setItemReward(Item *itemReward) {
	this->itemReward = itemReward;
}

Item* Quest::getItemReward() const {
	return itemReward;
}

void Quest::setDescription(std::string description) {
	m_questCanvas.changeQuestDescription(this, description);
}

std::string Quest::getDescription() const
{
	return description;
}

std::string Quest::getName() const
{
	return name;
}

bool Quest::finishQuest() {
	// we will try to finish the quest here. Depending on what the quest require of us.
	Player* ourPlayer = &Player::Get();

	// make sure we have all items required in the quest, if not we return false and doesn't finish the quest.
	for (size_t requiredItemIndex = 0; requiredItemIndex < requiredItems.size(); requiredItemIndex++) {
		if (ourPlayer->getInventory()->doesItemExistInBackpack(requiredItems[requiredItemIndex].first, requiredItems[requiredItemIndex].second) == false) {
			return false;
		}
	}

	// reward the player with experience, items and coins.
	if (getExperienceReward() > 0) {
		ourPlayer->gainExperience(getExperienceReward());
	}

	if (getCoinReward() > 0) {
		ourPlayer->giveCoins(getCoinReward());
		GLfloat blue[] = { 0.4f, 0.4f, 0.8f };
		DawnInterface::addTextToLogWindow(blue, "You received %s.", currency::getLongTextString(getCoinReward()).c_str());
	}

	if (getItemReward() != NULL) {
		DawnInterface::giveItemToPlayer(getItemReward());
	}
	
	// remove all quest items from the game that was needed for this quest.
	for (size_t requiredItemIndex = 0; requiredItemIndex < requiredItems.size(); requiredItemIndex++) {
		ourPlayer->getInventory()->removeItem(requiredItems[requiredItemIndex].first);
		Zone* curZone = ZoneManager::Get().getCurrentZone();
		curZone->getGroundLoot()->removeItem(requiredItems[requiredItemIndex].first);
	}

	m_questCanvas.addQuestToBeRemoved(this);
	return true;
}