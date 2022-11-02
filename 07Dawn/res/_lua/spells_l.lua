-- ==== Dynamic values ====
-- Note about dynamic values for the speltooltip.
-- Dynamic values can be set here. In the setInfo function, you can use these values that will be displayed in the game:
--"%minWeaponDamage%"
--"%maxWeaponDamage%"
--"%minSpellDirectDamage%"
--"%maxSpellDirectDamage%"
--"%minSpellContinuousDamage%"
--"%maxSpellContinuousDamage%"
--"%minDirectHealing%"
--"%maxDirectHealing%"
--"%minContinuousHealing%"
--"%maxContinuousHealing%"

-- ==== Spell ranks ====
-- When creating a spell, it automatically becomes rank 1.
-- To create rank X of this spell, use DawnInterface.copySpell( spellDatabase["name_of_spell"] );
-- Example:
-- curSpell = DawnInterface.copySpell( spellDatabase["magicmissile"] );
-- spellDatabase["magicmissilerank2"] = curSpell;
-- curSpell:setRank( 2 );
-- curSpell:setDirectDamage( 12, 18, ElementType.Light );
--
-- It's possible to alter most of the properties of a spell in another rank.

-- ==== Character States ====
-- To set character states to an effect we use setCharacterState( CharacterStates::CharacterStates );
--
-- Following CharacterStates are available:
--
--	Channeling
--	Charmed
--	Confused
--	Feared
--	Invisible
--	Mesmerized
--	Movementspeed
--	SeeInvisible
--	SeeSneaking
--	Sneaking
--	Stunned

-- === Other notes ===
-- % needs to be written with %%. So 50%% will be displayed in the tooltips as: "50%" 

symbols = {};

TextureAtlasCreator:get():init(1024, 1024);
symbols["magicmissile"] = DawnInterface.loadimage("res/spells/magicmissile/symbol.tga");
symbols["venomspit"] = DawnInterface.loadimage("res/spells/venomspit/symbol.tga");
symbols["electrocute"] = DawnInterface.loadimage("res/spells/electrocute/symbol.tga");
symbols["inferno"] = DawnInterface.loadimage("res/spells/inferno/symbol.tga");
symbols["lightning"] = DawnInterface.loadimage("res/spells/lightning/symbol.tga");
symbols["healing"] = DawnInterface.loadimage("res/spells/healing/symbol.tga");
symbols["healother"] = DawnInterface.loadimage("res/spells/healother/symbol.tga");
symbols["shieldbash"] = DawnInterface.loadimage("res/spells/shieldbash/symbol.tga");
symbols["leatherskin"] = DawnInterface.loadimage("res/spells/leatherskin/symbol.tga");
symbols["layingofhands"] = DawnInterface.loadimage("res/spells/layingofhands/symbol.tga");
symbols["callingofthegrave"] = DawnInterface.loadimage("res/spells/callingofthegrave/symbol.tga");
symbols["mightofthetitan"] = DawnInterface.loadimage("res/spells/mightofthetitan/symbol.tga");
symbols["vortex"] = DawnInterface.loadimage("res/spells/vortex/symbol.tga");
symbols["earthenseeds"] = DawnInterface.loadimage("res/spells/earthenseeds/symbol.tga");
symbols["melee"] = DawnInterface.loadimage("res/spells/melee/symbol.tga");
symbols["phantomstrike"] = DawnInterface.loadimage("res/spells/phantomstrike/symbol.tga");
symbols["venomweave"] = DawnInterface.loadimage("res/spells/venomweave/symbol.tga");
symbols["shoot"] = DawnInterface.loadimage("res/spells/shoot/symbol.tga");
symbols["terrify"] = DawnInterface.loadimage("res/spells/terrify/symbol.tga");
symbols["food"] = DawnInterface.loadimage("res/spells/food/symbol.tga");
symbols["frozenshot"] = DawnInterface.loadimage("res/spells/frozenshot/symbol.tga");
symbols["pindown"] = DawnInterface.loadimage("res/spells/pindown/symbol.tga");
symbols["flamingarrow"] = DawnInterface.loadimage("res/spells/flamingarrow/symbol.tga");
symbols["landyrsforce"] = DawnInterface.loadimage("res/spells/landyrsforce/symbol.tga");
symbols["voiceoftheforest"] = DawnInterface.loadimage("res/spells/voiceoftheforest/symbol.tga");
TextureManager:SetTextureAtlas("symbols", TextureAtlasCreator:get():getAtlas());
--TextureAtlasCreator:get():safeAtlas("tmp/symbols")

spellDatabase = {};

TextureAtlasCreator:get():init(1024, 1024);

curSpell = DawnInterface.createGeneralRayDamageSpell();
spellDatabase["electrocute"] = curSpell;
curSpell:setName("Electrocute");
curSpell:setInfo("Electrocutes the target with %minSpellDirectDamage%-%maxSpellDirectDamage% point of air damage +  %minSpellContinuousDamage%-%maxSpellContinuousDamage% air damage over 2 seconds.");
curSpell:setCastTime( 5000 );
curSpell:setCooldown( 5 );
curSpell:setSpellCost( 50 );
curSpell:setSoundSpellCasting( "res/sound/spellcast.ogg" );
curSpell:setDirectDamage(  50, 100, Enums.Air );
curSpell:setContinuousDamage( 10, 20, 2000, Enums.Air );
curSpell:setSymbolTextureRect(symbols["electrocute"]);
curSpell:setSoundSpellStart( "res/spells/lightning/lightning.ogg" );
curSpell:addAnimationFrame("res/spells/lightning/1.tga");
curSpell:addAnimationFrame("res/spells/lightning/2.tga");
curSpell:addAnimationFrame("res/spells/lightning/3.tga");
curSpell:addAnimationFrame("res/spells/lightning/4.tga");
curSpell:addAnimationFrame("res/spells/lightning/5.tga");
curSpell:setRequiredClass( Enums.Liche );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

TextureManager:SetTextureAtlas("spells", TextureAtlasCreator:get():getAtlas());
--TextureAtlasCreator:get():safeAtlas("tmp/spell")