function Zones.zone2.init()
	if(not Zones.zone2.inited) then
		InteractionPoints.zone2.leavePoint = DawnInterface.addInteractionPoint();
		InteractionPoints.zone2.leavePoint:setPosition( -670, 1568, 48, 70 );
		InteractionPoints.zone2.leavePoint:setBackgroundTexture("res/transparent4x4pixel.png", true);
		InteractionPoints.zone2.leavePoint:setInteractionType(Enums.Zone);
		InteractionPoints.zone2.leavePoint:setInteractionCode("Zones.zone2.normalLeavePoint()");

		InteractionRegions.zone2.enterRegion = DawnInterface.addInteractionRegion();
		InteractionRegions.zone2.enterRegion:setPosition( -800, 1400, 300, 300 );
		InteractionRegions.zone2.enterRegion:setOnEnterText("Zones.zone2.onEnterRegion()");

		InteractionPoints.zone2.lockedDoor = DawnInterface.addInteractionPoint();
		InteractionPoints.zone2.lockedDoor:setPosition( -1400, 1250, 60, 100 );
		InteractionPoints.zone2.lockedDoor:setBackgroundTexture("res/transparent4x4pixel.png", true);
		InteractionPoints.zone2.lockedDoor:setInteractionType(Enums.Zone);
		InteractionPoints.zone2.lockedDoor:setInteractionCode("Zones.zone2.lockedDoorInteraction()");

		Zones.zone2.inited = true;
		Zones.zone2.path = "res/_lua/zone2.init.lua";
	end
end

function Zones.zone2.onEnterRegion()
	if (Quests.quest_hexmaster)
	then
		Quests.quest_hexmaster.onEnteredUndergroundRegion()
	end
end

function Zones.zone2.normalLeavePoint()
	if (Quests.quest_hexmaster)
	then
		Quests.quest_hexmaster.onLeaveLaboratory();
	end
end

function Zones.zone2.lockedDoorInteraction()
	if (Quests.quest_hexmaster)
	then
		Quests.quest_hexmaster.onEnterHiddenChamber();
	end
end

function Zones.zone2.onEnterMap(x,y)
 
end

Zones.zone2.init()