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
--TextureManager:SetTextureAtlas("Wolf", TextureAtlasCreator:get():getAtlas());

--TextureAtlasCreator.get(TextureAtlasCreator):init(768, 864);
witchbase = DawnInterface.createNewMobType("Witch")
witchbase:setNumMoveTexturesPerDirection( ActivityType.Walking, 8 );
for index=0,7,1 do
	witchbase:setMoveTexture( ActivityType.Walking, N, index, "res/character/witch/walking n000"..index..".tga" );
	witchbase:setMoveTexture( ActivityType.Walking, NW, index, "res/character/witch/walking nw000"..index..".tga" );
	witchbase:setMoveTexture( ActivityType.Walking, W, index, "res/character/witch/walking w000"..index..".tga" );
	witchbase:setMoveTexture( ActivityType.Walking, SW, index, "res/character/witch/walking sw000"..index..".tga" );
	witchbase:setMoveTexture( ActivityType.Walking, S, index, "res/character/witch/walking s000"..index..".tga" );
	witchbase:setMoveTexture( ActivityType.Walking, SE, index, "res/character/witch/walking se000"..index..".tga" );
	witchbase:setMoveTexture( ActivityType.Walking, E, index, "res/character/witch/walking e000"..index..".tga" );
	witchbase:setMoveTexture( ActivityType.Walking, NE, index, "res/character/witch/walking ne000"..index..".tga" );
end

witchbase:setNumMoveTexturesPerDirection( ActivityType.Dying, 8 );
for index=0,7,1 do
	witchbase:setMoveTexture( ActivityType.Dying, N, index, "res/character/witch/walking n000"..index..".tga", -16, -16 );
	witchbase:setMoveTexture( ActivityType.Dying, NW, index, "res/character/witch/walking nw000"..index..".tga", -16, -16 );
	witchbase:setMoveTexture( ActivityType.Dying, W, index, "res/character/witch/walking w000"..index..".tga", -16, -16 );
	witchbase:setMoveTexture( ActivityType.Dying, SW, index, "res/character/witch/walking sw000"..index..".tga", -16, -16 );
	witchbase:setMoveTexture( ActivityType.Dying, S, index, "res/character/witch/walking s000"..index..".tga", -16, -16 );
	witchbase:setMoveTexture( ActivityType.Dying, SE, index, "res/character/witch/walking se000"..index..".tga", -16, -16 );
	witchbase:setMoveTexture( ActivityType.Dying, E, index, "res/character/witch/walking e000"..index..".tga", -16, -16 );
	witchbase:setMoveTexture( ActivityType.Dying, NE, index, "res/character/witch/walking ne000"..index..".tga", -16, -16 );
end
--TextureManager:SetTextureAtlas("Witch", TextureAtlasCreator:get():getAtlas());

--TextureAtlasCreator.get(TextureAtlasCreator):init(768, 864);
humanbase = DawnInterface.createNewMobType("Human")
humanbase:setNumMoveTexturesPerDirection( ActivityType.Walking, 8 );
for index=0,7,1 do
	humanbase:setMoveTexture( ActivityType.Walking, N, index, "res/character/swordsman/walking n000"..index..".tga" );
	humanbase:setMoveTexture( ActivityType.Walking, NW, index, "res/character/swordsman/walking nw000"..index..".tga" );
	humanbase:setMoveTexture( ActivityType.Walking, W, index, "res/character/swordsman/walking w000"..index..".tga" );
	humanbase:setMoveTexture( ActivityType.Walking, SW, index, "res/character/swordsman/walking sw000"..index..".tga" );
	humanbase:setMoveTexture( ActivityType.Walking, S, index, "res/character/swordsman/walking s000"..index..".tga" );
	humanbase:setMoveTexture( ActivityType.Walking, SE, index, "res/character/swordsman/walking se000"..index..".tga" );
	humanbase:setMoveTexture( ActivityType.Walking, E, index, "res/character/swordsman/walking e000"..index..".tga" );
	humanbase:setMoveTexture( ActivityType.Walking, NE, index, "res/character/swordsman/walking ne000"..index..".tga" );
end

