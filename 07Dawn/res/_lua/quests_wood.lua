function Quests.quest_playHideAndSeek.init()

	if(not Quests.quest_playHideAndSeek.inited) then
		InteractionPoints.quest_playHideAndSeek.john = DawnInterface.addInteractionPoint();
		InteractionPoints.quest_playHideAndSeek.john:setPosition( 820, 270, 64, 64 );
		InteractionPoints.quest_playHideAndSeek.john:setBackgroundTexture( "res/character/John.tga" );
		InteractionPoints.quest_playHideAndSeek.john:setInteractionType( Enums.Quest );
		InteractionPoints.quest_playHideAndSeek.john:setInteractionCode( "Quests.quest_playHideAndSeek.onActivateJohn()" );	
		Quests.quest_playHideAndSeek.inited = true
		Quests.quest_playHideAndSeek.path = "res/_lua/quests_wood.lua"
	end
	
end

function Quests.quest_playHideAndSeek.onActivateJohn()
	if( not Quests.quest_playHideAndSeek.fulfilled ) then
		if( Quests.quest_playHideAndSeek.added == nil ) then
			Quests.quest_playHideAndSeek.added = true;
			Quests.quest_playHideAndSeek.fulfilled = false;
			Quests.quest_playHideAndSeek.rewardGot = false;
			
			SpawnPoints.quest_playHideAndSeek.monsterSpawnPoint = DawnInterface.addMobSpawnPoint("Giant_Wolf", "Giant_Wolf", 3462, 557, 1, 0, Enums.HOSTILE);
			
			Eventhandlers.quest_playHideAndSeek.onDieEventHandler = DawnInterface.createEventHandler();
			Eventhandlers.quest_playHideAndSeek.onDieEventHandler:setExecuteText( "Quests.quest_playHideAndSeek.onKilledQuestMonster()" );
			
			SpawnPoints.quest_playHideAndSeek.monsterSpawnPoint:addOnDieEventHandler(Eventhandlers.quest_playHideAndSeek.onDieEventHandler );
			Quests.quest_playHideAndSeek.quest = DawnInterface.addQuest("Hide and Seek", "My little brother James is somewhere in this forest. I fear he got lost. Please find him and tell him to come to me." );
			Quests.quest_playHideAndSeek.quest:setExperienceReward( 150 );
			Quests.quest_playHideAndSeek.quest:setCoinReward( 821 );
			Quests.quest_playHideAndSeek.quest:setItemReward( itemDatabase["ringofdefense"] );
		end
		
		local textWindow = DawnInterface.createTextWindow();
		textWindow:center();
		textWindow:setText( "My little brother James is somewhere in this forest. I fear he got lost. Please find him and tell him to come to me." );
		textWindow:setAutocloseTime( 4000 );
	elseif( Quests.quest_playHideAndSeek.fulfilled and not Quests.quest_playHideAndSeek.rewardGot ) then
		local textWindow = DawnInterface.createTextWindow();
		textWindow:setPosition( Enums.CENTER, 512, 382 );
		textWindow:setText( "Thank you for looking after my brother. We will leave for home now. Take this ring for your effords." );
		textWindow:setAutocloseTime( 5000 );
		Quests.quest_playHideAndSeek.rewardGot = true;
		DawnInterface.removeInteractionPoint( InteractionPoints.quest_playHideAndSeek.john );
		DawnInterface.removeInteractionPoint( InteractionPoints.quest_playHideAndSeek.james );
		InteractionPoints.quest_playHideAndSeek.john = nil;
		InteractionPoints.quest_playHideAndSeek.james = nil;
		
		Quests.quest_playHideAndSeek.removeJohn = true;
		
		Quests.quest_playHideAndSeek.quest:finishQuest();
		Quests.quest_playHideAndSeek.quest = nil
	end
end

function Quests.quest_playHideAndSeek.onKilledQuestMonster()
	InteractionPoints.quest_playHideAndSeek.james = DawnInterface.addInteractionPoint();
	InteractionPoints.quest_playHideAndSeek.james:setPosition( 3783,640, 64, 64 );
	InteractionPoints.quest_playHideAndSeek.james:setBackgroundTexture( "res/character/James.tga" );
	InteractionPoints.quest_playHideAndSeek.james:setInteractionType( Enums.Quest );
	InteractionPoints.quest_playHideAndSeek.james:setInteractionCode( "Quests.quest_playHideAndSeek.onActivateJames()" );
	
	SpawnPoints.quest_playHideAndSeek.monsterSpawnPoint:removeOnDieEventHandler(Eventhandlers.quest_playHideAndSeek.onDieEventHandler);
	Eventhandlers.quest_playHideAndSeek.onDieEventHandler  = nil;
	
	DawnInterface.removeMobSpawnPoint( SpawnPoints.quest_playHideAndSeek.monsterSpawnPoint );
	SpawnPoints.quest_playHideAndSeek.monsterSpawnPoint = nil;
end

function Quests.quest_playHideAndSeek.onActivateJames()
	if( Quests.quest_playHideAndSeek == nil or Quests.quest_playHideAndSeek.fulfilled ) then
		local textWindow = DawnInterface.createTextWindow();
		textWindow:setPosition( Enums.CENTER, 512, 382 );
		textWindow:setText( "This forest is so exciting." );
		textWindow:setAutocloseTime( 2000 );
	elseif( not Quests.quest_playHideAndSeek.fulfilled ) then
		local textWindow = DawnInterface.createTextWindow();
		textWindow:center();
		textWindow:setText( "Thank you for helping me. I walked through this wonderful forest when suddenly I saw this huge wolf. Then I hid in the trees and hoped it would get away, but it didn't. I was so afraid. I'll better hurry to my brother John now." );
		textWindow:setAutocloseTime( 10000 );
		InteractionPoints.quest_playHideAndSeek.james:setPosition( 800, 200, 64, 64 );
		Quests.quest_playHideAndSeek.fulfilled = true;
		Quests.quest_playHideAndSeek.quest:setDescription("Found James whose way was blocked by a huge wolf. He is on his way back to John now.");
	end
end

Quests.quest_playHideAndSeek.init()
