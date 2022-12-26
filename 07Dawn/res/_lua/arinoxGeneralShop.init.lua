function Zones.arinoxGeneralShop.init()
	if(not Zones.arinoxGeneralShop.inited) then
		InteractionPoints.arinoxGeneralShop.leavePoint = DawnInterface.addInteractionPoint();
		InteractionPoints.arinoxGeneralShop.leavePoint:setPosition( -160, -94, 80, 80 );
		InteractionPoints.arinoxGeneralShop.leavePoint:setBackgroundTexture( "res/transparent4x4pixel.png", true);
		InteractionPoints.arinoxGeneralShop.leavePoint:setInteractionType( Enums.Zone );
		InteractionPoints.arinoxGeneralShop.leavePoint:setInteractionCode( "DawnInterface.enterZone( 'res/_lua/zone1', 747, 1530 );\nDawnInterface.setSavingAllowed( true );" );

		SpawnPoints.arinoxGeneralShop.trader = DawnInterface.addMobSpawnPoint( "Human", "Rake Fleetwood", -344, 180, 1, 0, Enums.FRIENDLY);
		InteractionPoints.arinoxGeneralShop.traderInteraction = DawnInterface.addCharacterInteractionPoint( SpawnPoints.arinoxGeneralShop.trader );
		InteractionPoints.arinoxGeneralShop.traderInteraction:setInteractionType( Enums.Shop );
		InteractionPoints.arinoxGeneralShop.traderInteraction:setInteractionCode( "Zones.arinoxGeneralShop.onActivateTrader()" );
		
		Zones.arinoxGeneralShop.inited = true;
		Zones.arinoxGeneralShop.path = "res/_lua/arinoxGeneralShop.init.lua"
	end
	
	traderShop2 = DawnInterface.addShop("shop2");
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
	traderShop2:addItem( itemDatabase["bladeofstrength"] );
	traderShop2:addItem( itemDatabase["bookofleatherskinrank2"] );
	traderShop2:loadShopkeeperInventory();
end

function Zones.arinoxGeneralShop.onEnterMap(x,y)
	local textWindow = DawnInterface.createTextWindow();
	textWindow:center();
	textWindow:setText( "Arinox general shop" );
	textWindow:setAutocloseTime( 1000 );
end

function Zones.arinoxGeneralShop.onActivateTrader()
	ShopCanvas:Get():activate();
end

Zones.arinoxGeneralShop.init()