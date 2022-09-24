directions = {'n', 'ne', 'e', 'se', 's', 'sw', 'w', 'nw'}

TextureAtlasCreator:get():init(864, 768);
wolfbase = DawnInterface.createNewMobType("Wolf");
wolfbase:setNumMoveTexturesPerDirection( ActivityType.Walking, 8 );
for direction = 1,8 do
	for index=0,7,1 do
		wolfbase:setMoveTexture( ActivityType.Walking, direction, index, "res/character/wolf/running "..directions[direction].."000"..index..".tga" );
	end
	TextureAtlasCreator:get():resetLine();
end
TextureAtlasCreator:get():addFrame();
wolfbase:setNumMoveTexturesPerDirection( ActivityType.Attacking, 9 );
for direction = 1,8 do
	for index=0,8,1 do
		wolfbase:setMoveTexture( ActivityType.Attacking, direction, index, "res/character/wolf/attacking "..directions[direction].."000"..index..".tga" );
	end
end
TextureAtlasCreator:get():addFrame();
wolfbase:setNumMoveTexturesPerDirection( ActivityType.Casting, 9 );
for direction = 1,8 do
	for index=0,8,1 do
		wolfbase:setMoveTexture( ActivityType.Casting, direction, index, "res/character/wolf/attacking "..directions[direction].."000"..index..".tga" );
	end
end
TextureAtlasCreator:get():addFrame();
wolfbase:setNumMoveTexturesPerDirection( ActivityType.Dying, 9 );
for direction = 1,8 do
	for index=0,8,1 do
		wolfbase:setMoveTexture( ActivityType.Dying, direction, index, "res/character/wolf/dying "..directions[direction].."000"..index..".tga" );
	end
end
TextureManager:SetTextureAtlas("Wolf", TextureAtlasCreator:get():getAtlas());