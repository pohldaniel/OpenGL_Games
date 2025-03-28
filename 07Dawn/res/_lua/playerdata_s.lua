directions = {'n', 'ne', 'e', 'se', 's', 'sw', 'w', 'nw'}

TextureAtlasCreator:Get():init("player", 1248, 784);

swordsman = DawnInterface.createNewMobType("player_s")

for index=0,7,1 do
	swordsman:addMoveTexture( Enums.Walking, 1, index, "res/character/swordsman/walking "..directions[1].."000"..index..".tga", 768, 782, 0, 0, 14, 0);
end

for direction = 2,8 do
	for index=0,7,1 do
		swordsman:addMoveTexture( Enums.Walking, direction, index, "res/character/swordsman/walking "..directions[direction].."000"..index..".tga", 768, 784);
	end
end
TextureAtlasCreator:Get():addFrame();


for index=0,12,1 do
	swordsman:addMoveTexture( Enums.Attacking, 1, index, "res/character/swordsman/attacking "..directions[1].."000"..index..".tga", 1248, 782, 0, 0, 14, 0);
end

for direction = 2,8 do
	for index=0,12,1 do
		swordsman:addMoveTexture( Enums.Attacking, direction, index, "res/character/swordsman/attacking "..directions[direction].."000"..index..".tga", 1248, 784);
	end
end
TextureAtlasCreator:Get():addFrame();

for index=0,12,1 do
	swordsman:addMoveTexture( Enums.Casting, 1, index, "res/character/swordsman/attacking "..directions[1].."000"..index..".tga", 1248, 782, 0, 0, 14, 0);
end

for direction = 2,8 do
	for index=0,12,1 do
		swordsman:addMoveTexture( Enums.Casting, direction, index, "res/character/swordsman/attacking "..directions[direction].."000"..index..".tga", 1248, 784);
	end
end
TextureAtlasCreator:Get():addFrame();

for index=0,12,1 do
	swordsman:addMoveTexture( Enums.Shooting, 1, index, "res/character/swordsman/attacking "..directions[1].."000"..index..".tga", 1248, 782, 0, 0, 14, 0);
end

for direction = 2,8 do
	for index=0,12,1 do
		swordsman:addMoveTexture( Enums.Shooting, direction, index, "res/character/swordsman/attacking "..directions[direction].."000"..index..".tga", 1248, 782);
	end
end
TextureAtlasCreator:Get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		swordsman:addMoveTexture( Enums.Dying, direction, index, "res/character/swordsman/dying "..directions[direction].."000"..index..".tga", 864, 768, -16, -16, -2, -30);
	end
end
TextureAtlasCreator:Get():addFrame();
TextureManager:SetTextureAtlas(TextureAtlasCreator:Get():getName(), TextureAtlasCreator:Get():getAtlas());

swordsman:calcNumMoveTexturesPerDirection();

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
player:setClass(Enums.Warrior);