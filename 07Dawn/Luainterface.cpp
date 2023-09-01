#include "Luainterface.h"

#include "TextureManager.h"
#include "TilesetManager.h"
#include "Zone.h"
#include "Npc.h"
#include "InteractionRegion.h"
#include "InteractionPoint.h"
#include "TextWindow.h"
#include "Actions.h"
#include "Spells.h"
#include "Player.h"
#include "Shop.h"
#include "Interface.h"
#include "Quest.h"
#include "Utils.h"

namespace EditorInterface{
	
	void addGroundTile(int posX, int posY, int tile){	
		
		Zone *currentZone = ZoneManager::Get().getCurrentZone();
		const TileSet& tileSet = EditorInterface::getTileSet(Enums::TileClassificationType::FLOOR);
		currentZone->getTileMap().push_back({ posX, posY, tileSet.getTile(tile + tileSet.getOffset())  });
	}

	void addEnvironment(int posX, int posY, int posZ, int tile){
		Zone *currentZone = ZoneManager::Get().getCurrentZone();
		const TileSet& tileSet = EditorInterface::getTileSet(Enums::TileClassificationType::ENVIRONMENT);
		currentZone->getEnvironmentMap().push_back({ posX, posY, posZ, tileSet.getTile(tile + tileSet.getOffset()), 1.0f, 1.0f, 1.0f, 1.0f, static_cast<float>(tileSet.getTile(tile + tileSet.getOffset()).textureRect.width), static_cast<float>(tileSet.getTile(tile + tileSet.getOffset()).textureRect.height) });
	}

	void adjustLastRGBA(float red, float green, float blue, float alpha){
		Zone *currentZone = ZoneManager::Get().getCurrentZone();
		EnvironmentMap &lastEnv = currentZone->getEnvironmentMap()[currentZone->getEnvironmentMap().size() - 1];
		lastEnv.red = red;
		lastEnv.green = green;
		lastEnv.blue = blue;
		lastEnv.transparency = alpha;
	}

	void adjustLastSize(float width, float height){
		Zone *currentZone = ZoneManager::Get().getCurrentZone();
		EnvironmentMap &lastEnv = currentZone->getEnvironmentMap()[currentZone->getEnvironmentMap().size() - 1];
		lastEnv.width = width;
		lastEnv.height = height;
	}

	void addCollisionRect(int lrx, int lry, int width, int height){
		Zone *currentZone = ZoneManager::Get().getCurrentZone();
		currentZone->getCollisionMap().push_back({ lrx, lry, width, height });
	}

	TileSet& getTileSet(Enums::TileClassificationType tileType) {
		return TileSetManager::Get().getTileSet(tileType);
	}
}
namespace DawnInterface{

	TextureRect& loadimage(std::string file) {
		return TextureManager::Loadimage(file);
	}

	std::string getSpellbookSaveText(){
		return Spellbook::Get().getLuaSaveText();
	}

	std::string getActionbarSaveText(){
		return Interface::Get().getActionBarLuaSaveText();
	}

	void restoreActionBar(int buttonNr, SpellActionBase *action) {
		Interface::Get().bindActionToButtonNr(buttonNr, action);	
	}

	
	std::string getReenterCurrentZoneText(){
		std::ostringstream oss;
		oss << "DawnInterface.enterZone( \"" << ZoneManager::Get().getCurrentZone()->getZoneName() << "\", " << Player::Get().getXPos() << ", " << Player::Get().getYPos() << " );" << std::endl;
		return oss.str();
	}

