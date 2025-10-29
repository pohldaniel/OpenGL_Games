function Zones.arinoxDungeonLevel1.init()
    if(not Zones.arinoxDungeonLevel1.inited) then
        InteractionPoints.arinoxDungeonLevel1.leavePoint = DawnInterface.addInteractionPoint();
        InteractionPoints.arinoxDungeonLevel1.leavePoint:setPosition( 2007, 1913, 76, 70 );
        InteractionPoints.arinoxDungeonLevel1.leavePoint:setBackgroundTexture("res/transparent4x4pixel.png", true);
        InteractionPoints.arinoxDungeonLevel1.leavePoint:setInteractionType( Enums.Zone );
        InteractionPoints.arinoxDungeonLevel1.leavePoint:setInteractionCode( "DawnInterface.enterZone( 'res/_lua/zone1', 870, 2520 );\nDawnInterface.setSavingAllowed( true );" );

        InteractionRegions.arinoxDungeonLevel1.closedDoorSpot = DawnInterface.addInteractionRegion();
        InteractionRegions.arinoxDungeonLevel1.closedDoorSpot:setPosition( 990, 400, 130, 180 );
        InteractionRegions.arinoxDungeonLevel1.closedDoorSpot:setOnEnterText( "Zones.arinoxDungeonLevel1.closedDoorInteraction()" );

        InteractionPoints.arinoxDungeonLevel1.leaveRegionSpot = DawnInterface.addInteractionPoint();
        InteractionPoints.arinoxDungeonLevel1.leaveRegionSpot:setPosition( 990, 300, 130, 100 );
        InteractionPoints.arinoxDungeonLevel1.leaveRegionSpot:setBackgroundTexture("res/transparent4x4pixel.png", true);
        InteractionPoints.arinoxDungeonLevel1.leaveRegionSpot:setInteractionType( Enums.Zone );
        InteractionPoints.arinoxDungeonLevel1.leaveRegionSpot:setInteractionCode( "Zones.arinoxDungeonLevel1.nextRegionInteraction()" );

        Zones.arinoxDungeonLevel1.inited = true;
		Zones.arinoxDungeonLevel1.path = "res/_lua/arinoxDungeonLevel1.init.lua";
    end
end

function  Zones.arinoxDungeonLevel1.onEnterMap(x,y)
    --DawnInterface.setBackgroundMusic("res/music/ratsrats_0.ogg");
end

function  Zones.arinoxDungeonLevel1.closedDoorInteraction()
	if ( Quests.quest_hexmaster )
	then
		Quests.quest_hexmaster.magicDoorInteraction();
	end
end

function  Zones.arinoxDungeonLevel1.nextRegionInteraction();
	if ( Quests.quest_hexmaster )
	then
		Quests.quest_hexmaster.nextRegionInteraction();
	end
end

Zones.arinoxDungeonLevel1.init();