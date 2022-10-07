directions = {'n', 'ne', 'e', 'se', 's', 'sw', 'w', 'nw'}

TextureAtlasCreator:get():init(1664, 1024);
wolfbase = DawnInterface.createNewMobType("Wolf");

for direction = 1,8 do
	for index=0,7,1 do
		wolfbase:addMoveTexture( Enums.Walking, direction, index, "res/character/wolf/running "..directions[direction].."000"..index..".tga", 786, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		wolfbase:addMoveTexture( Enums.Attacking, direction, index, "res/character/wolf/attacking "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		wolfbase:addMoveTexture( Enums.Casting, direction, index, "res/character/wolf/attacking "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		wolfbase:addMoveTexture( Enums.Dying, direction, index, "res/character/wolf/dying "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:get():addFrame();



witchbase = DawnInterface.createNewMobType("Witch")
for direction = 1,8 do
	for index=0,7,1 do
		witchbase:addMoveTexture( Enums.Walking, direction, index, "res/character/witch/walking "..directions[direction].."000"..index..".tga", 768, 768);		
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,7,1 do
		witchbase:addMoveTexture( Enums.Dying, direction, index, "res/character/witch/walking "..directions[direction].."000"..index..".tga", 768, 768);		
	end
end
TextureAtlasCreator:get():addFrame();

humanbase = DawnInterface.createNewMobType("Human")
for direction = 1,8 do
	for index=0,7,1 do
		humanbase:addMoveTexture( Enums.Walking, direction, index, "res/character/swordsman/walking "..directions[direction].."000"..index..".tga", 768, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		humanbase:addMoveTexture( Enums.Dying, direction, index, "res/character/swordsman/dying "..directions[direction].."000"..index..".tga", 1152, 1024);
	end
end
TextureAtlasCreator:get():addFrame();

wizardbase = DawnInterface.createNewMobType("Wizard")
for direction = 1,8 do
	for index=0,7,1 do
		wizardbase:addMoveTexture( Enums.Walking, direction, index, "res/character/wizard/walking "..directions[direction].."000"..index..".tga", 768, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,12,1 do
		wizardbase:addMoveTexture( Enums.Dying, direction, index, "res/character/wizard/dying "..directions[direction].."000"..index..".tga", 1664, 1024);
	end
end
TextureAtlasCreator:get():addFrame();

spiderlingbase = DawnInterface.createNewMobType("Spiderling")
for direction = 1,8 do
	for index=0,7,1 do
		spiderlingbase:addMoveTexture( Enums.Walking, direction, index, "res/character/spiderling/walking "..directions[direction].."000"..index..".tga", 384, 528);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		spiderlingbase:addMoveTexture( Enums.Attacking, direction, index, "res/character/spiderling/attacking "..directions[direction].."000"..index..".tga", 432, 528);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		spiderlingbase:addMoveTexture( Enums.Shooting, direction, index, "res/character/spiderling/attacking "..directions[direction].."000"..index..".tga", 432, 528);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,10,1 do
		spiderlingbase:addMoveTexture( Enums.Dying, direction, index, "res/character/spiderling/dying "..directions[direction].."000"..index..".tga", 528, 528);
	end
end
TextureAtlasCreator:get():addFrame();

spider = DawnInterface.createNewMobType("Spider")
for direction = 1,8 do
	for index=0,7,1 do
		spider:addMoveTexture( Enums.Walking, direction, index, "res/character/spider/walking "..directions[direction].."000"..index..".tga", 768, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		spider:addMoveTexture( Enums.Attacking, direction, index, "res/character/spider/attacking "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		spider:addMoveTexture( Enums.Shooting, direction, index, "res/character/spider/attacking "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,10,1 do
		spider:addMoveTexture( Enums.Dying, direction, index, "res/character/spider/dying "..directions[direction].."000"..index..".tga", 1056, 768);
	end
end
TextureAtlasCreator:get():addFrame();

skeletonbase = DawnInterface.createNewMobType("Skeleton")
for direction = 1,8 do
	for index=0,8,1 do
		skeletonbase:addMoveTexture( Enums.Walking, direction, index, "res/character/skeleton/walking "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,9,1 do
		skeletonbase:addMoveTexture( Enums.Attacking, direction, index, "res/character/skeleton/attacking "..directions[direction].."000"..index..".tga", 960, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,9,1 do
		skeletonbase:addMoveTexture( Enums.Casting, direction, index, "res/character/skeleton/attacking "..directions[direction].."000"..index..".tga", 960, 768);
	end
end
TextureAtlasCreator:get():addFrame();

skeletonarcherbase = DawnInterface.createNewMobType("Skeleton_Archer")
for direction = 1,8 do
	for index=0,7,1 do
		skeletonarcherbase:addMoveTexture( Enums.Walking, direction, index, "res/character/skeletonarcher/walking "..directions[direction].."000"..index..".tga", 768, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,12,1 do
		skeletonarcherbase:addMoveTexture( Enums.Attacking, direction, index, "res/character/skeletonarcher/attacking "..directions[direction].."000"..index..".tga", 1248, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,12,1 do
		skeletonarcherbase:addMoveTexture( Enums.Casting, direction, index, "res/character/skeletonarcher/attacking "..directions[direction].."000"..index..".tga", 1248, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,12,1 do
		skeletonarcherbase:addMoveTexture( Enums.Shooting, direction, index, "res/character/skeletonarcher/attacking "..directions[direction].."000"..index..".tga", 1248, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		skeletonarcherbase:addMoveTexture( Enums.Dying, direction, index, "res/character/skeletonarcher/dying "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:get():addFrame();
TextureManager:SetTextureAtlas("mobs", TextureAtlasCreator:get():getAtlas());
--TextureAtlasCreator:get():safeAtlas("tmp/mobs")