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
thePlayer:setClass( Enums.Ranger );
thePlayer:setCharacterType("player_r" );
-- coins
thePlayer:setCoins( 576 );
-- position
thePlayer:setPosition( 764, 1354 );
thePlayer:init();

DawnInterface.setCurrentZone( "res/_lua/arinoxGeneralShop" );
arinoxGeneralShop=DawnInterface.getCurrentZone();
arinoxGeneralShop.inited = nil;
arinoxGeneralShop:setInit(false);
arinoxGeneralShop:loadZone();

-- arinoxGeneralShop event handlers

-- ground loot
DawnInterface.setCurrentZone( "res/_lua/zone1" );
zone1=DawnInterface.getCurrentZone();
zone1.inited = nil;
zone1:setInit(false);
zone1:loadZone();

-- zone1 event handlers
local curEventHandler = DawnInterface.createEventHandler();
curEventHandler:setExecuteText( [[quest_playHideAndSeek.onKilledQuestMonster()]] );

-- ground loot
DawnInterface.restoreGroundLootItem( itemDatabase[ "weakenedbow" ], 758, 1060 );
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
quest_hexmaster.isOpen=false
quest_venomousveins.inited=true
traderShop2=DawnInterface.addShop( "shop2" , true );
traderShop2:addItem( itemDatabase["bladeofstrength"] );
traderShop2:addItem( itemDatabase["leynorscap"] );
traderShop2:addItem( itemDatabase["smallhealingpotion"] );
traderShop2:addItem( itemDatabase["smallhealingpotion"] );
traderShop2:addItem( itemDatabase["smallhealingpotion"] );
traderShop2:addItem( itemDatabase["smallhealingpotion"] );
traderShop2:addItem( itemDatabase["smallhealingpotion"] );
traderShop2:addItem( itemDatabase["smallhealingpotion"] );
traderShop2:addItem( itemDatabase["smallhealingpotion"] );
traderShop2:addItem( itemDatabase["smallhealingpotion"] );
traderShop2:addItem( itemDatabase["smallhealingpotion"] );
traderShop2:addItem( itemDatabase["smallhealingpotion"] );
traderShop2:addItem( itemDatabase["bookofleatherskinrank2"] );
traderShop2:loadShopkeeperInventory();

quest_playHideAndSeek.rewardGot=false
quest_playHideAndSeek.fulfilled=false
quest_playHideAndSeek.inited=true
quest_playHideAndSeek.added=true
quest_playHideAndSeek.quest=DawnInterface.addQuest( "Hide and Seek","My little brother James is somewhere in this forest. I fear he got lost. Please find him and tell him to come to me." );
-- Player's inventory
-- Items in Backpack
DawnInterface.restoreItemInBackpack( itemDatabase["awaterpouch"], 7, 2, 5 );
DawnInterface.restoreItemInBackpack( itemDatabase["swordofkhazom"], 2, 0, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["eyeoflicor"], 0, 3, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["gnollshield"], 3, 0, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["gutteraxe"], 5, 0, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["snakeloop"], 1, 3, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["shadering"], 2, 3, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["scrolloftheboar"], 3, 2, 3 );
DawnInterface.restoreItemInBackpack( itemDatabase["smallhealingpotion"], 3, 3, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["tornleatherbelt"], 4, 3, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["coppernecklace"], 4, 2, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["fungalboots"], 7, 0, 1 );
DawnInterface.restoreItemInBackpack( itemDatabase["ajuicyapple"], 6, 3, 2 );
-- equipped Items
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "melee" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "leatherskin" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "shoot" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "frozenshot" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "pindown" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "flamingarrow" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "landyrsforce" ] );
DawnInterface.inscribeSpellInPlayerSpellbook( spellDatabase[ "voiceoftheforest" ] );
DawnInterface.reloadSpellsFromPlayer()
-- action bar
DawnInterface.restoreActionBar( 0, spellDatabase[ "leatherskin" ] );
DawnInterface.restoreActionBar( 1, spellDatabase[ "melee" ] );
DawnInterface.restoreActionBar( 2, spellDatabase[ "shoot" ] );
DawnInterface.restoreActionBar( 3, spellDatabase[ "frozenshot" ] );
DawnInterface.restoreActionBar( 4, spellDatabase[ "pindown" ] );
DawnInterface.restoreActionBar( 5, spellDatabase[ "flamingarrow" ] );
DawnInterface.restoreActionBar( 6, spellDatabase[ "landyrsforce" ] );
DawnInterface.restoreActionBar( 7, spellDatabase[ "voiceoftheforest" ] );
DawnInterface.enterZone( "res/_lua/zone1", 764, 1354 );
DawnInterface.setSavingAllowed( true );