	void enterZone(std::string zoneName, int enterX, int enterY){
		Zone& newZone = ZoneManager::Get().getZone(zoneName);
		newZone.loadZone();
		Player::Get().setPosition(enterX, enterY);
		std::ostringstream oss;
		std::string zoneNameNoPrefix = zoneName;
		if (zoneNameNoPrefix.find_last_of('/') != std::string::npos){
			zoneNameNoPrefix = zoneNameNoPrefix.substr(zoneNameNoPrefix.find_last_of('/') + 1);
		}

		oss << "if (" << "Zones." << zoneNameNoPrefix << ".onEnterMap ~= nil)\nthen\n    "<< "Zones." << zoneNameNoPrefix << ".onEnterMap(" << enterX << "," << enterY << ");\nelse    print \"" << "Zones." << zoneNameNoPrefix << ".onEnterMap was not defined\";\nend";
		std::string onEnterCall = oss.str();
		LuaFunctions::executeLuaScript(onEnterCall);
	}

	void enterZone() {
		Zone* zone = ZoneManager::Get().getCurrentZone();
		zone->loadZone();
		std::ostringstream oss;
		std::string zoneNameNoPrefix = zone->getZoneName();
		if (zoneNameNoPrefix.find_last_of('/') != std::string::npos) {
			zoneNameNoPrefix = zoneNameNoPrefix.substr(zoneNameNoPrefix.find_last_of('/') + 1);
		}
		oss << "if (" << "Zones." << zoneNameNoPrefix << ".onEnterMap ~= nil)\nthen\n    " << "Zones." << zoneNameNoPrefix << ".onEnterMap(" << Player::Get().getXPos() << "," << Player::Get().getYPos() << ");\nelse    print \"" << "Zones." << zoneNameNoPrefix << ".onEnterMap was not defined\";\nend";
		std::string onEnterCall = oss.str();
		LuaFunctions::executeLuaScript(onEnterCall);
	}

	void setCurrentZone(std::string zoneName){

		Zone& newCurZone = ZoneManager::Get().getZone(zoneName);
		ZoneManager::Get().setCurrentZone(&newCurZone);
	}

	const CharacterType& createNewMobType(std::string characterType) {
		return CharacterTypeManager::Get().getCharacterType(characterType);
	}

	Npc* addMobSpawnPoint(std::string characterType, std::string name, int x_pos, int y_pos, int respawn_rate, int do_respawn, Enums::Attitude attitude) {
		
		if (!CharacterTypeManager::Get().containsCaracterType(characterType)) {
			return nullptr;
		}

		Npc* newMob = new Npc();	
		newMob->setCharacterType(characterType);
		newMob->setSpawnInfo(x_pos, y_pos, respawn_rate, do_respawn);
		newMob->setAttitude(attitude);
		newMob->setName(name);
		ZoneManager::Get().getCurrentZone()->addNPC(newMob);
		return ZoneManager::Get().getCurrentZone()->getNPCs().back();
	}

	void removeMobSpawnPoint(Npc* mobSpawnPoint) {
		ZoneManager::Get().getCurrentZone()->removeNPC(mobSpawnPoint);
	}

	const Zone& getCurrentZone() {
		return *ZoneManager::Get().getCurrentZone();
	}

	const InteractionRegion& addInteractionRegion(){
		InteractionRegion::AddInteractionRegion(new InteractionRegion());
		return *InteractionRegion::GetInteractionRegions().back();
	}

	void removeInteractionRegion(InteractionRegion *regionToRemove){
		regionToRemove->markAsDeletable();
	}

	const InteractionPoint& addInteractionPoint(){
		InteractionPoint::AddInteractionPoint(new InteractionPoint());
		return *InteractionPoint::GetInteractionPoints().back();
	}

	const InteractionPoint& addCharacterInteractionPoint(Character *character){
		CharacterInteractionPoint::AddInteractionPoint(new CharacterInteractionPoint(character));
		return *CharacterInteractionPoint::GetInteractionPoints().back();
	}

	void removeInteractionPoint(InteractionPoint *pointToRemove){
		pointToRemove->markAsDeletable();
	}

	std::string getItemReferenceRestore(std::string varName, void *ignore) {
		return "";
	}

