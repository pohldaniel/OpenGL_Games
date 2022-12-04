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
	local leavePoint = DawnInterface.addInteractionPoint();
	leavePoint:setPosition( 747, 1588, 56, 80 );
	leavePoint:setInteractionType( Enums.Zone );
	leavePoint:setBackgroundTexture( "res/transparent4x4pixel.png", true);
	leavePoint:setInteractionCode( "DawnInterface.enterZone('res/_lua/arinoxGeneralShop', -158, 0);\nDawnInterface.setSavingAllowed( true );" );

	local townEnterRegion = DawnInterface.addInteractionRegion();
	townEnterRegion:setPosition( 550, 1300, 300, 100 );
	townEnterRegion:setOnEnterText( "zone1.enterTown()" );
	local townLeaveRegion = DawnInterface.addInteractionRegion();
	townLeaveRegion:setPosition( 550, 1200, 300, 100 );
	townLeaveRegion:setOnEnterText( "zone1.leaveTown()" );
end

print("Init Quests")
dofile("res/_lua/quests_wood.lua");
dofile("res/_lua/quests_hexmaster.lua");
dofile("res/_lua/quests_venomousveins.lua");

if( zone1.inited == nil ) then
	zone1.inited = true
	zone1.init()
end


