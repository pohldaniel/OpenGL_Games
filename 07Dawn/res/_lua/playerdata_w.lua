directions = {'n', 'ne', 'e', 'se', 's', 'sw', 'w', 'nw'}

TextureAtlasCreator:get():init(1664, 1024);

wizard = DawnInterface.createNewMobType("player_w")
for direction = 1,8 do
	for index=0,7,1 do
		wizard:addMoveTexture( Enums.Walking, direction, index, "res/character/wizard/walking "..directions[direction].."000"..index..".tga", 768, 768, true);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,12,1 do
		wizard:addMoveTexture( Enums.Attacking, direction, index, "res/character/wizard/attacking "..directions[direction].."000"..index..".tga", 1248, 768, true);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,12,1 do
		wizard:addMoveTexture( Enums.Casting, direction, index, "res/character/wizard/attacking "..directions[direction].."000"..index..".tga", 1248, 768, true);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,12,1 do
		wizard:addMoveTexture( Enums.Shooting, direction, index, "res/character/wizard/attacking "..directions[direction].."000"..index..".tga", 1248, 768, true);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,12,1 do
		wizard:addMoveTexture( Enums.Dying, direction, index, "res/character/wizard/dying "..directions[direction].."000"..index..".tga", 1664, 768, true, 16, 32);
	end
end
TextureAtlasCreator:get():addFrame();
TextureManager:SetTextureAtlas("player", TextureAtlasCreator:get():getAtlas());

wizard:calcNumMoveTexturesPerDirection();

player = Player:Get();
player:setBoundingBox( 18, 20, 64, 64 );
player:setMaxHealth(400);
player:setMaxMana(250);
player:setMaxFatigue(100);
player:setStrength(15);
player:setVitality(15);
player:setDexterity(20);
player:setWisdom(10);
player:setIntellect(10);
player:setHealthRegen(1);
player:setManaRegen(2);
player:setFatigueRegen( 5 );
player:giveCoins( 576 );
player:init(512, 400);
--TextureAtlasCreator:get():safeAtlas("tmp/player")