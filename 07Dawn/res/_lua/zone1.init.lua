zone1 = DawnInterface.getCurrentZone();
zone1.leavePoint = DawnInterface.addInteractionPoint();
zone1.leavePoint:setPosition( 747, 1588, 56, 80 );
zone1.leavePoint:setInteractionType( Enums.Zone );
zone1.leavePoint:setBackgroundTexture( "res/transparent2x2pixel.png", true);
zone1.leavePoint:setInteractionCode( 'DawnInterface.enterZone( "res/_lua/arinoxGeneralShop", -158, 0 );\nDawnInterface.setSavingAllowed( true );' );
--DawnInterface.enterZone( "res/_lua/arinoxGeneralShop", -158, 0 );
--DawnInterface.setSavingAllowed( true );

print("Init Quests")
dofile("res/_lua/quests_wood.lua");
dofile("res/_lua/quests_hexmaster.lua");
dofile("res/_lua/quests_venomousveins.lua");

