function Zones.arinoxMagesGuild.init()
	if(not Zones.arinoxMagesGuild.inited) then
		InteractionPoints.arinoxMagesGuild.leavePoint = DawnInterface.addInteractionPoint();
		InteractionPoints.arinoxMagesGuild.leavePoint:setPosition( -160, -94, 80, 80 );
		InteractionPoints.arinoxMagesGuild.leavePoint:setBackgroundTexture("res/transparent4x4pixel.png", true);
		InteractionPoints.arinoxMagesGuild.leavePoint:setInteractionType( Enums.Zone );
		InteractionPoints.arinoxMagesGuild.leavePoint:setInteractionCode( "DawnInterface.enterZone( 'res/_lua/zone1', 430, 2020 );\nDawnInterface.setSavingAllowed( true );" );

		SpawnPoints.arinoxMagesGuild.witch = DawnInterface.addMobSpawnPoint( "Witch", "Clara Wickbrew", -344, 180, 1, 0, Enums.FRIENDLY);
		InteractionPoints.arinoxMagesGuild.witchInteraction = DawnInterface.addCharacterInteractionPoint( SpawnPoints.arinoxMagesGuild.witch );
		InteractionPoints.arinoxMagesGuild.witchInteraction:setInteractionType( Enums.Quest );
		InteractionPoints.arinoxMagesGuild.witchInteraction:setInteractionCode( "Zones.arinoxMagesGuild.onActivate()" );
		
		Zones.arinoxMagesGuild.inited = true;
		Zones.arinoxMagesGuild.path = "res/_lua/arinoxMagesGuild.init.lua"
	end	
	
	--arinoxMagesGuild.traderShop = DawnInterface.addShop();
	--arinoxMagesGuild.traderShop:addItem( itemDatabase["leynorscap"] );
	--arinoxMagesGuild.traderShop:addItem( itemDatabase["smallhealingpotion"] );
	--arinoxMagesGuild.traderShop:addItem( itemDatabase["smallhealingpotion"] );
	--arinoxMagesGuild.traderShop:addItem( itemDatabase["smallhealingpotion"] );
	--arinoxMagesGuild.traderShop:addItem( itemDatabase["smallhealingpotion"] );
	--arinoxMagesGuild.traderShop:addItem( itemDatabase["smallhealingpotion"] );
	--arinoxMagesGuild.traderShop:addItem( itemDatabase["smallhealingpotion"] );
	--arinoxMagesGuild.traderShop:addItem( itemDatabase["smallhealingpotion"] );
	--arinoxMagesGuild.traderShop:addItem( itemDatabase["smallhealingpotion"] );
	--arinoxMagesGuild.traderShop:addItem( itemDatabase["smallhealingpotion"] );
	--arinoxMagesGuild.traderShop:addItem( itemDatabase["smallhealingpotion"] );
	--arinoxMagesGuild.traderShop:addItem( itemDatabase["bladeofstrength"] );
	--arinoxMagesGuild.traderShop:addItem( itemDatabase["bookofleatherskinrank2"] );
end

function Zones.arinoxMagesGuild.onEnterMap(x,y)
	local textWindow = DawnInterface.createTextWindow();
	textWindow:center();
	textWindow:setText( "Mages' Guild" );
	textWindow:setAutocloseTime( 1000 );
end

function Zones.arinoxMagesGuild.onActivate()
	--arinoxMagesGuild.traderShop:toggle()
	local textWindow = DawnInterface.createTextWindow();
	textWindow:setPosition( Enums.CENTER, 512, 382 );
	textWindow:setAutocloseTime( 0 );
	textWindow:setText( "Where are my mushrooms? I've lost my mushrooms!" );
end

Zones.arinoxMagesGuild.init()