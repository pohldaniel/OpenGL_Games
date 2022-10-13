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

wizardbase = DawnInterface.createNewMobType("Wizard")
for direction = 1,8 do
	for index=0,7,1 do
		wizardbase:addMoveTexture( Enums.Walking, direction, index, "res/character/wizard/walking "..directions[direction].."000"..index..".tga", 768, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,12,1 do
		wizardbase:addMoveTexture( Enums.Attacking, direction, index, "res/character/wizard/attacking "..directions[direction].."000"..index..".tga", 1248, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,12,1 do
		wizardbase:addMoveTexture( Enums.Dying, direction, index, "res/character/wizard/dying "..directions[direction].."000"..index..".tga", 1664, 1024);
	end
end
TextureAtlasCreator:get():addFrame();

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

spiderlingbase:calcNumMoveTexturesPerDirection();
spiderlingbase:setStrength( 10 )
spiderlingbase:setDexterity( 10 )
spiderlingbase:setVitality( 10 )
spiderlingbase:setIntellect( 10 )
spiderlingbase:setWisdom( 10 )
spiderlingbase:setMaxHealth( 135 )
spiderlingbase:setMaxMana( 500 )
spiderlingbase:setMaxFatigue( 100 )
spiderlingbase:setHealthRegen( 0 )
spiderlingbase:setManaRegen( 50 )
spiderlingbase:setFatigueRegen( 25 )
spiderlingbase:setMinDamage( 12 )
spiderlingbase:setMaxDamage( 16 )
spiderlingbase:setArmor( 10 )
spiderlingbase:setDamageModifierPoints( 5 )
spiderlingbase:setHitModifierPoints( 5 )
spiderlingbase:setEvadeModifierPoints( 0 )
spiderlingbase:setName("Spiderling")
spiderlingbase:setWanderRadius( 250 )
spiderlingbase:setLevel( 2 )
spiderlingbase:setClass( Enums.Ranger );
--spiderlingbase:inscribeSpellInSpellbook( spellDatabase["sylphiricacid"] );
--spiderlingbase:inscribeSpellInSpellbook( spellDatabase["venomspit"] );
--spiderlingbase:addItemToLootTable( itemDatabase["spidersilk"], 0.30 );
--spiderlingbase:addItemToLootTable( itemDatabase["sylphirfang"], 0.40 );
spiderlingbase:setExperienceValue( 30 );

spiderbase = DawnInterface.createNewMobType("Spider")
for direction = 1,8 do
	for index=0,7,1 do
		spiderbase:addMoveTexture( Enums.Walking, direction, index, "res/character/spider/walking "..directions[direction].."000"..index..".tga", 768, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		spiderbase:addMoveTexture( Enums.Attacking, direction, index, "res/character/spider/attacking "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		spiderbase:addMoveTexture( Enums.Shooting, direction, index, "res/character/spider/attacking "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,10,1 do
		spiderbase:addMoveTexture( Enums.Dying, direction, index, "res/character/spider/dying "..directions[direction].."000"..index..".tga", 1056, 768);
	end
end
TextureAtlasCreator:get():addFrame();

spiderbase:calcNumMoveTexturesPerDirection();
spiderbase:setStrength( 20 )
spiderbase:setDexterity( 30 )
spiderbase:setVitality( 20 )
spiderbase:setIntellect( 15 )
spiderbase:setWisdom( 15 )
spiderbase:setMaxHealth( 270 )
spiderbase:setMaxMana( 500 )
spiderbase:setMaxFatigue( 100 )
spiderbase:setHealthRegen( 0 )
spiderbase:setManaRegen( 50 )
spiderbase:setFatigueRegen( 25 )
spiderbase:setMinDamage( 5 )
spiderbase:setMaxDamage( 9 )
spiderbase:setArmor( 120 )
spiderbase:setDamageModifierPoints( 10 )
spiderbase:setHitModifierPoints( 10 )
spiderbase:setEvadeModifierPoints( 10 )
spiderbase:setName("Spider")
spiderbase:setWanderRadius( 250 )
spiderbase:setLevel( 2 )
spiderbase:setClass( Enums.Warrior );
--spiderbase:inscribeSpellInSpellbook( spellDatabase["spiderbite"] );
--spiderbase:inscribeSpellInSpellbook( spellDatabase["venomweave"] );
--spiderbase:addItemToLootTable( itemDatabase["spidersilk"], 0.30 );
--spiderbase:addItemToLootTable( itemDatabase["pristinesylphirsilk"], 0.20 );
spiderbase:setExperienceValue( 27 );

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

for direction = 1,8 do
	for index=0,8,1 do
		skeletonbase:addMoveTexture( Enums.Dying, direction, index, "res/character/skeleton/dying "..directions[direction].."000"..index..".tga", 864, 768);
	end
end
TextureAtlasCreator:get():addFrame();

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

skeletonarcherbase:calcNumMoveTexturesPerDirection();
skeletonarcherbase:setStrength( 25 )
skeletonarcherbase:setDexterity( 40 )
skeletonarcherbase:setVitality( 45 )
skeletonarcherbase:setIntellect( 10 )
skeletonarcherbase:setWisdom( 10 )
skeletonarcherbase:setMaxHealth( 250 )
skeletonarcherbase:setMaxMana( 500 )
skeletonarcherbase:setMaxFatigue( 100 )
skeletonarcherbase:setHealthRegen( 0 )
skeletonarcherbase:setManaRegen( 50 )
skeletonarcherbase:setFatigueRegen( 25 )
skeletonarcherbase:setMinDamage( 15 )
skeletonarcherbase:setMaxDamage( 25 )
skeletonarcherbase:setArmor( 200 )
skeletonarcherbase:setDamageModifierPoints( 15 )
skeletonarcherbase:setHitModifierPoints( 15 )
skeletonarcherbase:setEvadeModifierPoints( 15 )
skeletonarcherbase:setName("Skeleton Archer")
skeletonarcherbase:setWanderRadius( 250 )
skeletonarcherbase:setLevel( 3 )
--skeletonarcherbase:setCoinDrop( 25, 75, 0.75 );
skeletonarcherbase:setClass( Enums.Ranger );
--skeletonarcherbase:inscribeSpellInSpellbook( spellDatabase["shoot"] );
--skeletonarcherbase:addItemToLootTable( itemDatabase["weakenedbow"], 0.25 );
skeletonarcherbase:setExperienceValue( 40 );