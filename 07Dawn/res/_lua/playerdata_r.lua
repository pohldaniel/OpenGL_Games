directions = {'n', 'ne', 'e', 'se', 's', 'sw', 'w', 'nw'}

TextureAtlasCreator:Get():init("player", 1248, 864);

ranger = DawnInterface.createNewMobType("player_r")

for index=0,7,1 do
	ranger:addMoveTexture( Enums.Walking, 1, index, "res/character/ranger/walking "..directions[1].."000"..index..".tga", 768, 780, 0, 0, 12, 0);
end

for direction = 2,8 do
	for index=0,7,1 do
		ranger:addMoveTexture( Enums.Walking, direction, index, "res/character/ranger/walking "..directions[direction].."000"..index..".tga", 768, 780);
	end
end
TextureAtlasCreator:Get():addFrame();


for index=0,12,1 do
	ranger:addMoveTexture( Enums.Attacking, 1, index, "res/character/ranger/attacking "..directions[1].."000"..index..".tga", 1248, 780, 0, 0, 12, 0);
end

for direction = 2,8 do
	for index=0,12,1 do
		ranger:addMoveTexture( Enums.Attacking, direction, index, "res/character/ranger/attacking "..directions[direction].."000"..index..".tga", 1248, 864);
	end
end
TextureAtlasCreator:Get():addFrame();


for index=0,12,1 do
	ranger:addMoveTexture( Enums.Casting, 1, index, "res/character/ranger/attacking "..directions[1].."000"..index..".tga", 1248, 780, 0, 0, 12, 0);
end


for direction = 2,8 do
	for index=0,12,1 do
		ranger:addMoveTexture( Enums.Casting, direction, index, "res/character/ranger/attacking "..directions[direction].."000"..index..".tga", 1248, 864);
	end
end
TextureAtlasCreator:Get():addFrame();


for index=0,12,1 do
	ranger:addMoveTexture( Enums.Shooting, 1, index, "res/character/ranger/attacking "..directions[1].."000"..index..".tga", 1248, 780, 0, 0, 12, 0);
end

for direction = 2,8 do
	for index=0,12,1 do
		ranger:addMoveTexture( Enums.Shooting, direction, index, "res/character/ranger/attacking "..directions[direction].."000"..index..".tga", 1248, 864);
	end
end
TextureAtlasCreator:Get():addFrame();

for direction = 1,8 do
	for index=0,10,1 do
		ranger:addMoveTexture( Enums.Dying, direction, index, "res/character/ranger/dying "..directions[direction].."000"..index..".tga", 1056, 768, -16, -16, -5, -27);
	end
end
TextureAtlasCreator:Get():addFrame();

ranger:calcNumMoveTexturesPerDirection();


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
player:giveCoins( 48576 );
player:setClass(Enums.Ranger);