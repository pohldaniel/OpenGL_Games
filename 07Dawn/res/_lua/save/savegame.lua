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
thePlayer:setExperience( 40 );
thePlayer:setClass( Enums.Liche );
thePlayer:setCharacterType("player_w" );
thePlayer:setCoins( 576 );
thePlayer:setPosition( 3310, 533 );
thePlayer:setCurrentHealth(84);
thePlayer:setCurrentMana(168);
thePlayer:setCurrentFatigue(100);
thePlayer:init();

Zones={}
Zones.zone1={}
Quests={}
Quests.quest_playHideAndSeek={}
Quests.quest_venomousveins={}
Quests.quest_hexmaster={}
Eventhandlers={}
Eventhandlers.quest_playHideAndSeek={}
Eventhandlers.quest_venomousveins={}
SpawnPoints={}
SpawnPoints.quest_playHideAndSeek={}
SpawnPoints.quest_venomousveins={}
SpawnPoints.quest_hexmaster={}
InteractionPoints={}
InteractionPoints.quest_playHideAndSeek={}
InteractionPoints.quest_venomousveins={}
InteractionPoints.zone1={}
InteractionPoints.quest_hexmaster={}
InteractionRegions={}
InteractionRegions.quest_playHideAndSeek={}
InteractionRegions.quest_venomousveins={}
InteractionRegions.zone1={}
InteractionRegions.quest_hexmaster={}

Zones.zone1.path="res/_lua/zone1.init.lua"
Zones.zone1.inited=true
Quests.quest_playHideAndSeek.rewardGot=false
Quests.quest_playHideAndSeek.path="res/_lua/quests_wood.lua"
Quests.quest_playHideAndSeek.fulfilled=false
Quests.quest_playHideAndSeek.added=true
Quests.quest_playHideAndSeek.inited=true
Quests.quest_venomousveins.path="res/_lua/quests_venomousveins.lua"
Quests.quest_venomousveins.inited=true
Quests.quest_hexmaster.inited=true
Quests.quest_hexmaster.isOpen=false
Quests.quest_hexmaster.path="res/_lua/quests_hexmaster.lua"
Quests.quest_playHideAndSeek.quest=DawnInterface.addQuest( "Hide and Seek","My little brother James is somewhere in this forest. I fear he got lost. Please find him and tell him to come to me." );
Quests.quest_playHideAndSeek.quest:setExperienceReward( 150 );
Quests.quest_playHideAndSeek.quest:setCoinReward( 821 );
Quests.quest_playHideAndSeek.quest:setItemReward( itemDatabase["ringofdefense"] );


dofile('res/_lua/zone1.init.lua')
dofile('res/_lua/quests_wood.lua')
dofile('res/_lua/quests_venomousveins.lua')
dofile('res/_lua/quests_hexmaster.lua')

DawnInterface.setCurrentZone("res/_lua/zone1");
SpawnPoints.quest_venomousveins.jemma=DawnInterface.addMobSpawnPoint("Witch","Jemma",-2162,-4580,1,0,Enums.FRIENDLY);

DawnInterface.setCurrentZone("res/_lua/zone1");
SpawnPoints.quest_venomousveins.effreyLongback=DawnInterface.addMobSpawnPoint("Human","Effrey Longback",-1510,500,1,0,Enums.FRIENDLY);

DawnInterface.setCurrentZone("res/_lua/zone1");
SpawnPoints.quest_hexmaster.ornadSaidor=DawnInterface.addMobSpawnPoint("Human","Ornad Saidor",900,2350,1,0,Enums.FRIENDLY);

DawnInterface.setCurrentZone("res/_lua/zone1");
SpawnPoints.quest_hexmaster.noviceLeader=DawnInterface.addMobSpawnPoint("Wizard","Novice Leader",0,2100,1,0,Enums.FRIENDLY);

DawnInterface.setCurrentZone("res/_lua/zone1")
InteractionPoints.quest_playHideAndSeek.john=DawnInterface.addInteractionPoint();
InteractionPoints.quest_playHideAndSeek.john:setPosition(820,270,64,64)
InteractionPoints.quest_playHideAndSeek.john:setBackgroundTexture("res/character/John.tga",false)
InteractionPoints.quest_playHideAndSeek.john:setInteractionType(Enums.Quest)
InteractionPoints.quest_playHideAndSeek.john:setInteractionCode("Quests.quest_playHideAndSeek.onActivateJohn()");

DawnInterface.setCurrentZone("res/_lua/zone1")
InteractionPoints.quest_playHideAndSeek.james=DawnInterface.addInteractionPoint();
InteractionPoints.quest_playHideAndSeek.james:setPosition(3783,640,64,64)
InteractionPoints.quest_playHideAndSeek.james:setBackgroundTexture("res/character/James.tga",false)
InteractionPoints.quest_playHideAndSeek.james:setInteractionType(Enums.Quest)
InteractionPoints.quest_playHideAndSeek.james:setInteractionCode("Quests.quest_playHideAndSeek.onActivateJames()");

DawnInterface.setCurrentZone("res/_lua/zone1")
InteractionPoints.quest_venomousveins.jemmaInteraction=DawnInterface.addCharacterInteractionPoint(SpawnPoints.quest_venomousveins.jemma);
InteractionPoints.quest_venomousveins.jemmaInteraction:setInteractionType(Enums.Quest)
InteractionPoints.quest_venomousveins.jemmaInteraction:setInteractionCode( "Quests.quest_venomousveins.speakWithJemma()" );