humanbase:setNumMoveTexturesPerDirection( ActivityType.Dying, 9 );
for index=0,8,1 do
	humanbase:setMoveTexture( ActivityType.Dying, N, index, "res/character/swordsman/dying n000"..index..".tga", -16, -16 );
	humanbase:setMoveTexture( ActivityType.Dying, NW, index, "res/character/swordsman/dying nw000"..index..".tga", -16, -16 );
	humanbase:setMoveTexture( ActivityType.Dying, W, index, "res/character/swordsman/dying w000"..index..".tga", -16, -16 );
	humanbase:setMoveTexture( ActivityType.Dying, SW, index, "res/character/swordsman/dying sw000"..index..".tga", -16, -16 );
	humanbase:setMoveTexture( ActivityType.Dying, S, index, "res/character/swordsman/dying s000"..index..".tga", -16, -16 );
	humanbase:setMoveTexture( ActivityType.Dying, SE, index, "res/character/swordsman/dying se000"..index..".tga", -16, -16 );
	humanbase:setMoveTexture( ActivityType.Dying, E, index, "res/character/swordsman/dying e000"..index..".tga", -16, -16 );
	humanbase:setMoveTexture( ActivityType.Dying, NE, index, "res/character/swordsman/dying ne000"..index..".tga", -16, -16 );
end
--TextureManager:SetTextureAtlas("Human", TextureAtlasCreator:get():getAtlas());

--TextureAtlasCreator.get(TextureAtlasCreator):init(768, 864);
wizardbase = DawnInterface.createNewMobType("Wizard")
wizardbase:setNumMoveTexturesPerDirection( ActivityType.Walking, 8 );
for index=0,7,1 do
	wizardbase:setMoveTexture( ActivityType.Walking, N, index, "res/character/wizard/walking n000"..index..".tga" );
	wizardbase:setMoveTexture( ActivityType.Walking, NW, index, "res/character/wizard/walking nw000"..index..".tga" );
	wizardbase:setMoveTexture( ActivityType.Walking, W, index, "res/character/wizard/walking w000"..index..".tga" );
	wizardbase:setMoveTexture( ActivityType.Walking, SW, index, "res/character/wizard/walking sw000"..index..".tga" );
	wizardbase:setMoveTexture( ActivityType.Walking, S, index, "res/character/wizard/walking s000"..index..".tga" );
	wizardbase:setMoveTexture( ActivityType.Walking, SE, index, "res/character/wizard/walking se000"..index..".tga" );
	wizardbase:setMoveTexture( ActivityType.Walking, E, index, "res/character/wizard/walking e000"..index..".tga" );
	wizardbase:setMoveTexture( ActivityType.Walking, NE, index, "res/character/wizard/walking ne000"..index..".tga" );
end
wizardbase:setNumMoveTexturesPerDirection( ActivityType.Dying, 13 );
for index=0,12,1 do
	wizardbase:setMoveTexture( ActivityType.Dying, N, index, "res/character/wizard/dying n000"..index..".tga", -16, -16 );
	wizardbase:setMoveTexture( ActivityType.Dying, NW, index, "res/character/wizard/dying nw000"..index..".tga", -16, -16 );
	wizardbase:setMoveTexture( ActivityType.Dying, W, index, "res/character/wizard/dying w000"..index..".tga", -16, -16 );
	wizardbase:setMoveTexture( ActivityType.Dying, SW, index, "res/character/wizard/dying sw000"..index..".tga", -16, -16 );
	wizardbase:setMoveTexture( ActivityType.Dying, S, index, "res/character/wizard/dying s000"..index..".tga", -16, -16 );
	wizardbase:setMoveTexture( ActivityType.Dying, SE, index, "res/character/wizard/dying se000"..index..".tga", -16, -16 );
	wizardbase:setMoveTexture( ActivityType.Dying, E, index, "res/character/wizard/dying e000"..index..".tga", -16, -16 );
	wizardbase:setMoveTexture( ActivityType.Dying, NE, index, "res/character/wizard/dying ne000"..index..".tga", -16, -16 );
end
--TextureManager:SetTextureAtlas("Wizard", TextureAtlasCreator:get():getAtlas());

