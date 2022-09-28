directions = {'n', 'ne', 'e', 'se', 's', 'sw', 'w', 'nw'}

TextureAtlasCreator:get():init(864, 768);
wolfbase = DawnInterface.createNewMobType("Wolf");

for direction = 1,8 do
	for index=0,7,1 do
		wolfbase:addMoveTexture( ActivityType.Walking, direction, index, "res/character/wolf/running "..directions[direction].."000"..index..".tga" );
	end
	TextureAtlasCreator:get():resetLine();
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		wolfbase:addMoveTexture( ActivityType.Attacking, direction, index, "res/character/wolf/attacking "..directions[direction].."000"..index..".tga" );
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		wolfbase:addMoveTexture( ActivityType.Casting, direction, index, "res/character/wolf/attacking "..directions[direction].."000"..index..".tga" );
	end
end
TextureAtlasCreator:get():addFrame();

for direction = 1,8 do
	for index=0,8,1 do
		wolfbase:addMoveTexture( ActivityType.Dying, direction, index, "res/character/wolf/dying "..directions[direction].."000"..index..".tga" );
	end
end
TextureManager:SetTextureAtlas("Wolf", TextureAtlasCreator:get():getAtlas());

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
wolfbase:setClass( CharacterClass.Warrior );
--wolfbase:inscribeSpellInSpellbook( spellDatabase["wolfbite"] );
--wolfbase:inscribeSpellInSpellbook( spellDatabase["rabidbite"] );
--wolfbase:addItemToLootTable( itemDatabase["patchofwolfskin"], 0.25 )
--wolfbase:addItemToLootTable( itemDatabase["wolfmeat"], 0.10 )
wolfbase:setExperienceValue( 25 );