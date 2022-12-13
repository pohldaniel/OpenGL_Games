-- init quest namespace so different quests / zones don't conflict in names
if( quest_playHideAndSeek == nil )
then
	quest_playHideAndSeek = {}
end

function quest_playHideAndSeek.init()
	quest_playHideAndSeek.john = DawnInterface.addInteractionPoint();
	quest_playHideAndSeek.john:setPosition( 820, 270, 64, 64 );
	quest_playHideAndSeek.john:setBackgroundTexture( "res/character/John.tga" );
	quest_playHideAndSeek.john:setInteractionType( Enums.Quest );
	quest_playHideAndSeek.john:setInteractionCode( "quest_playHideAndSeek.onActivateJohn()" );
	
	--quest_playHideAndSeek.onActivateJohn();
end

function quest_playHideAndSeek.onActivateJohn()
	if( not quest_playHideAndSeek.fulfilled ) then
		if( quest_playHideAndSeek.added == nil ) then
			quest_playHideAndSeek.added = true;
			quest_playHideAndSeek.fulfilled = false;
			quest_playHideAndSeek.rewardGot = false;
			quest_playHideAndSeek.monsterSpawnPoint = DawnInterface.addMobSpawnPoint("Giant_Wolf", "Giant_Wolf", 3462, 557, 1, 0, Enums.HOSTILE);
			onDieEventHandler = DawnInterface.createEventHandler();
			onDieEventHandler:setExecuteText( "quest_playHideAndSeek.onKilledQuestMonster()" );
			quest_playHideAndSeek.monsterSpawnPoint:addOnDieEventHandler( onDieEventHandler );
			quest_playHideAndSeek.quest = DawnInterface.addQuest("Hide and Seek", "My little brother James is somewhere in this forest. I fear he got lost. Please find him and tell him to come to me." );
			quest_playHideAndSeek.quest:setExperienceReward( 150 );
			quest_playHideAndSeek.quest:setCoinReward( 821 );
			quest_playHideAndSeek.quest:setItemReward( itemDatabase["ringofdefense"] );
		end
		local textWindow = DawnInterface.createTextWindow();
		textWindow:center();
		textWindow:setText( "My little brother James is somewhere in this forest. I fear he got lost. Please find him and tell him to come to me." );
		textWindow:setAutocloseTime( 4000 );
	elseif( quest_playHideAndSeek.fulfilled and not quest_playHideAndSeek.rewardGot ) then
		local textWindow = DawnInterface.createTextWindow();
		textWindow:setPosition( Enums.CENTER, 512, 382 );
		textWindow:setText( "Thank you for looking after my brother. We will leave for home now. Take this ring for your effords." );
		textWindow:setAutocloseTime( 5000 );
		quest_playHideAndSeek.rewardGot = true;
		DawnInterface.removeInteractionPoint( john );
		DawnInterface.removeInteractionPoint( james );
		john = nil;
		james = nil;
		quest_playHideAndSeek.quest:finishQuest();
		quest_playHideAndSeek.quest = nil
	end
end

function quest_playHideAndSeek.onKilledQuestMonster()
	james = DawnInterface.addInteractionPoint();
	james:setPosition( 3783,640, 64, 64 );
	james:setBackgroundTexture( "res/character/James.tga" );
	james:setInteractionType( InteractionType.Quest );
	james:setInteractionCode( "quest_playHideAndSeek.onActivateJames()" );
	DawnInterface.removeMobSpawnPoint( quest_playHideAndSeek.monsterSpawnPoint );
	quest_playHideAndSeek.monsterSpawnPoint = nil;
end

function quest_playHideAndSeek.onActivateJames()
	if( quest_playHideAndSeek == nil or quest_playHideAndSeek.fulfilled ) then
		local textWindow = DawnInterface.createTextWindow();
		textWindow:setPosition( Enums.CENTER, 512, 382 );
		textWindow:setText( "This forest is so exciting." );
		textWindow:setAutocloseTime( 2000 );
	elseif( not quest_playHideAndSeek.fulfilled ) then
		local textWindow = DawnInterface.createTextWindow();
		textWindow:center();
		textWindow:setText( "Thank you for helping me. I walked through this wonderful forest when suddenly I saw this huge wolf. Then I hid in the trees and hoped it would get away, but it didn't. I was so afraid. I'll better hurry to my brother John now." );
		textWindow:setAutocloseTime( 10000 );
		james:setPosition( 800, 200, 64, 64 );
		quest_playHideAndSeek.fulfilled = true;
		quest_playHideAndSeek.quest:setDescription("Found James whose way was blocked by a huge wolf. He is on his way back to John now.");
	end
end

-- init quest if this has not been done yet
if( quest_playHideAndSeek.inited == nil ) then
	quest_playHideAndSeek.inited = true
	quest_playHideAndSeek.init()
end