--TextureAtlasCreator.get(TextureAtlasCreator):init(768, 864);
spiderlingbase = DawnInterface.createNewMobType("Spiderling")
spiderlingbase:setNumMoveTexturesPerDirection( ActivityType.Walking, 8 );
for index=0,7,1 do
	spiderlingbase:setMoveTexture( ActivityType.Walking, N, index, "res/character/spiderling/walking n000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Walking, NW, index, "res/character/spiderling/walking nw000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Walking, W, index, "res/character/spiderling/walking w000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Walking, SW, index, "res/character/spiderling/walking sw000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Walking, S, index, "res/character/spiderling/walking s000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Walking, SE, index, "res/character/spiderling/walking se000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Walking, E, index, "res/character/spiderling/walking e000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Walking, NE, index, "res/character/spiderling/walking ne000"..index..".tga" );
end

spiderlingbase:setNumMoveTexturesPerDirection( ActivityType.Attacking, 9 );
for index=0,8,1 do
	spiderlingbase:setMoveTexture( ActivityType.Attacking, N, index, "res/character/spiderling/attacking n000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Attacking, NW, index, "res/character/spiderling/attacking nw000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Attacking, W, index, "res/character/spiderling/attacking w000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Attacking, SW, index, "res/character/spiderling/attacking sw000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Attacking, S, index, "res/character/spiderling/attacking s000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Attacking, SE, index, "res/character/spiderling/attacking se000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Attacking, E, index, "res/character/spiderling/attacking e000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Attacking, NE, index, "res/character/spiderling/attacking ne000"..index..".tga" );
end

spiderlingbase:setNumMoveTexturesPerDirection( ActivityType.Shooting, 9 );
for index=0,8,1 do
	spiderlingbase:setMoveTexture( ActivityType.Shooting, N, index, "res/character/spiderling/attacking n000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Shooting, NW, index, "res/character/spiderling/attacking nw000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Shooting, W, index, "res/character/spiderling/attacking w000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Shooting, SW, index, "res/character/spiderling/attacking sw000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Shooting, S, index, "res/character/spiderling/attacking s000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Shooting, SE, index, "res/character/spiderling/attacking se000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Shooting, E, index, "res/character/spiderling/attacking e000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Shooting, NE, index, "res/character/spiderling/attacking ne000"..index..".tga" );
end

spiderlingbase:setNumMoveTexturesPerDirection( ActivityType.Dying, 11 );
for index=0,10,1 do
	spiderlingbase:setMoveTexture( ActivityType.Dying, N, index, "res/character/spiderling/dying n000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Dying, NW, index, "res/character/spiderling/dying nw000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Dying, W, index, "res/character/spiderling/dying w000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Dying, SW, index, "res/character/spiderling/dying sw000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Dying, S, index, "res/character/spiderling/dying s000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Dying, SE, index, "res/character/spiderling/dying se000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Dying, E, index, "res/character/spiderling/dying e000"..index..".tga" );
	spiderlingbase:setMoveTexture( ActivityType.Dying, NE, index, "res/character/spiderling/dying ne000"..index..".tga" );
end
--TextureManager:SetTextureAtlas("Spiderling", TextureAtlasCreator:get():getAtlas());

--TextureAtlasCreator.get(TextureAtlasCreator):init(768, 864);
spiderbase = DawnInterface.createNewMobType("Spider")
spiderbase:setNumMoveTexturesPerDirection( ActivityType.Walking, 8 );
for index=0,7,1 do
	spiderbase:setMoveTexture( ActivityType.Walking, N, index, "res/character/spider/walking n000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Walking, NW, index, "res/character/spider/walking nw000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Walking, W, index, "res/character/spider/walking w000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Walking, SW, index, "res/character/spider/walking sw000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Walking, S, index, "res/character/spider/walking s000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Walking, SE, index, "res/character/spider/walking se000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Walking, E, index, "res/character/spider/walking e000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Walking, NE, index, "res/character/spider/walking ne000"..index..".tga" );
end

spiderbase:setNumMoveTexturesPerDirection( ActivityType.Attacking, 9 );
for index=0,8,1 do
	spiderbase:setMoveTexture( ActivityType.Attacking, N, index, "res/character/spider/attacking n000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Attacking, NW, index, "res/character/spider/attacking nw000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Attacking, W, index, "res/character/spider/attacking w000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Attacking, SW, index, "res/character/spider/attacking sw000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Attacking, S, index, "res/character/spider/attacking s000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Attacking, SE, index, "res/character/spider/attacking se000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Attacking, E, index, "res/character/spider/attacking e000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Attacking, NE, index, "res/character/spider/attacking ne000"..index..".tga" );
end

spiderbase:setNumMoveTexturesPerDirection( ActivityType.Casting, 9 );
for index=0,8,1 do
	spiderbase:setMoveTexture( ActivityType.Casting, N, index, "res/character/spider/attacking n000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Casting, NW, index, "res/character/spider/attacking nw000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Casting, W, index, "res/character/spider/attacking w000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Casting, SW, index, "res/character/spider/attacking sw000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Casting, S, index, "res/character/spider/attacking s000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Casting, SE, index, "res/character/spider/attacking se000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Casting, E, index, "res/character/spider/attacking e000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Casting, NE, index, "res/character/spider/attacking ne000"..index..".tga" );
end

spiderbase:setNumMoveTexturesPerDirection( ActivityType.Dying, 11 );
for index=0,10,1 do
	spiderbase:setMoveTexture( ActivityType.Dying, N, index, "res/character/spider/dying n000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Dying, NW, index, "res/character/spider/dying nw000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Dying, W, index, "res/character/spider/dying w000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Dying, SW, index, "res/character/spider/dying sw000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Dying, S, index, "res/character/spider/dying s000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Dying, SE, index, "res/character/spider/dying se000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Dying, E, index, "res/character/spider/dying e000"..index..".tga" );
	spiderbase:setMoveTexture( ActivityType.Dying, NE, index, "res/character/spider/dying ne000"..index..".tga" );
end
--TextureManager:SetTextureAtlas("Spider", TextureAtlasCreator:get():getAtlas());

--TextureAtlasCreator.get(TextureAtlasCreator):init(768, 864);
skeletonbase = DawnInterface.createNewMobType("Skeleton")
skeletonbase:setNumMoveTexturesPerDirection( ActivityType.Walking, 9 );
for index=0,8,1 do
	skeletonbase:setMoveTexture( ActivityType.Walking, N, index, "res/character/skeleton/walking n000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Walking, NW, index, "res/character/skeleton/walking nw000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Walking, W, index, "res/character/skeleton/walking w000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Walking, SW, index, "res/character/skeleton/walking sw000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Walking, S, index, "res/character/skeleton/walking s000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Walking, SE, index, "res/character/skeleton/walking se000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Walking, E, index, "res/character/skeleton/walking e000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Walking, NE, index, "res/character/skeleton/walking ne000"..index..".tga" );
end

skeletonbase:setNumMoveTexturesPerDirection( ActivityType.Attacking, 10 );
for index=0,9,1 do
	skeletonbase:setMoveTexture( ActivityType.Attacking, N, index, "res/character/skeleton/attacking n000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Attacking, NW, index, "res/character/skeleton/attacking nw000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Attacking, W, index, "res/character/skeleton/attacking w000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Attacking, SW, index, "res/character/skeleton/attacking sw000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Attacking, S, index, "res/character/skeleton/attacking s000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Attacking, SE, index, "res/character/skeleton/attacking se000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Attacking, E, index, "res/character/skeleton/attacking e000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Attacking, NE, index, "res/character/skeleton/attacking ne000"..index..".tga" );
end

skeletonbase:setNumMoveTexturesPerDirection( ActivityType.Casting, 10 );
for index=0,9,1 do
	skeletonbase:setMoveTexture( ActivityType.Casting, N, index, "res/character/skeleton/attacking n000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Casting, NW, index, "res/character/skeleton/attacking nw000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Casting, W, index, "res/character/skeleton/attacking w000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Casting, SW, index, "res/character/skeleton/attacking sw000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Casting, S, index, "res/character/skeleton/attacking s000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Casting, SE, index, "res/character/skeleton/attacking se000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Casting, E, index, "res/character/skeleton/attacking e000"..index..".tga" );
	skeletonbase:setMoveTexture( ActivityType.Casting, NE, index, "res/character/skeleton/attacking ne000"..index..".tga" );
end
--TextureManager:SetTextureAtlas("Skeleton", TextureAtlasCreator:get():getAtlas());

--TextureAtlasCreator.get(TextureAtlasCreator):init(768, 864);
skeletonarcherbase = DawnInterface.createNewMobType("Skeleton Archer")
skeletonarcherbase:setNumMoveTexturesPerDirection( ActivityType.Walking, 8 );
for index=0,7,1 do
	skeletonarcherbase:setMoveTexture( ActivityType.Walking, N, index, "res/character/skeletonarcher/walking n000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Walking, NW, index, "res/character/skeletonarcher/walking nw000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Walking, W, index, "res/character/skeletonarcher/walking w000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Walking, SW, index, "res/character/skeletonarcher/walking sw000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Walking, S, index, "res/character/skeletonarcher/walking s000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Walking, SE, index, "res/character/skeletonarcher/walking se000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Walking, E, index, "res/character/skeletonarcher/walking e000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Walking, NE, index, "res/character/skeletonarcher/walking ne000"..index..".tga" );
end

skeletonarcherbase:setNumMoveTexturesPerDirection( ActivityType.Attacking, 13 );
for index=0,12,1 do
	skeletonarcherbase:setMoveTexture( ActivityType.Attacking, N, index, "res/character/skeletonarcher/attacking n000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Attacking, NW, index, "res/character/skeletonarcher/attacking nw000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Attacking, W, index, "res/character/skeletonarcher/attacking w000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Attacking, SW, index, "res/character/skeletonarcher/attacking sw000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Attacking, S, index, "res/character/skeletonarcher/attacking s000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Attacking, SE, index, "res/character/skeletonarcher/attacking se000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Attacking, E, index, "res/character/skeletonarcher/attacking e000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Attacking, NE, index, "res/character/skeletonarcher/attacking ne000"..index..".tga" );
end

skeletonarcherbase:setNumMoveTexturesPerDirection( ActivityType.Casting, 13 );
for index=0,12,1 do
	skeletonarcherbase:setMoveTexture( ActivityType.Casting, N, index, "res/character/skeletonarcher/attacking n000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Casting, NW, index, "res/character/skeletonarcher/attacking nw000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Casting, W, index, "res/character/skeletonarcher/attacking w000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Casting, SW, index, "res/character/skeletonarcher/attacking sw000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Casting, S, index, "res/character/skeletonarcher/attacking s000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Casting, SE, index, "res/character/skeletonarcher/attacking se000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Casting, E, index, "res/character/skeletonarcher/attacking e000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Casting, NE, index, "res/character/skeletonarcher/attacking ne000"..index..".tga" );
end

skeletonarcherbase:setNumMoveTexturesPerDirection( ActivityType.Shooting, 13 );
for index=0,12,1 do
	skeletonarcherbase:setMoveTexture( ActivityType.Shooting, N, index, "res/character/skeletonarcher/attacking n000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Shooting, NW, index, "res/character/skeletonarcher/attacking nw000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Shooting, W, index, "res/character/skeletonarcher/attacking w000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Shooting, SW, index, "res/character/skeletonarcher/attacking sw000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Shooting, S, index, "res/character/skeletonarcher/attacking s000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Shooting, SE, index, "res/character/skeletonarcher/attacking se000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Shooting, E, index, "res/character/skeletonarcher/attacking e000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Shooting, NE, index, "res/character/skeletonarcher/attacking ne000"..index..".tga" );
end

skeletonarcherbase:setNumMoveTexturesPerDirection( ActivityType.Dying, 9 );
for index=0,8,1 do
	skeletonarcherbase:setMoveTexture( ActivityType.Dying, N, index, "res/character/skeletonarcher/dying n000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Dying, NW, index, "res/character/skeletonarcher/dying nw000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Dying, W, index, "res/character/skeletonarcher/dying w000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Dying, SW, index, "res/character/skeletonarcher/dying sw000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Dying, S, index, "res/character/skeletonarcher/dying s000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Dying, SE, index, "res/character/skeletonarcher/dying se000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Dying, E, index, "res/character/skeletonarcher/dying e000"..index..".tga" );
	skeletonarcherbase:setMoveTexture( ActivityType.Dying, NE, index, "res/character/skeletonarcher/dying ne000"..index..".tga" );
end
TextureManager:SetTextureAtlas("Skeleton_Archer", TextureAtlasCreator:get():getAtlas());