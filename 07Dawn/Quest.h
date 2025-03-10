#pragma once
#include <string>
#include <vector>
#include "Widget.h"
#include "Player.h"

struct TextureRect;
struct CharacterSet;
class Quest;

class QuestCanvas : public Widget{

	friend class Quest;

public:

	QuestCanvas();
	~QuestCanvas();
	void draw() override;
	void processInput() override;
	void init(std::vector<TextureRect> textures);
	void addQuest(Quest *quest);
	std::vector<Quest*>& getQuests();
	void removeAllQuests();
	bool anyQuestNeedThis(Item *item) const;

	static QuestCanvas& Get();

private:

	void addQuestToBeRemoved(Quest *quest);
	void tryToPurgeQuests();
	void removeQuest(Quest *quest);
	
	void changeQuestDescription(Quest *quest, std::string newDescription);

	std::vector<Quest*> quests;
	std::vector<std::vector< std::string >> questDescriptions;
	std::vector<Quest*> questsToBeRemoved;
	int selectedQuestNr;
	std::vector<TextureRect> m_textures;
	CharacterSet *font;

	static QuestCanvas s_instance;
};

class Quest {

public:

	Quest(std::string name, std::string description);
	~Quest();

	bool finishQuest();
	bool isQuestFinished() const;

	void addRequiredItemForCompletion(Item* requiredItem, int quantity);
	bool isItemRequiredInQuest(Item* item) const;

	void setExperienceReward(uint16_t experienceReward);
	uint16_t getExperienceReward() const;

	void setCoinReward(uint16_t coinReward);
	uint16_t getCoinReward() const;

	void setItemReward(Item* itemReward);
	Item* getItemReward() const;

	void setDescription(std::string description);
	std::string getDescription() const;
	std::string getName() const;
	std::vector<std::pair< Item*, int8_t>>& getRequiredItems();

private:

	uint16_t experienceReward;
	uint16_t coinReward;
	Item* itemReward;

	bool questFinished;

	std::vector<std::pair< Item*, int8_t>> requiredItems;

	std::string name;
	std::string description;
	QuestCanvas& m_questCanvas;
};