	std::string getItemReferenceRestore(std::string varName, InteractionPoint* interactionPoint) {
		if (interactionPoint == NULL) {
			return "nil;";
		}

		std::string zoneName = "";
		std::unordered_map<std::string, Zone>& allZones = ZoneManager::Get().getAllZones();
		for (std::unordered_map<std::string, Zone>::iterator it = allZones.begin(); it != allZones.end(); ++it) {
			std::vector<InteractionPoint*>& interactionPoints = (it->second).getInteractionPoints();
			if (std::find(interactionPoints.begin(), interactionPoints.end(), interactionPoint) != interactionPoints.end()) {
				zoneName = (it->first);
				break;
			}
		}
		
		std::ostringstream oss;
		oss << "DawnInterface.setCurrentZone(\"" << zoneName << "\")" << std::endl;	
		if (dynamic_cast<CharacterInteractionPoint*>(interactionPoint) != nullptr) {
			
			oss << varName << "=DawnInterface.addCharacterInteractionPoint(" << LuaFunctions::getSpawnPointName("SpawnPoints", dynamic_cast<Npc*>(dynamic_cast<CharacterInteractionPoint*>(interactionPoint)->getCharacter())) << ");" << std::endl;
			oss << varName << ":setInteractionType(" << interactionPoint->toStringForLua(interactionPoint->getInteractionType()) << ")" << std::endl;
			oss << varName << ":setInteractionCode( \"" << Utils::replaceEscape(interactionPoint->getInteractionCode()) << "\" );" << std::endl;
			return oss.str();

		} else {

			oss << varName << "=DawnInterface.addInteractionPoint();" << std::endl;
			oss << varName << ":setPosition(" << interactionPoint->getPosX() << "," << interactionPoint->getPosY() << "," << interactionPoint->getWidth() << "," << interactionPoint->getHeight() << ")" << std::endl;

			if (!interactionPoint->getBackgroundTextureName().empty())
				oss << varName << ":setBackgroundTexture(\"" << interactionPoint->getBackgroundTextureName() << "\"," << std::boolalpha << interactionPoint->getBackgroundTextureTransparent() << ")" << std::endl;
			oss << varName << ":setInteractionType(" << interactionPoint->toStringForLua(interactionPoint->getInteractionType()) << ")" << std::endl;
			oss << varName << ":setInteractionCode(\"" << Utils::replaceEscape(interactionPoint->getInteractionCode()) << "\");" << std::endl;
			return oss.str();
		}
	}

	std::string getItemReferenceRestore(std::string varName, InteractionRegion *interactionRegion) {
		if (interactionRegion == NULL) {
			return "nil;";
		}

		std::string zoneName = "";
		std::unordered_map<std::string, Zone>& allZones = ZoneManager::Get().getAllZones();
		for (std::unordered_map<std::string, Zone>::iterator it = allZones.begin(); it != allZones.end(); ++it) {
			std::vector<InteractionRegion*>& interactionRegions = (it->second).getInteractionRegions();
			if (std::find(interactionRegions.begin(), interactionRegions.end(), interactionRegion) != interactionRegions.end()) {
				zoneName = (it->first);
				break;
			}
		}
		
			
		std::ostringstream oss;
		oss << "DawnInterface.setCurrentZone(\"" << zoneName << "\")" << std::endl;
		oss << varName << "=DawnInterface.addInteractionRegion();" << std::endl;
		oss << varName << ":setPosition(" << interactionRegion->getLeft() << "," << interactionRegion->getBottom() << "," << interactionRegion->getWidth() << "," << interactionRegion->getHeight() << ")" << std::endl;
		if (!interactionRegion->getOnEnterText().empty())
			oss << varName << ":setOnEnterText(\"" << Utils::replaceEscape(interactionRegion->getOnEnterText()) << "\");" << std::endl;

		if (!interactionRegion->getOnLeaveText().empty())
			oss << varName << ":setOnLeaveText(\"" << Utils::replaceEscape(interactionRegion->getOnLeaveText()) << "\");" << std::endl;

		return oss.str();
	}

