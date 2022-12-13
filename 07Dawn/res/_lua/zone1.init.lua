zone1 = DawnInterface.getCurrentZone();

function zone1.enterTown()
	if ( not DawnInterface.isSavingAllowed() )
	then
		DawnInterface.setSavingAllowed( true );

		local textWindow = DawnInterface.createTextWindow();
		textWindow:center();
		textWindow:setText( "The town of Arinox." );
		textWindow:setAutocloseTime( 1000 );
	end
end

function zone1.leaveTown()
	if ( DawnInterface.isSavingAllowed() )
	then
		DawnInterface.setSavingAllowed( false );
	end
end

function zone1.onEnterMap(x,y)
    --DawnInterface.setBackgroundMusic("res/music/Early_Dawn_Simple.ogg");
end

function zone1.init()
	zone1.leavePoint = DawnInterface.addInteractionPoint();
	zone1.leavePoint:setPosition( 747, 1588, 56, 80 );
	zone1.leavePoint:setInteractionType( Enums.Zone );
	zone1.leavePoint:setBackgroundTexture( "res/transparent4x4pixel.png", true);
	zone1.leavePoint:setInteractionCode( "DawnInterface.enterZone('res/_lua/arinoxGeneralShop', -158, 0);\nDawnInterface.setSavingAllowed( true );" );

	zone1.leavePoint = DawnInterface.addInteractionPoint();
	zone1.leavePoint:setPosition( 822, 2570, 128, 128 );
	zone1.leavePoint:setBackgroundTexture( "res/village/stairs.tga" );
	zone1.leavePoint:setInteractionType( Enums.Zone );
	zone1.leavePoint:setInteractionCode( 'DawnInterface.enterZone( "res/_lua/arinoxDungeonLevel1", 2020, 1880 );\nDawnInterface.setSavingAllowed( false );' );

	zone1.leavePoint = DawnInterface.addInteractionPoint();
	zone1.leavePoint:setPosition( 428, 2060, 56, 80 );
	zone1.leavePoint:setInteractionType( Enums.Zone );
	zone1.leavePoint:setBackgroundTexture( "res/transparent4x4pixel.png", true);
	zone1.leavePoint:setInteractionCode( 'DawnInterface.enterZone( "res/_lua/arinoxMagesGuild", -158, 0 );\nDawnInterface.setSavingAllowed( true );' );

	zone1.townEnterRegion = DawnInterface.addInteractionRegion();
	zone1.townEnterRegion:setPosition( 550, 1300, 300, 100 );
	zone1.townEnterRegion:setOnEnterText( "zone1.enterTown()" );
	zone1.townLeaveRegion = DawnInterface.addInteractionRegion();
	zone1.townLeaveRegion:setPosition( 550, 1200, 300, 100 );
	zone1.townLeaveRegion:setOnEnterText( "zone1.leaveTown()" );
end

--print("Init Quests")
dofile("res/_lua/quests_wood.lua");
dofile("res/_lua/quests_hexmaster.lua");
dofile("res/_lua/quests_venomousveins.lua");

if( zone1.inited == nil ) then
	zone1.inited = true
	zone1.init()
end


