function Quests.quest_venomousveins.init()
  -- this quest is initialized from zone zone1
  if(not Quests.quest_venomousveins.inited) then
	SpawnPoints.quest_venomousveins.effreyLongback = DawnInterface.addMobSpawnPoint("Human", "Effrey Longback", -1510, 500, 1, 0, Enums.FRIENDLY);
	SpawnPoints.quest_venomousveins.effreyLongback:setWanderRadius( 50 );
	InteractionPoints.quest_venomousveins.effreyLongbackInteraction = DawnInterface.addCharacterInteractionPoint(SpawnPoints.quest_venomousveins.effreyLongback );
	InteractionPoints.quest_venomousveins.effreyLongbackInteraction:setInteractionType( Enums.Quest );
	InteractionPoints.quest_venomousveins.effreyLongbackInteraction:setInteractionCode( "Quests.quest_venomousveins.speakWithEffrey()" );

	SpawnPoints.quest_venomousveins.jemma = DawnInterface.addMobSpawnPoint("Witch", "Jemma", -2162, -4580, 1, 0, Enums.FRIENDLY);
	SpawnPoints.quest_venomousveins.jemma:setWanderRadius( 50 );
	InteractionPoints.quest_venomousveins.jemmaInteraction = DawnInterface.addCharacterInteractionPoint(SpawnPoints.quest_venomousveins.jemma );
	InteractionPoints.quest_venomousveins.jemmaInteraction:setInteractionType( Enums.Quest );
	InteractionPoints.quest_venomousveins.jemmaInteraction:setInteractionCode( "Quests.quest_venomousveins.speakWithJemma()" );
	
	InteractionRegions.quest_venomousveins.sylphirwoodsEnter = DawnInterface.addInteractionRegion();
	InteractionRegions.quest_venomousveins.sylphirwoodsEnter:setPosition( -1215, -1000, 100, 360 );
	InteractionRegions.quest_venomousveins.sylphirwoodsEnter:setOnEnterText( "Quests.quest_venomousveins.enterWoods()" );
	
	Quests.quest_venomousveins.inited = true;
	Quests.quest_venomousveins.path = "res/_lua/quests_venomousveins.lua"
  end

 end

function Quests.quest_venomousveins.enterWoods()
  local textWindow = DawnInterface.createTextWindow();
  textWindow:setPosition(Enums.CENTER, 512, 382);
  textWindow:setText( "The Sylphir woods." );
  textWindow:setAutocloseTime( 1000 );
end

function Quests.quest_venomousveins.speakWithJemma()
  if( Quests.quest_venomousveins.quest_one and not Quests.quest_venomousveins.gotAntidote ) then
    if( not Quests.quest_venomousveins.completed and not Quests.quest_venomousveins.gotAntidote ) then
      if( Quests.quest_venomousveins.added == true and Quests.quest_venomousveins.completed == false and Quests.quest_venomousveins.gotInstructionFromJemma == false ) then
        Quests.quest_venomousveins.showJemmaText( 2 );
        Quests.quest_venomousveins.gotInstructionFromJemma = true;
        Quests.quest_venomousveins.quest_one:setDescription( "Jemma requires two fangs from the small spiderlings and three pristine spidersilks from the Webweaves in order to make an antidote." );
        Quests.quest_venomousveins.quest_one:addRequiredItemForCompletion( itemDatabase["pristinesylphirsilk"], 3 );
        Quests.quest_venomousveins.quest_one:addRequiredItemForCompletion( itemDatabase["sylphirfang"], 2 );
      elseif( Quests.quest_venomousveins.added == true and Quests.quest_venomousveins.completed == false and Quests.quest_venomousveins.gotInstructionFromJemma == true ) then
        Quests.quest_venomousveins.showJemmaText( 4 );
      end
    end

    -- try to finish the quest if possible.
    if( Quests.quest_venomousveins.quest_one:finishQuest() == true ) then
      Quests.quest_venomousveins.quest_one = nil;
      Quests.quest_venomousveins.gotAntidote = true;
      Quests.quest_venomousveins.showJemmaText( 5 );
      Quests.quest_venomousveins.quest_two = DawnInterface.addQuest( "Sylphir antidote", "Deliver the antidote to Effrey's daughter before it's too late!" );
      Quests.quest_venomousveins.quest_two:addRequiredItemForCompletion( itemDatabase["sylphirantidote"], 1 );
      Quests.quest_venomousveins.quest_two:setExperienceReward( 350 );
      Quests.quest_venomousveins.quest_two:setItemReward( itemDatabase["ringofdefense"] );
    end
  else
    Quests.quest_venomousveins.showJemmaText( 1 );
  end
end