	std::string getItemReferenceRestore(std::string varName, CallIndirection *eventHandler) {
		if (eventHandler == NULL) {
			return "nil;";
		}

		std::string zoneName = "";
		std::unordered_map<std::string, Zone>& allZones = ZoneManager::Get().getAllZones();
		for (std::unordered_map<std::string, Zone>::iterator it = allZones.begin(); it != allZones.end(); ++it) {
			std::vector<CallIndirection*>& eventHandlers = (it->second).getEventHandlers();

			if (std::find(eventHandlers.begin(), eventHandlers.end(), eventHandler) != eventHandlers.end()) {
				zoneName = (it->first);
				break;
			}
		}

		std::ostringstream oss;
		oss << "DawnInterface.setCurrentZone(\"" << zoneName << "\")" << std::endl;		
		oss << varName << "=DawnInterface.createEventHandler();" << std::endl;
		oss << varName << ":setExecuteText(\"" << Utils::replaceEscape(eventHandler->getExecuteText()) <<"\");" << std::endl;
		return oss.str();
	}

	std::string getItemReferenceRestore(std::string varName, Npc* npc) {
		if (npc == NULL) {
			return "";
		}

		std::string zoneName = "";
		std::unordered_map<std::string, Zone>& allZones = ZoneManager::Get().getAllZones();
		for (std::unordered_map<std::string, Zone>::iterator it = allZones.begin(); it != allZones.end(); ++it) {
			std::vector<Npc*>& npcs = (it->second).getNPCs();
			if (std::find(npcs.begin(), npcs.end(), npc) != npcs.end()) {
				zoneName = (it->first);
				break;
			}
		}

		std::ostringstream oss;
		oss << "DawnInterface.setCurrentZone(\"" << zoneName << "\")" << std::endl;
		oss << varName << "=DawnInterface.addMobSpawnPoint(\"" << npc->getCharacterTypeStr() << "\",\""  << npc->getName() << "\"," << npc->getSpawnPosX() << "," << npc->getSpawnPosY() << "," << npc->getSecondsToRespawn() << "," << npc->getDoRespawn() << "," << npc->getAttitudeStr() <<  ");" << std::endl;
		oss << varName << ":setWanderRadius(" << npc->getWanderRadius() << ");" << std::endl;
		for (size_t curOnDieHandlerNr = 0; curOnDieHandlerNr < npc->getOnDieEventHandlers().size(); ++curOnDieHandlerNr) {
			oss << varName << ":addOnDieEventHandler(" << LuaFunctions::getEventHandlerName("Eventhandlers", npc->getOnDieEventHandlers()[curOnDieHandlerNr]) << ");" << std::endl;
		}
		return oss.str();
	}

	std::string getItemReferenceRestore(std::string varName, Quest* quest) {
		if (quest == NULL) {
			return "nil;";
		}

		std::ostringstream oss;
		oss << varName << "=DawnInterface.addQuest( \"" << quest->getName() << "\",\"" << quest->getDescription() << "\" );" << std::endl;

		if (quest->getExperienceReward() > 0) 
			oss << varName << ":setExperienceReward( " << static_cast<int>(quest->getExperienceReward()) << " );" << std::endl;
		
		if (quest->getCoinReward() > 0) 
			oss << varName << ":setCoinReward( " << static_cast<int>(quest->getCoinReward()) << " );" << std::endl;
		
		if (quest->getItemReward() != NULL)
			oss << varName << ":setItemReward( itemDatabase[\"" << quest->getItemReward()->getID() << "\"] );" << std::endl;
		
		std::vector<std::pair< Item*, int8_t>>& requiredItems = quest->getRequiredItems();
		for (size_t curReqItemNr = 0; curReqItemNr < requiredItems.size(); ++curReqItemNr) {
			const std::pair< Item*, uint8_t > &curReqItem = requiredItems[curReqItemNr];
			oss << varName << ":addRequiredItemForCompletion( itemDatabase[\"" << curReqItem.first->getID() << "\"], " << static_cast<int>(curReqItem.second) << " );" << std::endl;
		}
		oss << std::endl;
		return oss.str();
	}

