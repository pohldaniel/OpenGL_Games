function Zones.zone1.enterTown()
	if ( not DawnInterface.isSavingAllowed() )
	then
		DawnInterface.setSavingAllowed( true );

		local textWindow = DawnInterface.createTextWindow();
		textWindow:center();
		textWindow:setText( "The town of Arinox." );
		textWindow:setAutocloseTime( 1000 );
	end
end

function Zones.zone1.leaveTown()
	if ( DawnInterface.isSavingAllowed() )
	then
		DawnInterface.setSavingAllowed( false );
	end
end

function Zones.zone1.onEnterMap(x,y)
    --DawnInterface.setBackgroundMusic("res/music/Early_Dawn_Simple.ogg");
end

function Zones.zone1.init()
	if(not Zones.zone1.inited) then
		InteractionPoints.zone1.leavePoint1 = DawnInterface.addInteractionPoint();
		InteractionPoints.zone1.leavePoint1:setPosition( 747, 1588, 56, 80 );
		InteractionPoints.zone1.leavePoint1:setInteractionType( Enums.Zone );
		InteractionPoints.zone1.leavePoint1:setBackgroundTexture("res/transparent4x4pixel.png", true);
		InteractionPoints.zone1.leavePoint1:setInteractionCode( "DawnInterface.enterZone('res/_lua/arinoxGeneralShop', -158, 0);\nDawnInterface.setSavingAllowed( true );" );
		
		InteractionPoints.zone1.leavePoint2 = DawnInterface.addInteractionPoint();
		InteractionPoints.zone1.leavePoint2:setPosition( 822, 2570, 128, 128 );
		InteractionPoints.zone1.leavePoint2:setBackgroundTexture( "res/village/stairs.tga" );
		InteractionPoints.zone1.leavePoint2:setInteractionType( Enums.Zone );
		InteractionPoints.zone1.leavePoint2:setInteractionCode( "DawnInterface.enterZone( 'res/_lua/arinoxDungeonLevel1', 2020, 1880 );\nDawnInterface.setSavingAllowed( false );" );

		InteractionPoints.zone1.leavePoint3 = DawnInterface.addInteractionPoint();
		InteractionPoints.zone1.leavePoint3:setPosition( 428, 2060, 56, 80 );
		InteractionPoints.zone1.leavePoint3:setInteractionType( Enums.Zone );
		InteractionPoints.zone1.leavePoint3:setBackgroundTexture( "res/transparent4x4pixel.png", true);
		InteractionPoints.zone1.leavePoint3:setInteractionCode( "DawnInterface.enterZone( 'res/_lua/arinoxMagesGuild', -158, 0 );\nDawnInterface.setSavingAllowed( true );" );
		--InteractionPoints.zone1.leavePoint3:setInteractionCode( "DawnInterface.enterZone( 'res/_lua/zone2', -670, 1529 );\nDawnInterface.setSavingAllowed( false );" );
		--InteractionPoints.zone1.leavePoint3:setInteractionCode( "DawnInterface.enterZone( 'res/_lua/arinoxDungeonLevel1', 2020, 1880 );\nDawnInterface.setSavingAllowed( false );" );

		InteractionRegions.zone1.townEnterRegion = DawnInterface.addInteractionRegion();
		InteractionRegions.zone1.townEnterRegion:setPosition( 550, 1300, 300, 100 );
		InteractionRegions.zone1.townEnterRegion:setOnEnterText( "Zones.zone1.enterTown()" );
	
		InteractionRegions.zone1.townLeaveRegion = DawnInterface.addInteractionRegion();
		InteractionRegions.zone1.townLeaveRegion:setPosition( 550, 1200, 300, 100 );
		InteractionRegions.zone1.townLeaveRegion:setOnEnterText( "Zones.zone1.leaveTown()" );
	
		dofile("res/_lua/quests_wood.lua");
		dofile("res/_lua/quests_hexmaster.lua");
		dofile("res/_lua/quests_venomousveins.lua");
	
		Zones.zone1.inited = true;
		Zones.zone1.path = "res/_lua/zone1.init.lua";
	end
end

Zones.zone1.init()