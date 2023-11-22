directions = {'n', 'ne', 'e', 'se', 's', 'sw', 'w', 'nw'}

TextureAtlasCreator:Get():init("mobs", 1664, 1024);
wolfbase = DawnInterface.createNewMobType("Wolf");

for direction = 1,8 do
	for index=0,7,1 do
		wolfbase:addMoveTexture( Enums.Walking, direction, index, "res/character/wolf/running "..directions[direction].."000"..index..".tga", 786, 768);
	end
end
TextureAtlasCreator:Get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		wolfbase:addMoveTexture( Enums.Attacking, direction, index, "res/character/wolf/attacking "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:Get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		wolfbase:addMoveTexture( Enums.Casting, direction, index, "res/character/wolf/attacking "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:Get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		wolfbase:addMoveTexture( Enums.Dying, direction, index, "res/character/wolf/dying "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:Get():addFrame();

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

humanbase = DawnInterface.createNewMobType("Human")
for direction = 1,8 do
	for index=0,7,1 do
		humanbase:addMoveTexture( Enums.Walking, direction, index, "res/character/swordsman/walking "..directions[direction].."000"..index..".tga", 768, 768);
	end
end
TextureAtlasCreator:Get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		humanbase:addMoveTexture( Enums.Dying, direction, index, "res/character/swordsman/dying "..directions[direction].."000"..index..".tga", 1152, 1024);
	end
end
TextureAtlasCreator:Get():addFrame();

humanbase:calcNumMoveTexturesPerDirection();
humanbase:setStrength( 15 )
humanbase:setDexterity( 25 )
humanbase:setVitality( 15 )
humanbase:setIntellect( 10 )
humanbase:setWisdom( 10 )
humanbase:setMaxHealth( 250 )
humanbase:setMaxMana( 500 )
humanbase:setMaxFatigue( 100 )
humanbase:setHealthRegen( 0 )
humanbase:setManaRegen( 50 )
humanbase:setFatigueRegen( 25 )
humanbase:setMinDamage( 5 )
humanbase:setMaxDamage( 10 )
humanbase:setArmor( 20 )
humanbase:setDamageModifierPoints( 0 )
humanbase:setHitModifierPoints( 0 )
humanbase:setEvadeModifierPoints( 0 )
humanbase:setName("A swordsman")
humanbase:setWanderRadius( 250 )
humanbase:setLevel( 2 )
humanbase:setClass( Enums.Warrior );
humanbase:setExperienceValue( 25 );

witchbase = DawnInterface.createNewMobType("Witch")
for direction = 1,8 do
	for index=0,7,1 do
		witchbase:addMoveTexture( Enums.Walking, direction, index, "res/character/witch/walking "..directions[direction].."000"..index..".tga", 768, 768);		
	end
end
TextureAtlasCreator:Get():addFrame();

for direction = 1,8 do
	for index=0,7,1 do
		witchbase:addMoveTexture( Enums.Dying, direction, index, "res/character/witch/walking "..directions[direction].."000"..index..".tga", 768, 768);		
	end
end
TextureAtlasCreator:Get():addFrame();


witchbase:calcNumMoveTexturesPerDirection();
witchbase:setStrength( 15 )
witchbase:setDexterity( 25 )
witchbase:setVitality( 15 )
witchbase:setIntellect( 10 )
witchbase:setWisdom( 10 )
witchbase:setMaxHealth( 250 )
witchbase:setMaxMana( 500 )
witchbase:setMaxFatigue( 100 )
witchbase:setHealthRegen( 0 )
witchbase:setManaRegen( 50 )
witchbase:setFatigueRegen( 25 )
witchbase:setMinDamage( 5 )
witchbase:setMaxDamage( 10 )
witchbase:setArmor( 20 )
witchbase:setDamageModifierPoints( 0 )
witchbase:setHitModifierPoints( 0 )
witchbase:setEvadeModifierPoints( 0 )
witchbase:setName("A witch")
witchbase:setWanderRadius( 250 )
witchbase:setLevel( 2 )
witchbase:setClass( Enums.Warrior );
witchbase:setExperienceValue( 25 );

wizardbase = DawnInterface.createNewMobType("Wizard")
for direction = 1,8 do
	for index=0,7,1 do
		wizardbase:addMoveTexture( Enums.Walking, direction, index, "res/character/wizard/walking "..directions[direction].."000"..index..".tga", 768, 768);
	end
end
TextureAtlasCreator:Get():addFrame();

for direction = 1,8 do
	for index=0,12,1 do
		wizardbase:addMoveTexture( Enums.Attacking, direction, index, "res/character/wizard/attacking "..directions[direction].."000"..index..".tga", 1248, 768);
	end
end
TextureAtlasCreator:Get():addFrame();

for direction = 1,8 do
	for index=0,12,1 do
		wizardbase:addMoveTexture( Enums.Dying, direction, index, "res/character/wizard/dying "..directions[direction].."000"..index..".tga", 1664, 1024);
	end
end
TextureAtlasCreator:Get():addFrame();

wizardbase:calcNumMoveTexturesPerDirection();
wizardbase:setStrength( 10 )
wizardbase:setDexterity( 10 )
wizardbase:setVitality( 10 )
wizardbase:setIntellect( 25 )
wizardbase:setWisdom( 20 )
wizardbase:setMaxHealth( 150 )
wizardbase:setMaxMana( 100 )
wizardbase:setHealthRegen( 0 )
wizardbase:setManaRegen( 1 )
wizardbase:setMinDamage( 2 )
wizardbase:setMaxDamage( 3 )
wizardbase:setArmor( 10 )
wizardbase:setDamageModifierPoints( 0 )
wizardbase:setHitModifierPoints( 0 )
wizardbase:setEvadeModifierPoints( 0 )
wizardbase:setName("A wizard")
wizardbase:setWanderRadius( 250 )
wizardbase:setLevel( 2 )
wizardbase:setClass( Enums.Liche )
wizardbase:setExperienceValue( 25 );

skeletonbase = DawnInterface.createNewMobType("Skeleton")
for direction = 1,8 do
	for index=0,8,1 do
		skeletonbase:addMoveTexture( Enums.Walking, direction, index, "res/character/skeleton/walking "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:Get():addFrame();

for direction = 1,8 do
	for index=0,9,1 do
		skeletonbase:addMoveTexture( Enums.Attacking, direction, index, "res/character/skeleton/attacking "..directions[direction].."000"..index..".tga", 960, 768);
	end
end
TextureAtlasCreator:Get():addFrame();

for direction = 1,8 do
	for index=0,9,1 do
		skeletonbase:addMoveTexture( Enums.Casting, direction, index, "res/character/skeleton/attacking "..directions[direction].."000"..index..".tga", 960, 768);
	end
end
TextureAtlasCreator:Get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		skeletonbase:addMoveTexture( Enums.Dying, direction, index, "res/character/skeleton/dying "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:Get():addFrame();

skeletonbase:calcNumMoveTexturesPerDirection();
skeletonbase:setStrength( 25 )
skeletonbase:setDexterity( 40 )
skeletonbase:setVitality( 45 )
skeletonbase:setIntellect( 10 )
skeletonbase:setWisdom( 10 )
skeletonbase:setMaxHealth( 250 )
skeletonbase:setMaxMana( 500 )
skeletonbase:setMaxFatigue( 100 )
skeletonbase:setHealthRegen( 0 )
skeletonbase:setManaRegen( 50 )
skeletonbase:setFatigueRegen( 25 )
skeletonbase:setMinDamage( 15 )
skeletonbase:setMaxDamage( 25 )
skeletonbase:setArmor( 200 )
skeletonbase:setDamageModifierPoints( 15 )
skeletonbase:setHitModifierPoints( 15 )
skeletonbase:setEvadeModifierPoints( 15 )
skeletonbase:setName("Skeleton")
skeletonbase:setWanderRadius( 250 )
skeletonbase:setLevel( 3 )
--skeletonbase:setCoinDrop( 25, 75, 0.75 );
skeletonbase:setClass( Enums.Warrior );
skeletonbase:setExperienceValue( 40 );

TextureManager:SetTextureAtlas(TextureAtlasCreator:Get():getName(), TextureAtlasCreator:Get():getAtlas());