	std::string getItemReferenceRestore(std::string varName, Shop *shop) {
		std::ostringstream oss;
		oss << varName << "=DawnInterface.addShop( \"" << shop->m_name << "\" , true );" << std::endl;
		for (size_t curTab = 0; curTab < 3; ++curTab) {
			for (size_t curItemNr = 0; curItemNr < shop->shopkeeperInventory[curTab].size(); ++curItemNr) {
				oss << varName << ":addItem( itemDatabase[\"" << shop->shopkeeperInventory[curTab][curItemNr]->getItem()->getID() << "\"] );" << std::endl;
			}
		}

		oss << varName << ":loadShopkeeperInventory();" << std::endl;
		return oss.str();
	}

	std::string storeGroundloot() {

		std::unordered_map<std::string, Zone>& allZones = ZoneManager::Get().getAllZones();
		std::ostringstream oss;
		for (std::unordered_map<std::string, Zone>::iterator it = allZones.begin(); it != allZones.end(); ++it) {
			std::string zoneNameNoPrefix = it->first;
			if (zoneNameNoPrefix.find_last_of('/') != std::string::npos) {
				zoneNameNoPrefix = zoneNameNoPrefix.substr(zoneNameNoPrefix.find_last_of('/') + 1);
			}
			oss << "-- " << zoneNameNoPrefix << " ground loot" << std::endl;
			if((it->second).getGroundLoot().getGroundItems().size() == 0)
				return oss.str();

			oss << "DawnInterface.setCurrentZone( \"" << it->first << "\" );" << std::endl;
			for (size_t curGroundItemNr = 0; curGroundItemNr < (it->second).getGroundLoot().getGroundItems().size(); ++curGroundItemNr) {
				sGroundItems curGroundItem = (it->second).getGroundLoot().getGroundItems()[curGroundItemNr];
				Item *item = curGroundItem.item;
				if (dynamic_cast<GoldHeap*>(item) != NULL) {
					GoldHeap *goldHeap = dynamic_cast<GoldHeap*>(item);
					oss << "DawnInterface.restoreGroundGold( "
						<< goldHeap->numCoins() << ", "
						<< curGroundItem.xpos << ", "
						<< curGroundItem.ypos << " );" << std::endl;
				}
				else {
					oss << "DawnInterface.restoreGroundLootItem( "
						<< "itemDatabase[ \"" << item->getID() << "\" ], "
						<< curGroundItem.xpos << ", "
						<< curGroundItem.ypos << " );" << std::endl << std::endl;
				}
			}
		}
		return oss.str();
	}

	void restoreGroundLootItem(Item *item, int xPos, int yPos) {
		ZoneManager::Get().getCurrentZone()->getGroundLoot().addItem(xPos, yPos, item);
	}

	void restoreGroundGold(int amount, int xPos, int yPos){
		ZoneManager::Get().getCurrentZone()->getGroundLoot().addItem(xPos, yPos, new GoldHeap(amount));
	}

	Character* restoreCharacterReference(std::string zoneName, int posInArray) {
		Zone *correctZone = &ZoneManager::Get().getZone(zoneName);
		return correctZone->getCharacterPointer(posInArray);
	}

	InteractionPoint* restoreInteractionPointReference(std::string zoneName, int posInArray) {
		Zone *correctZone = &ZoneManager::Get().getZone(zoneName);
		return correctZone->getInteractionPointPointer(posInArray);
	}

	InteractionRegion* restoreInteractionRegionReference(std::string zoneName, int posInArray) {
		Zone *correctZone = &ZoneManager::Get().getZone(zoneName);
		return correctZone->getInteractionRegionPointer(posInArray);
	}


	CallIndirection* restoreEventHandlerReference(std::string zoneName, int posInArray) {
		Zone *correctZone = &ZoneManager::Get().getZone(zoneName);
		return correctZone->getEventHandlerPointer(posInArray);
	}