DawnInterface.setCurrentZone("res/_lua/zone1")
InteractionPoints.quest_venomousveins.effreyLongbackInteraction=DawnInterface.addCharacterInteractionPoint(SpawnPoints.quest_venomousveins.effreyLongback);
InteractionPoints.quest_venomousveins.effreyLongbackInteraction:setInteractionType(Enums.Quest)
InteractionPoints.quest_venomousveins.effreyLongbackInteraction:setInteractionCode( "Quests.quest_venomousveins.speakWithEffrey()" );

DawnInterface.setCurrentZone("res/_lua/zone1")
InteractionPoints.zone1.leavePoint2=DawnInterface.addInteractionPoint();
InteractionPoints.zone1.leavePoint2:setPosition(822,2570,128,128)
InteractionPoints.zone1.leavePoint2:setBackgroundTexture("res/village/stairs.tga",false)
InteractionPoints.zone1.leavePoint2:setInteractionType(Enums.Zone)
InteractionPoints.zone1.leavePoint2:setInteractionCode("DawnInterface.enterZone( 'res/_lua/arinoxDungeonLevel1', 2020, 1880 );\nDawnInterface.setSavingAllowed( false );");

DawnInterface.setCurrentZone("res/_lua/zone1")
InteractionPoints.zone1.leavePoint3=DawnInterface.addInteractionPoint();
InteractionPoints.zone1.leavePoint3:setPosition(428,2060,56,80)
InteractionPoints.zone1.leavePoint3:setBackgroundTexture("res/transparent4x4pixel.png",true)
InteractionPoints.zone1.leavePoint3:setInteractionType(Enums.Zone)
InteractionPoints.zone1.leavePoint3:setInteractionCode("DawnInterface.enterZone( 'res/_lua/arinoxMagesGuild', -158, 0 );\nDawnInterface.setSavingAllowed( true );");

DawnInterface.setCurrentZone("res/_lua/zone1")
InteractionPoints.quest_hexmaster.ornadSaidorInteraction=DawnInterface.addCharacterInteractionPoint(SpawnPoints.quest_hexmaster.ornadSaidor);
InteractionPoints.quest_hexmaster.ornadSaidorInteraction:setInteractionType(Enums.Quest)
InteractionPoints.quest_hexmaster.ornadSaidorInteraction:setInteractionCode( "Quests.quest_hexmaster.onOrnadSaidorInteraction()" );

DawnInterface.setCurrentZone("res/_lua/zone1")
InteractionPoints.quest_hexmaster.noviceLeaderInteraction=DawnInterface.addCharacterInteractionPoint(SpawnPoints.quest_hexmaster.noviceLeader);
InteractionPoints.quest_hexmaster.noviceLeaderInteraction:setInteractionType(Enums.Quest)
InteractionPoints.quest_hexmaster.noviceLeaderInteraction:setInteractionCode( "Quests.quest_hexmaster.onNoviceLeaderInteraction()" );

DawnInterface.setCurrentZone("res/_lua/zone1")
InteractionRegions.quest_venomousveins.sylphirwoodsEnter=DawnInterface.addInteractionRegion();
InteractionRegions.quest_venomousveins.sylphirwoodsEnter:setPosition(-1215,-1000,100,360)
InteractionRegions.quest_venomousveins.sylphirwoodsEnter:setOnEnterText("Quests.quest_venomousveins.enterWoods()");

DawnInterface.setCurrentZone("res/_lua/zone1")
InteractionRegions.zone1.leavePoint1=DawnInterface.addInteractionPoint();
InteractionRegions.zone1.leavePoint1:setPosition(747,1588,56,80)
InteractionRegions.zone1.leavePoint1:setBackgroundTexture("res/transparent4x4pixel.png",true)
InteractionRegions.zone1.leavePoint1:setInteractionType(Enums.Zone)
InteractionRegions.zone1.leavePoint1:setInteractionCode("DawnInterface.enterZone('res/_lua/arinoxGeneralShop', -158, 0);\nDawnInterface.setSavingAllowed( true );");

DawnInterface.setCurrentZone("res/_lua/zone1")
InteractionRegions.zone1.townEnterRegion=DawnInterface.addInteractionRegion();
InteractionRegions.zone1.townEnterRegion:setPosition(550,1300,300,100)
InteractionRegions.zone1.townEnterRegion:setOnEnterText("Zones.zone1.enterTown()");

DawnInterface.setCurrentZone("res/_lua/zone1")
InteractionRegions.zone1.townLeaveRegion=DawnInterface.addInteractionRegion();
InteractionRegions.zone1.townLeaveRegion:setPosition(550,1200,300,100)
InteractionRegions.zone1.townLeaveRegion:setOnEnterText("Zones.zone1.leaveTown()");

DawnInterface.setCurrentZone("res/_lua/zone1")
InteractionRegions.quest_hexmaster.questStartRegion=DawnInterface.addInteractionRegion();
InteractionRegions.quest_hexmaster.questStartRegion:setPosition(730,2250,400,310)
InteractionRegions.quest_hexmaster.questStartRegion:setOnEnterText("Quests.quest_hexmaster.onQuestGiverRegionInteraction()");

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
DawnInterface.enterZone( "res/_lua/zone1", 3310, 533 );
DawnInterface.setSavingAllowed( true );
