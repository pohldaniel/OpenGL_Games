if ( arinoxGeneralShop == nil )
then
	arinoxGeneralShop = {};
	dofile("res/_lua/arinoxGeneralShop.spawnpoints.lua");
	arinoxGeneralShop.leavePoint = DawnInterface.addInteractionPoint();
	arinoxGeneralShop.leavePoint:setPosition( -160, -94, 80, 80 );
	arinoxGeneralShop.leavePoint:setBackgroundTexture( "res/transparent4x4pixel.png", true);
	arinoxGeneralShop.leavePoint:setInteractionType( Enums.Zone );
	arinoxGeneralShop.leavePoint:setInteractionCode( 'DawnInterface.enterZone( "res/_lua/zone1", 747, 1530 );\nDawnInterface.setSavingAllowed( true );' );

	trader = DawnInterface.addMobSpawnPoint( "Human", "Rake Fleetwood", -344, 180, 1, 0, Enums.FRIENDLY);
	traderInteraction = DawnInterface.addCharacterInteractionPoint( trader );
	traderInteraction:setInteractionType( Enums.Shop );
	traderInteraction:setInteractionCode( "arinoxGeneralShop.onActivateTrader()" );
	traderShop = DawnInterface.addShop("shop1");
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
	traderShop:addItem( itemDatabase["bladeofstrength"] );
	traderShop:addItem( itemDatabase["bookofleatherskinrank2"] );
	traderShop:loadShopkeeperInventory();
end

function arinoxGeneralShop.onEnterMap(x,y)
	local textWindow = DawnInterface.createTextWindow();
	textWindow:center();
	textWindow:setText( "Arinox general shop" );
	textWindow:setAutocloseTime( 1000 );
end

function arinoxGeneralShop.onActivateTrader()
	--traderShop:toggle()
end