	TextWindow* createTextWindow() {
		TextWindow::AddTextWindow(new TextWindow());
		return TextWindow::GetTextWindows().back();
	}

	GeneralRayDamageSpell* createGeneralRayDamageSpell() {
		std::auto_ptr<GeneralRayDamageSpell> newSpell(dynamic_cast<GeneralRayDamageSpell*>(SpellCreation::getGeneralRayDamageSpell()));
		return newSpell.release();
	}

	GeneralAreaDamageSpell* createGeneralAreaDamageSpell() {
		std::auto_ptr<GeneralAreaDamageSpell> newSpell(dynamic_cast<GeneralAreaDamageSpell*>(SpellCreation::getGeneralAreaDamageSpell()));
		return newSpell.release();
	}

	GeneralBoltDamageSpell* createGeneralBoltDamageSpell() {
		std::auto_ptr<GeneralBoltDamageSpell> newSpell(dynamic_cast<GeneralBoltDamageSpell*>(SpellCreation::getGeneralBoltDamageSpell()));
		return newSpell.release();
	}

	GeneralHealingSpell* createGeneralHealingSpell() {
		std::auto_ptr<GeneralHealingSpell> newSpell(dynamic_cast<GeneralHealingSpell*>(SpellCreation::getGeneralHealingSpell()));
		return newSpell.release();
	}

	GeneralBuffSpell* createGeneralBuffSpell() {
		std::auto_ptr<GeneralBuffSpell> newSpell(dynamic_cast<GeneralBuffSpell*>(SpellCreation::getGeneralBuffSpell()));
		return newSpell.release();
	}

	MeleeDamageAction* createMeleeDamageAction() {
		std::auto_ptr<MeleeDamageAction> newAction(dynamic_cast<MeleeDamageAction*>(SpellCreation::getMeleeDamageAction()));
		return newAction.release();
	}

	RangedDamageAction* createRangedDamageAction() {
		std::auto_ptr<RangedDamageAction> newAction(dynamic_cast<RangedDamageAction*>(SpellCreation::getRangedDamageAction()));
		return newAction.release();
	}

	GeneralRayDamageSpell* copySpell(GeneralRayDamageSpell *other) {
		std::auto_ptr<GeneralRayDamageSpell> newSpell(dynamic_cast<GeneralRayDamageSpell*>(SpellCreation::getGeneralRayDamageSpell(other)));
		newSpell->unsetLuaID();
		return newSpell.release();
	}

	GeneralAreaDamageSpell* copySpell(GeneralAreaDamageSpell *other) {
		std::auto_ptr<GeneralAreaDamageSpell> newSpell(dynamic_cast<GeneralAreaDamageSpell*>(SpellCreation::getGeneralAreaDamageSpell(other)));
		newSpell->unsetLuaID();
		return newSpell.release();
	}

	GeneralBoltDamageSpell* copySpell(GeneralBoltDamageSpell *other) {
		std::auto_ptr<GeneralBoltDamageSpell> newSpell(dynamic_cast<GeneralBoltDamageSpell*>(SpellCreation::getGeneralBoltDamageSpell(other)));
		newSpell->unsetLuaID();
		return newSpell.release();
	}

	GeneralHealingSpell* copySpell(GeneralHealingSpell *other) {
		std::auto_ptr<GeneralHealingSpell> newSpell(dynamic_cast<GeneralHealingSpell*>(SpellCreation::getGeneralHealingSpell(other)));
		newSpell->unsetLuaID();
		return newSpell.release();
	}

	GeneralBuffSpell* copySpell(GeneralBuffSpell *other) {
		std::auto_ptr<GeneralBuffSpell> newSpell(dynamic_cast<GeneralBuffSpell*>(SpellCreation::getGeneralBuffSpell(other)));
		newSpell->unsetLuaID();
		return newSpell.release();
	}

	MeleeDamageAction* copySpell(MeleeDamageAction *other) {
		std::auto_ptr<MeleeDamageAction> newSpell(dynamic_cast<MeleeDamageAction*>(SpellCreation::getMeleeDamageAction(other)));
		newSpell->unsetLuaID();
		return newSpell.release();
	}

