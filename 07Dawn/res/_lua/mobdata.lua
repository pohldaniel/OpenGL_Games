TextureAtlasCreator:get():init(768, 864);
wolfbase = DawnInterface.createNewMobType("Wolf");
wolfbase:setNumMoveTexturesPerDirection( ActivityType.Walking, 8 );
for index=0,7,1 do
	wolfbase:setMoveTexture( ActivityType.Walking, N, index, "res/character/wolf/running n000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Walking, NW, index, "res/character/wolf/running nw000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Walking, W, index, "res/character/wolf/running w000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Walking, SW, index, "res/character/wolf/running sw000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Walking, S, index, "res/character/wolf/running s000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Walking, SE, index, "res/character/wolf/running se000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Walking, E, index, "res/character/wolf/running e000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Walking, NE, index, "res/character/wolf/running ne000"..index..".tga" );
end
wolfbase:setNumMoveTexturesPerDirection( ActivityType.Attacking, 9 );
for index=0,8,1 do
	wolfbase:setMoveTexture( ActivityType.Attacking, N, index, "res/character/wolf/attacking n000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Attacking, NW, index, "res/character/wolf/attacking nw000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Attacking, W, index, "res/character/wolf/attacking w000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Attacking, SW, index, "res/character/wolf/attacking sw000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Attacking, S, index, "res/character/wolf/attacking s000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Attacking, SE, index, "res/character/wolf/attacking se000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Attacking, E, index, "res/character/wolf/attacking e000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Attacking, NE, index, "res/character/wolf/attacking ne000"..index..".tga" );
end
wolfbase:setNumMoveTexturesPerDirection( ActivityType.Casting, 9 );
for index=0,8,1 do
	wolfbase:setMoveTexture( ActivityType.Casting, N, index, "res/character/wolf/attacking n000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Casting, NW, index, "res/character/wolf/attacking nw000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Casting, W, index, "res/character/wolf/attacking w000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Casting, SW, index, "res/character/wolf/attacking sw000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Casting, S, index, "res/character/wolf/attacking s000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Casting, SE, index, "res/character/wolf/attacking se000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Casting, E, index, "res/character/wolf/attacking e000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Casting, NE, index, "res/character/wolf/attacking ne000"..index..".tga" );
end
wolfbase:setNumMoveTexturesPerDirection( ActivityType.Dying, 9 );
for index=0,8,1 do
	wolfbase:setMoveTexture( ActivityType.Dying, N, index, "res/character/wolf/dying n000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Dying, NW, index, "res/character/wolf/dying nw000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Dying, W, index, "res/character/wolf/dying w000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Dying, SW, index, "res/character/wolf/dying sw000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Dying, S, index, "res/character/wolf/dying s000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Dying, SE, index, "res/character/wolf/dying se000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Dying, E, index, "res/character/wolf/dying e000"..index..".tga" );
	wolfbase:setMoveTexture( ActivityType.Dying, NE, index, "res/character/wolf/dying ne000"..index..".tga" );
end
TextureManager:SetTextureAtlas("Wolf", TextureAtlasCreator:get():getAtlas());