function Quests.quest_venomousveins.speakWithEffrey()
  if( Quests.quest_venomousveins.quest_two ) then
    if( Quests.quest_venomousveins.quest_two:finishQuest() == true ) then
      Quests.quest_venomousveins.quest_two = nil;
      Quests.quest_venomousveins.completed = true;
      Quests.quest_venomousveins.showJeffreyText( 4 );
    end
  end

  if( not Quests.quest_venomousveins.completed ) then
    if( Quests.quest_venomousveins.added == nil ) then
      Quests.quest_venomousveins.added = true;
      Quests.quest_venomousveins.completed = false;
      Quests.quest_venomousveins.gotReward = false;
      Quests.quest_venomousveins.gotAntidote = false;
      Quests.quest_venomousveins.gotInstructionFromJemma = false;
      Quests.quest_venomousveins.quest_one = DawnInterface.addQuest( "Venomous veins", "Effrey Longback's daughter was bitten by a Sylphir spider. Search out Jemma the alchemist in the southern part of the Sylphir woods and obtain an antidote for Effrey." );
      Quests.quest_venomousveins.quest_one:setItemReward( itemDatabase["sylphirantidote"] );
      Quests.quest_venomousveins.showJeffreyText( 1 );
    elseif( Quests.quest_venomousveins.added == true and Quests.quest_venomousveins.completed == false and Quests.quest_venomousveins.gotAntidote == false ) then
      Quests.quest_venomousveins.showJeffreyText( 3 );
    end
  end
end

function Quests.quest_venomousveins.showJeffreyText( part )
  if( part == 1 ) then
    local textWindow = DawnInterface.createTextWindow();
    textWindow:setPosition( Enums.CENTER, 512, 382 );
    textWindow:setText( "My daughter was bitten by a spider this morning. She is really sick and I am fearing the worst. I don't know if she went playing near the Sylphir woods... She knows it's forbidden! It's only getting worse and if I don't get an antidote to cure her I don't know what I will do. I have heard about an alchemist named Jemma in the southern part of the Sylphir woods. I would go myself but I cannot leave my little girl. You look like you could make it to Jemma's house unharmed." );
    textWindow:setAutocloseTime( 0 );
    textWindow:setOnCloseText( "Quests.quest_venomousveins.showJeffreyText( 2 )" );
  end

  if( part == 2 ) then
    local textWindow = DawnInterface.createTextWindow();
    textWindow:setPosition( Enums.CENTER, 512, 382 );
    textWindow:setText( "Could you visit Jemma and bring me back an antidote? I'm no rich man, but I do have some pieces of equipment I could trade in return for the antidote. Please hurry!" );
    textWindow:setAutocloseTime( 0 );
  end

  if( part == 3 ) then
    local textWindow = DawnInterface.createTextWindow();
    textWindow:setPosition( Enums.CENTER, 512, 382 );
    textWindow:setText( "Please seek out Jemma down in the southern part of the Sylphir woods. My daugher is really sick!" );
    textWindow:setAutocloseTime( 0 );
  end

  if( part == 4 ) then
    local textWindow = DawnInterface.createTextWindow();
    textWindow:setPosition( Enums.CENTER, 512, 382 );
    textWindow:setText( "Thank you so much brave one! You have saved my daughters life! I know I can't repay you for this, but here.. take this armor. Does more use with you than with me." );
    textWindow:setAutocloseTime( 0 );
  end
end

function Quests.quest_venomousveins.showJemmaText( part )
  if( part == 1 ) then
    local textWindow = DawnInterface.createTextWindow();
    textWindow:setPosition( Enums.CENTER, 512, 382 );
    textWindow:setText( "I'm Jemma. Not many people dare to wander down here due to the spiders around. Don't you know these woods are dangerous? Sylphirs are growing stronger these days. Beware of the small ones. They may not look tough, but they can give you quite a bite..." );
    textWindow:setAutocloseTime( 7000 );
  end

  if( part == 2 ) then
    local textWindow = DawnInterface.createTextWindow();
    textWindow:setPosition( Enums.CENTER, 512, 382 );
    textWindow:setText( "Someone got bitten? Well, you shouldn't wander out in the woods unless you are prepared to face off against those pesky spiders. Lucky for you I know just what you need... " );
    textWindow:setAutocloseTime( 0 );
    textWindow:setOnCloseText( "quest_venomousveins.showJemmaText( 3 )" );
  end

  if( part == 3 ) then
    local textWindow = DawnInterface.createTextWindow();
    textWindow:setPosition( Enums.CENTER, 512, 382 );
    textWindow:setText( "To make this antidote I will require two fangs from the small spiderlings. Also, three pristine spidersilks from the Webweavers. Bring me this and I have enough to make your antidote. We can discuss my fee when you return." );
    textWindow:setAutocloseTime( 0 );
  end

  if( part == 4 ) then
    local textWindow = DawnInterface.createTextWindow();
    textWindow:setPosition( Enums.CENTER, 512, 382 );
    textWindow:setText( "How is the gathering coming along? Remember, I need spidersilks in pristine condition and two spiderling fangs!" );
    textWindow:setAutocloseTime( 0 );
  end

  if( part == 5 ) then
    local textWindow = DawnInterface.createTextWindow();
    textWindow:setPosition( Enums.CENTER, 512, 382 );
    textWindow:setText( "Excellent! I hope it wasn't too much trouble getting all this." );
    textWindow:setAutocloseTime( 0 );
    textWindow:setOnCloseText( "quest_venomousveins.showJemmaText( 6 )" );
  end

  if( part == 6 ) then
    local textWindow = DawnInterface.createTextWindow();
    textWindow:setPosition( Enums.CENTER, 512, 382 );
    textWindow:setText( "I have already finished the potion in the waiting of your arrival. You better hurry up, you might not have that much time left. Now remember, you are in my debt. I will call upon you to do me a favor once the time is right." );
    textWindow:setAutocloseTime( 0 );
  end
end

Quests.quest_venomousveins.init()