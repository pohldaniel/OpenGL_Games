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
	void init();
	void init(unsigned int textureAtlas, std::vector<TextureRect> textures);
	void addQuest(Quest *quest);
	std::vector<Quest*>& getQuests();

	static QuestCanvas& Get();

private:

	bool anyQuestNeedThis(Item *item) const;
	
	void addQuestToBeRemoved(Quest *quest);
	void tryToPurgeQuests();
	void removeQuest(Quest *quest);
	void removeAllQuests();
	void changeQuestDescription(Quest *quest, std::string newDescription);

	std::vector<Quest*> quests;
	/// \brief For each quest the description. Formatted for multiline output.
	std::vector< std::vector< std::string > > questDescriptions;
	std::vector<Quest*> questsToBeRemoved;
	int selectedQuestNr;
	std::vector<TextureRect> m_textures;
	unsigned int m_textureAtlas;
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

private:

	uint16_t experienceReward;
	uint16_t coinReward;
	Item* itemReward;

	bool questFinished;

	std::vector< std::pair< Item*, int8_t > > requiredItems;

	std::string name;
	std::string description;
	QuestCanvas& m_questCanvas;

	//friend std::string DawnInterface::getReinitialisationString(std::string fullVarName, Quest* quest);
};