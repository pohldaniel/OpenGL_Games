directions = {'n', 'ne', 'e', 'se', 's', 'sw', 'w', 'nw'}

TextureAtlasCreator:get():init(1248, 782);

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
		wizard:addMoveTexture( Enums.Dying, direction, index, "res/character/wizard/dying "..directions[direction].."000"..index..".tga", 1248, 768, true, -16, -16, -12, -20);
	end
end
TextureAtlasCreator:get():addFrame();

wizard:calcNumMoveTexturesPerDirection();

ranger = DawnInterface.createNewMobType("player_r")

for index=0,7,1 do
	ranger:addMoveTexture( Enums.Walking, 1, index, "res/character/ranger/walking "..directions[1].."000"..index..".tga", 768, 780, true, 0, 0, 12, 0);
end

for direction = 2,8 do
	for index=0,7,1 do
		ranger:addMoveTexture( Enums.Walking, direction, index, "res/character/ranger/walking "..directions[direction].."000"..index..".tga", 768, 780, true);
	end
end
TextureAtlasCreator:get():addFrame();


for index=0,12,1 do
	ranger:addMoveTexture( Enums.Attacking, 1, index, "res/character/ranger/attacking "..directions[1].."000"..index..".tga", 1248, 780, true, 0, 0, 12, 0);
end

for direction = 2,8 do
	for index=0,12,1 do
		ranger:addMoveTexture( Enums.Attacking, direction, index, "res/character/ranger/attacking "..directions[direction].."000"..index..".tga", 1248, 864, true);
	end
end
TextureAtlasCreator:get():addFrame();


for index=0,12,1 do
	ranger:addMoveTexture( Enums.Casting, 1, index, "res/character/ranger/attacking "..directions[1].."000"..index..".tga", 1248, 780, true, 0, 0, 12, 0);
end


for direction = 2,8 do
	for index=0,12,1 do
		ranger:addMoveTexture( Enums.Casting, direction, index, "res/character/ranger/attacking "..directions[direction].."000"..index..".tga", 1248, 864, true);
	end
end
TextureAtlasCreator:get():addFrame();


for index=0,12,1 do
	ranger:addMoveTexture( Enums.Shooting, 1, index, "res/character/ranger/attacking "..directions[1].."000"..index..".tga", 1248, 780, true, 0, 0, 12, 0);
end

for direction = 2,8 do
	for index=0,12,1 do
		ranger:addMoveTexture( Enums.Shooting, direction, index, "res/character/ranger/attacking "..directions[direction].."000"..index..".tga", 1248, 864, true);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,10,1 do
		ranger:addMoveTexture( Enums.Dying, direction, index, "res/character/ranger/dying "..directions[direction].."000"..index..".tga", 1056, 768, true, -16, -16, -5, -27);
	end
end
TextureAtlasCreator:get():addFrame();

ranger:calcNumMoveTexturesPerDirection();


swordsman = DawnInterface.createNewMobType("player_s")

for index=0,7,1 do
	swordsman:addMoveTexture( Enums.Walking, 1, index, "res/character/swordsman/walking "..directions[1].."000"..index..".tga", 768, 782, true, 0, 0, 14, 0);
end

for direction = 2,8 do
	for index=0,7,1 do
		swordsman:addMoveTexture( Enums.Walking, direction, index, "res/character/swordsman/walking "..directions[direction].."000"..index..".tga", 768, 784);
	end
end
TextureAtlasCreator:get():addFrame();


for index=0,12,1 do
	swordsman:addMoveTexture( Enums.Attacking, 1, index, "res/character/swordsman/attacking "..directions[1].."000"..index..".tga", 1248, 782, true, 0, 0, 14, 0);
end

for direction = 2,8 do
	for index=0,12,1 do
		swordsman:addMoveTexture( Enums.Attacking, direction, index, "res/character/swordsman/attacking "..directions[direction].."000"..index..".tga", 1248, 784, true);
	end
end
TextureAtlasCreator:get():addFrame();

for index=0,12,1 do
	swordsman:addMoveTexture( Enums.Casting, 1, index, "res/character/swordsman/attacking "..directions[1].."000"..index..".tga", 1248, 782, true, 0, 0, 14, 0);
end

for direction = 2,8 do
	for index=0,12,1 do
		swordsman:addMoveTexture( Enums.Casting, direction, index, "res/character/swordsman/attacking "..directions[direction].."000"..index..".tga", 1248, 784, true);
	end
end
TextureAtlasCreator:get():addFrame();

for index=0,12,1 do
	swordsman:addMoveTexture( Enums.Shooting, 1, index, "res/character/swordsman/attacking "..directions[1].."000"..index..".tga", 1248, 782, true, 0, 0, 14, 0);
end

for direction = 2,8 do
	for index=0,12,1 do
		swordsman:addMoveTexture( Enums.Shooting, direction, index, "res/character/swordsman/attacking "..directions[direction].."000"..index..".tga", 1248, 782, true);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		swordsman:addMoveTexture( Enums.Dying, direction, index, "res/character/swordsman/dying "..directions[direction].."000"..index..".tga", 864, 768, true, -16, -16, -2, -30);
	end
end
TextureAtlasCreator:get():addFrame();
TextureManager:SetTextureAtlas("player", TextureAtlasCreator:get():getAtlas());

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
player:giveCoins( 576 );
player:init(512, 400);
--TextureAtlasCreator:get():safeAtlas("tmp/player")