	RangedDamageAction* copySpell(RangedDamageAction *other) {
		std::auto_ptr<RangedDamageAction> newSpell(dynamic_cast<RangedDamageAction*>(SpellCreation::getRangedDamageAction(other)));
		newSpell->unsetLuaID();
		return newSpell.release();
	}

	void inscribeSpellInPlayerSpellbook(SpellActionBase* inscribedSpell) {
		Player::Get().inscribeSpellInSpellbook(inscribedSpell);
	}

	void reloadSpellsFromPlayer() {
		Spellbook::Get().reloadSpellsFromPlayer();
	}

	void addTextToLogWindow(float color[], const char *text, ...){
		char buffer[1024];
		std::va_list args;

		// writing our text and arguments to the buffer
		va_start(args, text);
		vsnprintf(buffer, 1024, text, args);
		va_end(args);

		// push everything to our vector.
		Interface::Get().addTextToLog(std::string(buffer), Vector4f(color[0], color[1], color[2], 1.0f));
	}

	void clearLogWindow() {
		Interface::Get().clearLogWindow();
	}

	std::string getInventorySaveText() {
		return  Player::Get().getInventory().getReloadText();
	}

	void restoreItemInBackpack(Item* item, int inventoryPosX, int inventoryPosY, size_t stackSize) {
		InventoryItem* invItem = new InventoryItem(item, inventoryPosX, inventoryPosY);
		invItem->setCurrentStackSize(stackSize);
		Player::Get().getInventory().insertItemWithExchangeAt(invItem, inventoryPosX, inventoryPosY);
	}

	void restoreWieldItem(int slot, Item* item) {
		Enums::ItemSlot slotToUse = static_cast<Enums::ItemSlot>(slot);
		InventoryItem* invItem = new InventoryItem(item, 0, 0);
		Player::Get().getInventory().wieldItemAtSlot(slotToUse, invItem);
	}

	void giveItemToPlayer(Item* item) {
		Inventory& playerInventory = Player::Get().getInventory();
		bool wasInserted = playerInventory.insertItem(item);
		if (!wasInserted) {
			Player* player = &Player::Get();
			ZoneManager::Get().getCurrentZone()->getGroundLoot().addItem(player->getXPos(), player->getYPos(), item);
		} else {
			GLfloat blue[] = { 0.4f, 0.4f, 0.8f };
			DawnInterface::addTextToLogWindow(blue, "You receive %s.", item->getName().c_str());
		}
	}

	Item* createNewItem(std::string name,
		int sizeX,
		int sizeY,
		std::string symbolFile,
		Enums::ItemQuality itemQuality,
		EquipPosition::EquipPosition equipPos,
		Enums::ItemType itemType,
		Enums::ArmorType armorType,
		Enums::WeaponType weaponType) {

		Item *newItem = new Item(name, sizeX, sizeY, symbolFile, itemQuality, equipPos, itemType, armorType, weaponType);
		//allItems.push_back(newItem);
		return newItem;
	}

	Player& getPlayer() {
		return Player::Get();
	}

	const Shop& addShop(std::string name, bool replace){
		return ShopManager::Get().getShop(name, replace);
	}

	Quest* addQuest(std::string questName, std::string questDescription) {
		Quest* newQuest = new Quest(questName, questDescription);
		QuestCanvas::Get().addQuest(newQuest);
		return QuestCanvas::Get().getQuests().back();
	}

	LuaCallIndirection* createEventHandler(){
		LuaCallIndirection *newEventHandler = new LuaCallIndirection();

		ZoneManager::Get().getCurrentZone()->addEventHandler(newEventHandler);
		return newEventHandler;
	}

	bool isSavingAllowed() {
		return Globals::savingAllowed;
	}

	void setSavingAllowed(bool savingAllowed) {
		Globals::savingAllowed = savingAllowed;
	}
}