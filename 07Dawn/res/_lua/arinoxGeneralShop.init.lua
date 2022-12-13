arinoxGeneralShop = DawnInterface.getCurrentZone();

function arinoxGeneralShop.init()

	arinoxGeneralShop.leavePoint = DawnInterface.addInteractionPoint();
	arinoxGeneralShop.leavePoint:setPosition( -160, -94, 80, 80 );
	arinoxGeneralShop.leavePoint:setBackgroundTexture( "res/transparent4x4pixel.png", true);
	arinoxGeneralShop.leavePoint:setInteractionType( Enums.Zone );
	arinoxGeneralShop.leavePoint:setInteractionCode( 'DawnInterface.enterZone( "res/_lua/zone1", 747, 1530 );\nDawnInterface.setSavingAllowed( true );' );

	arinoxGeneralShop.trader = DawnInterface.addMobSpawnPoint( "Human", "Rake Fleetwood", -344, 180, 1, 0, Enums.FRIENDLY);
	arinoxGeneralShop.traderInteraction = DawnInterface.addCharacterInteractionPoint( arinoxGeneralShop.trader );
	arinoxGeneralShop.traderInteraction:setInteractionType( Enums.Shop );
	arinoxGeneralShop.traderInteraction:setInteractionCode( "arinoxGeneralShop.onActivateTrader()" );
	
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


function arinoxGeneralShop.onEnterMap(x,y)
	local textWindow = DawnInterface.createTextWindow();
	textWindow:center();
	textWindow:setText( "Arinox general shop" );
	textWindow:setAutocloseTime( 1000 );
end

function arinoxGeneralShop.onActivateTrader()
	ShopCanvas:Get():activate();
end

if( arinoxGeneralShop.inited == nil ) then
	arinoxGeneralShop.inited = true
	arinoxGeneralShop.init()
end
