-- General status attributes
local thePlayer = DawnInterface.getPlayer();
thePlayer:setStrength( 15 );
thePlayer:setDexterity( 20 );
thePlayer:setVitality( 15 );
thePlayer:setIntellect( 10 );
thePlayer:setWisdom( 10 );
thePlayer:setMaxHealth( 400 );
thePlayer:setMaxMana( 250 );
thePlayer:setMaxFatigue( 100 );
thePlayer:setMinDamage( 0 );
thePlayer:setMaxDamage( 0 );
thePlayer:setDamageModifierPoints( 0 );
thePlayer:setHitModifierPoints( 0 );
thePlayer:setEvadeModifierPoints( 0 );
thePlayer:setParryModifierPoints( 0 );
thePlayer:setBlockModifierPoints( 0 );
thePlayer:setMeleeCriticalModifierPoints( 0 );
thePlayer:setResistAllModifierPoints( 0 );
thePlayer:setSpellEffectAllModifierPoints( 0 );
thePlayer:setResistElementModifierPoints( 0, 0 );
thePlayer:setSpellEffectElementModifierPoints( 0, 0 );
thePlayer:setResistElementModifierPoints( 1, 0 );
thePlayer:setSpellEffectElementModifierPoints( 1, 0 );
thePlayer:setResistElementModifierPoints( 2, 0 );
thePlayer:setSpellEffectElementModifierPoints( 2, 0 );
thePlayer:setResistElementModifierPoints( 3, 0 );
thePlayer:setSpellEffectElementModifierPoints( 3, 0 );
thePlayer:setResistElementModifierPoints( 4, 0 );
thePlayer:setSpellEffectElementModifierPoints( 4, 0 );
thePlayer:setResistElementModifierPoints( 5, 0 );
thePlayer:setSpellEffectElementModifierPoints( 5, 0 );
thePlayer:setSpellCriticalModifierPoints( 0 );
thePlayer:setName( "Enylyn" );
thePlayer:setLevel( 1 );
thePlayer:setExperience( 0 );
thePlayer:setClass( Enums.Liche );
thePlayer:setCharacterType("player_w" );
-- coins
thePlayer:setCoins( 576 );
-- position
thePlayer:setPosition( 850, 2247 );
thePlayer:init();

DawnInterface.setCurrentZone( "res/_lua/zone1" );
zone1=DawnInterface.getCurrentZone();
zone1.inited = nil;
zone1:setInit(false);
zone1:loadZone();

-- zone1 event handlers
local curEventHandler = DawnInterface.createEventHandler();
curEventHandler:setExecuteText( [[quest_playHideAndSeek.onKilledQuestMonster()]] );

-- zone1 ground loot
traderShop=DawnInterface.addShop( "shop1" , true );
traderShop:addItem( itemDatabase["bladeofstrength"] );
traderShop:addItem( itemDatabase["leynorscap"] );
traderShop:addItem( itemDatabase["smallhealingpotion"] );
traderShop:addItem( itemDatabase["smallhealingpotion"] );
traderShop:addItem( itemDatabase["smallhealingpotion"] );
traderShop:addItem( itemDatabase["smallhealingpotion"] );
traderShop:addItem( itemDatabase["smallhealingpotion"] );
traderShop:addItem( itemDatabase["smallhealingpotion"] );
traderShop:addItem( itemDatabase["smallhealingpotion"] );
traderShop:addItem( itemDatabase["smallhealingpotion"] );
traderShop:addItem( itemDatabase["smallhealingpotion"] );
traderShop:addItem( itemDatabase["smallhealingpotion"] );
traderShop:addItem( itemDatabase["bookofleatherskinrank2"] );
traderShop:loadShopkeeperInventory();

quest_hexmaster.inited=true
quest_hexmaster.questGiven=true
quest_hexmaster.quest=DawnInterface.addQuest( "The Hexmaster","Explore the catacombs underneath the town of Arinox and report back any trace of Jorni to Ornad Saidor." );
quest_hexmaster.isOpen=false
quest_venomousveins.inited=true
quest_playHideAndSeek.rewardGot=false
quest_playHideAndSeek.fulfilled=false
quest_playHideAndSeek.inited=true
quest_playHideAndSeek.added=true
quest_playHideAndSeek.quest=DawnInterface.addQuest( "Hide and Seek","My little brother James is somewhere in this forest. I fear he got lost. Please find him and tell him to come to me." );
-- Player's inventory
-- Items in Backpack
DawnInterface.restoreItemInBackpack( itemDatabase["bookofmagicmissilerank2"], 0, 0, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["moldytome"], 0, 2, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["daggerofflowingthought"], 2, 0, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["swordofkhazom"], 3, 0, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["eyeoflicor"], 2, 2, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["gnollshield"], 4, 0, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["gutteraxe"], 6, 0, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["snakeloop"], 2, 3, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["shadering"], 3, 3, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["scrolloftheboar"], 4, 2, 3 );
DawnInterface.restoreItemInBackpack( itemDatabase["smallhealingpotion"], 4, 3, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["tornleatherbelt"], 5, 3, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["coppernecklace"], 5, 2, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["fungalboots"], 8, 0, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["ajuicyapple"], 7, 3, 2 );
DawnInterface.restoreItemInBackpack( itemDatabase["awaterpouch"], 8, 2, 5 );
-- equipped Items
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "magicmissile" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "electrocute" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "inferno" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "lightningbolt" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "healing" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "forcedhealing" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "hymnofrestoration" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "callingofthegrave" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "manavortex" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "earthenseeds" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "melee" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "terrify" ] );
DawnInterface.reloadSpellsFromPlayer()
-- action bar
DawnInterface.restoreActionBar( 0, spellDatabase[ "magicmissile" ] );
DawnInterface.restoreActionBar( 1, spellDatabase[ "electrocute" ] );
DawnInterface.restoreActionBar( 2, spellDatabase[ "inferno" ] );
DawnInterface.restoreActionBar( 3, spellDatabase[ "lightningbolt" ] );
DawnInterface.restoreActionBar( 4, spellDatabase[ "healing" ] );
DawnInterface.restoreActionBar( 5, spellDatabase[ "forcedhealing" ] );
DawnInterface.restoreActionBar( 6, spellDatabase[ "hymnofrestoration" ] );
DawnInterface.restoreActionBar( 7, spellDatabase[ "callingofthegrave" ] );
DawnInterface.restoreActionBar( 8, spellDatabase[ "manavortex" ] );
DawnInterface.restoreActionBar( 9, spellDatabase[ "earthenseeds" ] );
DawnInterface.enterZone( "res/_lua/zone1", 850, 2247 );
DawnInterface.setSavingAllowed( true );
