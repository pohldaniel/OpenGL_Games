directions = {'n', 'ne', 'e', 'se', 's', 'sw', 'w', 'nw'}

TextureAtlasCreator:get():init(864, 768);
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
TextureManager:SetTextureAtlas("mobs", TextureAtlasCreator:get():getAtlas());
--TextureAtlasCreator:get():safeAtlas("tmp/Wolf")

wolfbase:calcNumMoveTexturesPerDirection();
wolfbase:setStrength( 15 )
wolfbase:setDexterity( 25 )
wolfbase:setVitality( 15 )
wolfbase:setIntellect( 10 )
wolfbase:setWisdom( 10 )
wolfbase:setMaxHealth( 250 )
wolfbase:setMaxMana( 500 )
wolfbase:setMaxFatigue( 100 )
wolfbase:setHealthRegen( 0 )
wolfbase:setManaRegen( 50 )
wolfbase:setFatigueRegen( 25 )
wolfbase:setMinDamage( 5 )
wolfbase:setMaxDamage( 10 )
wolfbase:setArmor( 20 )
wolfbase:setDamageModifierPoints( 0 )
wolfbase:setHitModifierPoints( 0 )
wolfbase:setEvadeModifierPoints( 0 )
wolfbase:setName("A wolf")
wolfbase:setWanderRadius( 250 )
wolfbase:setLevel( 2 )
wolfbase:setClass( Enums.Warrior );
--wolfbase:inscribeSpellInSpellbook( spellDatabase["wolfbite"] );
--wolfbase:inscribeSpellInSpellbook( spellDatabase["rabidbite"] );
--wolfbase:addItemToLootTable( itemDatabase["patchofwolfskin"], 0.25 )
--wolfbase:addItemToLootTable( itemDatabase["wolfmeat"], 0.10 )
wolfbase:setExperienceValue( 25 );

wolfpup = DawnInterface.createNewMobType("Wolf_Pup")
wolfpup:baseOnType("Wolf")
wolfpup:modifyStrength( -2 )
wolfpup:modifyDexterity( -5 )
wolfpup:modifyVitality( -1 )
wolfpup:modifyIntellect( -5 )
wolfpup:modifyWisdom( -5 )
wolfpup:modifyMaxHealth( -100 )
wolfpup:setMinDamage( 3 )
wolfpup:setMaxDamage( 6 )
wolfpup:setArmor( 10 )
wolfpup:setName("A wolf pup")
wolfpup:setWanderRadius( 250 )
wolfpup:setLevel( 1 )
wolfpup:setExperienceValue( 15 );

giantwolf = DawnInterface.createNewMobType("Giant_Wolf")
giantwolf:baseOnType("Wolf")
giantwolf:modifyStrength( 10 )
giantwolf:modifyDexterity( 5 )
giantwolf:modifyVitality( 20 )
giantwolf:modifyIntellect( 10 )
giantwolf:modifyWisdom( 4 )
giantwolf:modifyMaxHealth( 150 )
giantwolf:setMinDamage( 10 )
giantwolf:setMaxDamage( 20 )
giantwolf:setArmor( 50 )
giantwolf:setName("A giant wolf")
giantwolf:setWanderRadius( 50 )
giantwolf:setLevel( 5 )
giantwolf:setExperienceValue( 40 );