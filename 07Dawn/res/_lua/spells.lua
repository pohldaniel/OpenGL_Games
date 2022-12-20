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

TextureAtlasCreator:Get():init("symbols", 1024, 1024);
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
TextureManager:SetTextureAtlas(TextureAtlasCreator:Get():getName(), TextureAtlasCreator:Get():getAtlas());

spellDatabase = {};

TextureAtlasCreator:Get():init("spells", 1024, 1024);

curSpell = DawnInterface.createGeneralBoltDamageSpell();
spellDatabase["magicmissile"] = curSpell;
curSpell:setName("Magic Missile");
curSpell:setInfo("A magical missile causing %minSpellDirectDamage%-%maxSpellDirectDamage% points of light damage to the target");
curSpell:setCastTime( 1000 );
curSpell:setSpellCost( 10 );
curSpell:setSoundSpellCasting( "res/sound/spellcast.ogg" );
curSpell:setDirectDamage(  6, 10, Enums.Light );
curSpell:setSymbolTextureRect(symbols["magicmissile"]);
curSpell:addAnimationFrame("res/spells/magicmissile/magicmissile.tga");
curSpell:setMoveSpeed( 500 );
curSpell:setRequiredClass( Enums.Liche );
curSpell:setNeedTarget(true);
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.copySpell( spellDatabase["magicmissile"] );
spellDatabase["magicmissilerank2"] = curSpell;
curSpell:setDirectDamage( 12, 18, Enums.Light );
curSpell:setRank( 2 );
curSpell:setRequiredLevel( 3 );
curSpell:setSpellCost( 17 );
curSpell:setNeedTarget(true);

curSpell = DawnInterface.createGeneralRayDamageSpell();
spellDatabase["venomspiteffect1"] = curSpell;
curSpell:setName("Venomspit");
curSpell:setSymbolTextureRect(symbols["venomspit"]);
curSpell:setContinuousDamage( 4, 6, 10000, Enums.Earth );
curSpell:setNeedTarget(true);

curSpell = DawnInterface.createRangedDamageAction();
spellDatabase["venomspit"] = curSpell;
curSpell:setName("Venomspit");
curSpell:setDamageBonus( 1.0 );
curSpell:setCastTime( 2000 );
curSpell:setSpellCost( 0 );
curSpell:setSoundSpellStart( "res/spells/venomspit/venomspit.ogg" );
curSpell:addAnimationFrame("res/spells/venomspit/0.tga");
curSpell:setMoveSpeed( 1000 );
curSpell:addAdditionalSpellOnTarget( spellDatabase["venomspiteffect1"], 0.05 );
curSpell:setRequiredClass( Enums.Ranger );

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
curSpell:setNeedTarget(true);
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralAreaDamageSpell();
spellDatabase["inferno"] = curSpell;
curSpell:setName("Inferno");
curSpell:setInfo("Burns the target with %minSpellDirectDamage%-%maxSpellDirectDamage% point of fire damage + %minSpellContinuousDamage%-%maxSpellContinuousDamage% fire damage over 5 seconds.");
curSpell:setCastTime( 2000 );
curSpell:setCooldown( 5 );
curSpell:setSpellCost( 75 );
curSpell:setDirectDamage(  15, 30, Enums.Fire );
curSpell:setContinuousDamage( 4, 7, 5000, Enums.Fire );
curSpell:setSymbolTextureRect(symbols["inferno"]);
curSpell:addAnimationFrame("res/spells/inferno/inferno.tga");
curSpell:setRadius( 120 );
curSpell:setRequiredClass( Enums.Liche );
curSpell:setRange(0, -1);
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralBoltDamageSpell();
spellDatabase["lightningbolt"] = curSpell;
curSpell:setName("Lightning Bolt");
curSpell:setInfo("A bolt of raw lightning aimed at the target causing 20-40 points of lightning damage.");
curSpell:setCastTime( 1500 );
curSpell:setSpellCost( 25 );
curSpell:setDirectDamage(  20, 40, Enums.Air );
curSpell:setSymbolTextureRect(symbols["lightning"]);
curSpell:addAnimationFrame("res/spells/lightning/1_small.tga");
curSpell:addAnimationFrame("res/spells/lightning/2_small.tga");
curSpell:addAnimationFrame("res/spells/lightning/3_small.tga");
curSpell:addAnimationFrame("res/spells/lightning/4_small.tga");
curSpell:addAnimationFrame("res/spells/lightning/5_small.tga");
curSpell:setMoveSpeed( 1000 );
curSpell:setRequiredClass( Enums.Liche );
curSpell:setNeedTarget(true);
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralHealingSpell();
spellDatabase["healing"] = curSpell;
curSpell:setName("Healing");
curSpell:setInfo("Heals %minDirectHealing% points of damage by heavenly light on self.");
curSpell:setCastTime( 2000 );
curSpell:setSpellCost( 30 );
curSpell:setSymbolTextureRect(symbols["healing"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setDirectHealing( 50, 50, Enums.Light );
curSpell:setRequiredClass( Enums.Liche );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralHealingSpell();
spellDatabase["forcedhealing"] = curSpell;
curSpell:setName("Forced Healing");
curSpell:setInfo("Heals %minDirectHealing%-%maxDirectHealing% points of damage by dark magic on target.");
curSpell:setCastTime( 2000 );
curSpell:setSpellCost( 40 );
curSpell:setSymbolTextureRect(symbols["healother"]);
curSpell:setEffectType( Enums.SingleTargetSpell );
curSpell:setDirectHealing( 50, 70, Enums.Dark );
curSpell:setRequiredClass( Enums.Liche );
curSpell:setNeedTarget(true);
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralHealingSpell();
spellDatabase["hymnofrestoration"] = curSpell;
curSpell:setName("Hymn of restoration");
curSpell:setInfo("Heals you for %minContinuousHealing%-%maxContinuousHealing% points of damage over 12 seconds.");
curSpell:setCastTime( 500 );
curSpell:setSpellCost( 25 );
curSpell:setSymbolTextureRect(symbols["healing"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setCooldown( 30 );
curSpell:setContinuousHealing( 10, 15, 12000, Enums.Light );
curSpell:setRequiredClass( Enums.Liche );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["steelgrip"] = curSpell;
curSpell:setName("Steelgrip");
curSpell:setInfo("Slams against your shield causing your blood to boil, increasing strength by 5.");
curSpell:setStats( Enums.Strength, 5 );
curSpell:setCastTime( 0 );
curSpell:setSpellCost( 0 );
curSpell:setSymbolTextureRect(symbols["shieldbash"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setDuration( 30 );
curSpell:setRequiredClass( Enums.ANYCLASS );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["leatherskin"] = curSpell;
curSpell:setName("Leatherskin");
curSpell:setInfo("Encoats your skin with leather, increasing armor by 80.");
curSpell:setStats( Enums.Armor, 80 );
curSpell:setCastTime( 2500 );
curSpell:setSpellCost( 75 );
curSpell:setSymbolTextureRect(symbols["leatherskin"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setDuration( 300 );
curSpell:setRequiredClass( Enums.Ranger );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.copySpell( spellDatabase["leatherskin"] );
spellDatabase["leatherskinrank2"] = curSpell;
curSpell:setRank( 2 );
curSpell:setStats( Enums.Armor, 120 );
curSpell:setSpellCost( 125 );
curSpell:setRequiredLevel( 4 );
curSpell:setInfo( "Encoats your skin with leather, increasing armor by 125." );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["spiritoftheboar"] = curSpell;
curSpell:setName("Spirit of the Boar");
curSpell:setInfo("Embraces you with the spirit of the boar, increasing vitality by 5.");
curSpell:setStats( Enums.Vitality, 5 );
curSpell:setCastTime( 0 );
curSpell:setSpellCost( 0 );
curSpell:setSymbolTextureRect(symbols["leatherskin"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setDuration( 600 );
curSpell:setRequiredClass( Enums.ANYCLASS );

curSpell = DawnInterface.createGeneralHealingSpell();
spellDatabase["layingofhands"] = curSpell;
curSpell:setName("Laying of hands");
curSpell:setInfo("Instantly heals you for %minDirectHealing% points of damage.");
curSpell:setCastTime( 0 );
curSpell:setCooldown( 600 );
curSpell:setSpellCost( 0 );
curSpell:setSymbolTextureRect(symbols["layingofhands"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setDirectHealing( 500, 500, Enums.Light );
curSpell:setRequiredClass( Enums.Warrior );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralHealingSpell();
spellDatabase["smallhealingpotion"] = curSpell;
curSpell:setName("Small healing potion");
curSpell:setInfo("Heals %minDirectHealing%-%maxDirectHealing% points of damage on self.");
curSpell:setCastTime( 0 );
curSpell:setSpellCost( 0 );
curSpell:setCooldown( 120 );
curSpell:setSymbolTextureRect(symbols["healing"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setDirectHealing( 10, 25, Enums.Light );
curSpell:setRequiredClass( Enums.ANYCLASS );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["callingofthegrave"] = curSpell;
curSpell:setName("Calling of the grave");
curSpell:setSpellEffectElementModifierPoints( Enums.Dark, 25 );
curSpell:setDuration( 600 );
curSpell:setInfo("The ancient tombs of Ark'li engulfs you, increasing Dark spell damage.");
curSpell:setCastTime( 4000 );
curSpell:setSpellCost( 100 );
curSpell:setSymbolTextureRect(symbols["callingofthegrave"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setRequiredClass( Enums.Liche );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["mightofthetitan"] = curSpell;
curSpell:setName("Might of the Titan");
curSpell:setStats( Enums.Strength, 15 );
curSpell:setDuration( 600 );
curSpell:setInfo("Calls for the power of the Titans, increasing your strength.");
curSpell:setCastTime( 1000 );
curSpell:setSpellCost( 0 );
curSpell:setSymbolTextureRect(symbols["mightofthetitan"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setRequiredClass( Enums.Warrior );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["flowingthought"] = curSpell;
curSpell:setName("Flowing thought");
curSpell:setStats( Enums.ManaRegen, 5 );
curSpell:setDuration( 3 );
curSpell:setInfo("Clears your mind, regenerating 15 mana over 3 seconds.");
curSpell:setCastTime( 0 );
curSpell:setSymbolTextureRect(symbols["vortex"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setRequiredClass( Enums.ANYCLASS );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["manavortex"] = curSpell;
curSpell:setName("Mana Vortex");
curSpell:setStats( Enums.ManaRegen, 10 );
curSpell:setDuration( 15 );
curSpell:setInfo("Expanding your mind with a vortex of knowledge, increasing mana regen rate.");
curSpell:setCastTime( 500 );
curSpell:setCooldown( 120 );
curSpell:setSymbolTextureRect(symbols["vortex"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setRequiredClass( Enums.Liche );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["earthenseeds"] = curSpell;
curSpell:setName("Earthen Seeds");
curSpell:setStats( Enums.HealthRegen, 3 );
curSpell:setDuration( 180 );
curSpell:setInfo("Seeds in the earth increases your health regen.");
curSpell:setSpellCost( 75 );
curSpell:setCastTime( 2000 );
curSpell:setSymbolTextureRect(symbols["earthenseeds"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setRequiredClass( Enums.Liche );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createMeleeDamageAction();
spellDatabase["spiderbite"] = curSpell;
curSpell:setName("Spiderbite");
curSpell:setDamageBonus( 1.0 );
curSpell:setCooldown( 2 );
curSpell:setSymbolTextureRect(symbols["melee"]);
curSpell:setSpellCost( 0 );
curSpell:setRequiredClass( Enums.ANYCLASS );
curSpell:setNeedTarget(true);

curSpell = DawnInterface.createMeleeDamageAction();
spellDatabase["wolfbite"] = curSpell;
curSpell:setName("Wolfbite");
curSpell:setDamageBonus( 1.0 );
curSpell:setCooldown( 2 );
curSpell:setSymbolTextureRect(symbols["melee"]);
curSpell:setSpellCost( 0 );
curSpell:setRequiredClass( Enums.ANYCLASS );
curSpell:setNeedTarget(true);

curSpell = DawnInterface.createMeleeDamageAction();
spellDatabase["melee"] = curSpell;
curSpell:setName("Melee");
curSpell:setDamageBonus( 1.0 );
curSpell:setCooldown( 2 );
curSpell:setSymbolTextureRect(symbols["melee"]);
curSpell:setSpellCost( 0 );
curSpell:setSoundSpellStart( "res/spells/melee/melee_swing.ogg" );
curSpell:setSoundSpellHit( "res/spells/melee/melee_hit.ogg" );
curSpell:setInfo("Attack with your weapon, causing %minWeaponDamage%-%maxWeaponDamage% damage.");
curSpell:setRequiredClass( Enums.ANYCLASS );
curSpell:setNeedTarget(true);
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createMeleeDamageAction();
spellDatabase["phantomstrike"] = curSpell;
curSpell:setName("Phantom strike");
curSpell:setDamageBonus( 4.0 );
curSpell:setCooldown( 12 );
curSpell:setInfo("Devastating strike, causing %minWeaponDamage% to %maxWeaponDamage% damage.");
curSpell:setSymbolTextureRect(symbols["phantomstrike"]);
curSpell:setSpellCost( 50 );
curSpell:setRequiredClass( Enums.Warrior );
curSpell:setNeedTarget(true);
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["venomweaveeffect2"] = curSpell;
curSpell:setName("Venomweave");
curSpell:setInfo("Spiderweb entangled your entire body, rendering you unable to act or move!");
curSpell:setCharacterState( Enums.Stunned );
curSpell:setDuration( 5 );
curSpell:setSymbolTextureRect(symbols["venomweave"]);

curSpell = DawnInterface.createGeneralRayDamageSpell();
spellDatabase["venomweaveeffect1"] = curSpell;
curSpell:setName("Venomweave");
curSpell:setSymbolTextureRect(symbols["venomspit"]);
curSpell:setContinuousDamage( 4, 6, 3000, Enums.Earth );
curSpell:addAdditionalSpellOnTarget( spellDatabase["venomweaveeffect2"], 0.1 );
curSpell:setNeedTarget(true);

curSpell = DawnInterface.createMeleeDamageAction();
spellDatabase["venomweave"] = curSpell;
curSpell:setName("Venomweave");
curSpell:setDamageBonus( 2.0 );
curSpell:setCooldown( 4 );
curSpell:setSymbolTextureRect(symbols["venomweave"]);
curSpell:setSpellCost( 25 );
curSpell:addAdditionalSpellOnTarget( spellDatabase["venomweaveeffect1"], 0.75 );
curSpell:setRequiredClass( Enums.Warrior );
curSpell:setNeedTarget(true);

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["rabies"] = curSpell;
curSpell:setName("Rabies");
curSpell:setStats( Enums.HealthRegen, -25 );
curSpell:setStats( Enums.Vitality, -5 );
curSpell:setDuration( 60 );
curSpell:setInfo("Rabies reducing your health regen and vitality.");
curSpell:setSpellCost( 0 );
curSpell:setCastTime( 0 );
curSpell:setSymbolTextureRect(symbols["earthenseeds"]);
curSpell:setEffectType( Enums.SingleTargetSpell );

curSpell = DawnInterface.createMeleeDamageAction();
spellDatabase["rabidbite"] = curSpell;
curSpell:setName("Rabid bite");
curSpell:setDamageBonus( 1.5 );
curSpell:setCooldown( 1 );
curSpell:setSymbolTextureRect(symbols["melee"]);
curSpell:setSpellCost( 0 );
curSpell:setSoundSpellStart( "res/spells/rabidbite/rabidbite.ogg" );
curSpell:addAdditionalSpellOnTarget( spellDatabase["rabies"], 0.05 );
curSpell:setRequiredClass( Enums.Warrior );
curSpell:setNeedTarget(true);

curSpell = DawnInterface.createGeneralHealingSpell();
spellDatabase["bandages"] = curSpell;
curSpell:setName("Bandages");
curSpell:setInfo("Bandaging yourself, healing %minContinuousHealing%-%maxContinuousHealing% points of damage over 10 seconds.");
curSpell:setCastTime( 1000 );
curSpell:setSpellCost( 0 );
curSpell:setContinuousHealing( 15, 20, 10000, Enums.Light );
curSpell:setCooldown( 60 );
curSpell:setSymbolTextureRect(symbols["healing"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setRequiredClass( Enums.Warrior );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["lungeeffect1"] = curSpell;
curSpell:setName("Lunge");
curSpell:setStats( Enums.ParryModifier, 15 );
curSpell:setDuration( 6 );
curSpell:setSymbolTextureRect(symbols["melee"]);

curSpell = DawnInterface.createMeleeDamageAction();
spellDatabase["lunge"] = curSpell;
curSpell:setName("Lunge");
curSpell:setDamageBonus( 1.5 );
curSpell:setCooldown( 12 );
curSpell:setSymbolTextureRect(symbols["melee"]);
curSpell:setSpellCost( 30 );
curSpell:setInfo("Tactical strike, causing %minWeaponDamage% to %maxWeaponDamage% and increasing your parry chance the next 6 seconds.");
curSpell:addAdditionalSpellOnCreator( spellDatabase["lungeeffect1"], 1.0 );
curSpell:setRequiredClass( Enums.Warrior );
curSpell:addRequiredWeapon( Enums.ONEHAND_SWORD );
curSpell:setNeedTarget(true);
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["shieldbasheffect1"] = curSpell;
curSpell:setName("Shield bash");
curSpell:setCharacterState( Enums.Stunned );
curSpell:setDuration( 3 );
curSpell:setSymbolTextureRect(symbols["shieldbash"]);

curSpell = DawnInterface.createMeleeDamageAction();
spellDatabase["shieldbash"] = curSpell;
curSpell:setName("Shield bash");
curSpell:setDamageBonus( 1.2 );
curSpell:setCooldown( 12 );
curSpell:setSymbolTextureRect(symbols["shieldbash"]);
curSpell:setSpellCost( 40 );
curSpell:setInfo("Bash with your shield, causing %minWeaponDamage% to %maxWeaponDamage% and stunning your enemy for 3 seconds.");
curSpell:addAdditionalSpellOnTarget( spellDatabase["shieldbasheffect1"], 1.0 );
curSpell:setRequiredClass( Enums.Warrior );
curSpell:addRequiredWeapon( Enums.SHIELD ); 
curSpell:setNeedTarget(true);
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createRangedDamageAction();
spellDatabase["shoot"] = curSpell;
curSpell:setName("Shoot");
curSpell:setInfo("Shoot with your ranged weapon, causing %minWeaponDamage%-%maxWeaponDamage% damage.");
curSpell:setDamageBonus( 1.0 );
curSpell:setCastTime( 2000 );
curSpell:setSpellCost( 0 );
curSpell:setSymbolTextureRect(symbols["shoot"]);
curSpell:addAnimationFrame("res/spells/shoot/0.tga");
curSpell:setMoveSpeed( 1000 );
curSpell:setRequiredClass( Enums.Ranger );
curSpell:setSoundSpellStart( "res/spells/shoot/shoot.ogg" );
curSpell:addRequiredWeapon( Enums.BOW );
curSpell:addRequiredWeapon( Enums.CROSSBOW );
curSpell:addRequiredWeapon( Enums.SLINGSHOT );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["invisibility"] = curSpell;
curSpell:setName("Invisibility");
curSpell:setDuration( 12 );
curSpell:setInfo("Granting you invisibility, making you undetectable by most enemies.");
curSpell:setSpellCost( 50 );
curSpell:setCastTime( 1000 );
curSpell:setSymbolTextureRect(symbols["healing"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setRequiredClass( Enums.NOCLASS );
curSpell:setCharacterState( Enums.Invisible );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["sneak"] = curSpell;
curSpell:setName("Sneak");
curSpell:setDuration( 30 );
curSpell:setInfo("Hiding in shadows and sneaking allows you to go undetected from most enemies.");
curSpell:setSpellCost( 50 );
curSpell:setCastTime( 1000 );
curSpell:setSymbolTextureRect(symbols["healing"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setRequiredClass( Enums.NOCLASS );
curSpell:setCharacterState( Enums.Sneaking );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["terrifyeffect2"] = curSpell;
curSpell:setName("Terrify");
curSpell:setCastTime( 0 );
curSpell:setDuration( 60 );
curSpell:setSpellCost( 0 );
curSpell:setResistElementModifierPoints( Enums.Dark, -10 );
curSpell:setSymbolTextureRect(symbols["terrify"]);
curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["terrifyeffect1"] = curSpell;
curSpell:setName("Terrify");
curSpell:setCharacterState( Enums.Feared );
curSpell:setDuration( 4 );
curSpell:setSymbolTextureRect(symbols["terrify"]);
curSpell:addAdditionalSpellOnTarget( spellDatabase["terrifyeffect2"], 1.0 );

curSpell = DawnInterface.createGeneralBoltDamageSpell();
spellDatabase["terrify"] = curSpell;
curSpell:setName("Terrify");
curSpell:setCooldown( 30 );
curSpell:setDirectDamage( 20, 30, Enums.Dark );
curSpell:setCastTime( 1500 );
curSpell:setSymbolTextureRect(symbols["terrify"]);
curSpell:setSpellCost( 40 );
curSpell:setSoundSpellStart( "res/spells/terrify/terrify.ogg" );
curSpell:setInfo("Sends a dark soul of the Umbral'un to terrorize the target dealing %minSpellDirectDamage%-%maxSpellDirectDamage% damage and causing it to flee for a short period. When the soul leaves it curses the target, reducing Dark magic resistance for a minute.");
curSpell:addAnimationFrame("res/spells/terrify/0.tga");
curSpell:setMoveSpeed( 650 );
curSpell:addAdditionalSpellOnTarget( spellDatabase["terrifyeffect1"], 1.0 );
curSpell:setRequiredClass( Enums.Liche );
curSpell:setNeedTarget(true);
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["food"] = curSpell;
curSpell:setName("Food");
curSpell:setStats( Enums.HealthRegen, 10 );
curSpell:setDuration( 18 );
curSpell:setInfo("Increases health regeneration by 10 while eating.");
curSpell:setSymbolTextureRect(symbols["food"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setCharacterState( Enums.Channeling );
curSpell:setRequiredClass( Enums.ANYCLASS );

curSpell = DawnInterface.copySpell( spellDatabase["food"] );
spellDatabase["foodrank2"] = curSpell;
curSpell:setRank( 2 );
curSpell:setStats( Enums.HealthRegen, 15 );
curSpell:setInfo("Increases health regeneration by 15 while eating.");
curSpell:setRequiredLevel( 3 );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["drink"] = curSpell;
curSpell:setName("Drink");
curSpell:setStats( Enums.ManaRegen, 15 );
curSpell:setDuration( 18 );
curSpell:setInfo("Increases mana regeneration by 15 while drinking.");
curSpell:setSymbolTextureRect(symbols["food"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setCharacterState( Enums.Channeling );
curSpell:setRequiredClass( Enums.ANYCLASS );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["frozenshoteffect1"] = curSpell;
curSpell:setName("Frozen shot ef");
curSpell:setDuration( 12 );
curSpell:setSymbolTextureRect(symbols["frozenshot"]);
curSpell:setCharacterState( Enums.Movementspeed, 0.5 );

curSpell = DawnInterface.createRangedDamageAction();
spellDatabase["frozenshot"] = curSpell;
curSpell:setName("Frozen shot");
curSpell:setInfo("Launches a frozen projectile, causing %minWeaponDamage%-%maxWeaponDamage% damage and slowing the target's movementspeed by 50%% for 12 seconds.");
curSpell:setDamageBonus( 2.0 );
curSpell:setCastTime( 2000 );
curSpell:setSpellCost( 25 );
curSpell:setSymbolTextureRect(symbols["frozenshot"]);
curSpell:setCooldown( 18 );
curSpell:addAnimationFrame("res/spells/shoot/0.tga" );
curSpell:setMoveSpeed( 1000 );
curSpell:setRequiredClass( Enums.Ranger );
curSpell:addRequiredWeapon( Enums.BOW );
curSpell:addRequiredWeapon( Enums.CROSSBOW );
curSpell:addRequiredWeapon( Enums.SLINGSHOT );
curSpell:addAdditionalSpellOnTarget( spellDatabase["frozenshoteffect1"], 1.0 );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["pindowneffect1"] = curSpell;
curSpell:setName("Pin down ef");
curSpell:setDuration( 5 );
curSpell:setSymbolTextureRect(symbols["pindown"]);
curSpell:setCharacterState( Enums.Movementspeed, 0.0 );

curSpell = DawnInterface.createRangedDamageAction();
spellDatabase["pindown"] = curSpell;
curSpell:setName("Pin down");
curSpell:setInfo("Skillfully pins down your target with your missile causing them to be immobilized for 5 seconds and causing %minWeaponDamage%-%maxWeaponDamage% damage.");
curSpell:setDamageBonus( 0.5 );
curSpell:setCastTime( 500 );
curSpell:setSpellCost( 10 );
curSpell:setCooldown( 14 );
curSpell:setSymbolTextureRect(symbols["pindown"]);
curSpell:addAnimationFrame("res/spells/shoot/0.tga" );
curSpell:setMoveSpeed( 1000 );
curSpell:setRequiredClass( Enums.Ranger );
curSpell:addRequiredWeapon( Enums.BOW );
curSpell:addRequiredWeapon( Enums.CROSSBOW );
curSpell:addRequiredWeapon( Enums.SLINGSHOT );
curSpell:addAdditionalSpellOnTarget( spellDatabase["pindowneffect1"], 1.0 );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralBoltDamageSpell();
spellDatabase["flamingarroweffect1"] = curSpell;
curSpell:setName("Flaming arrow ef");
curSpell:setSymbolTextureRect(symbols["frozenshot"]);
curSpell:setInstant( true );
curSpell:setDirectDamage( 15, 30, Enums.Earth );
curSpell:setNeedTarget(true);

curSpell = DawnInterface.createRangedDamageAction();
spellDatabase["flamingarrow"] = curSpell;
curSpell:setName("Flaming arrow");
curSpell:setInfo("Launching a projectile imbued with the fire of Angyr. Causing %minWeaponDamage%-%maxWeaponDamage% + 15-30 earth magic damage.");
curSpell:setDamageBonus( 1 );
curSpell:setCastTime( 1500 );
curSpell:setSpellCost( 20 );
curSpell:setCooldown( 5 );
curSpell:setSoundSpellStart( "res/spells/flamingarrow/flamingarrow.ogg" );
curSpell:setSymbolTextureRect(symbols["flamingarrow"]);
curSpell:addAnimationFrame("res/spells/shoot/0.tga");
curSpell:setMoveSpeed( 1000 );
curSpell:setRequiredClass( Enums.Ranger );
curSpell:addRequiredWeapon( Enums.BOW );
curSpell:addRequiredWeapon( Enums.CROSSBOW );
curSpell:addRequiredWeapon( Enums.SLINGSHOT );
curSpell:addAdditionalSpellOnTarget( spellDatabase["flamingarroweffect1"], 1.0 );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createRangedDamageAction();
spellDatabase["landyrsforce"] = curSpell;
curSpell:setName("Landyr's force");
curSpell:setInfo("Carefully aiming and striking with Landyr's force causing %minWeaponDamage%-%maxWeaponDamage% damage.");
curSpell:setDamageBonus( 4.0 );
curSpell:setCastTime( 4000 );
curSpell:setSpellCost( 35 );
curSpell:setSymbolTextureRect(symbols["landyrsforce"]);
curSpell:setCooldown( 24 );
curSpell:addAnimationFrame("res/spells/shoot/0.tga");
curSpell:setMoveSpeed( 1200 );
curSpell:setRequiredClass( Enums.Ranger );
curSpell:addRequiredWeapon( Enums.BOW );
curSpell:addRequiredWeapon( Enums.CROSSBOW );
curSpell:addRequiredWeapon( Enums.SLINGSHOT );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["voiceoftheforest"] = curSpell;
curSpell:setName("Voice of the forest");
curSpell:setStats( Enums.MeleeCritical, 5 );
curSpell:setStats( Enums.Dexterity, 15 );
curSpell:setDuration( 180 );
curSpell:setInfo("The spirits of the forest imbues your body, increasing your melee critical strike chance and dexterity.");
curSpell:setCastTime( 2000 );
curSpell:setSymbolTextureRect(symbols["voiceoftheforest"]);
curSpell:setEffectType( Enums.SelfAffectingSpell );
curSpell:setRequiredClass( Enums.Ranger );
DawnInterface.inscribeSpellInPlayerSpellbook( curSpell );

curSpell = DawnInterface.createGeneralBuffSpell();
spellDatabase["sylphiricacideffect1"] = curSpell;
curSpell:setName("Terrify");
curSpell:setCharacterState( Enums.Feared );
curSpell:setDuration( 4 );
curSpell:setSymbolTextureRect(symbols["terrify"]);

curSpell = DawnInterface.createRangedDamageAction();
spellDatabase["sylphiricacid"] = curSpell;
curSpell:setName("Sylphiric Acid");
curSpell:setInfo("Burning acid sphews over the victim, causing agonizing pain and sometimes inflicts terror in the victim.");
curSpell:setDamageBonus( 2.5 );
curSpell:setCastTime( 2000 );
curSpell:setSpellCost( 35 );
curSpell:setSoundSpellStart( "res/spells/venomspit/venomspit.ogg" );
curSpell:addAnimationFrame("res/spells/venomspit/0.tga");
curSpell:setMoveSpeed( 600 );
curSpell:setSymbolTextureRect(symbols["landyrsforce"]);
curSpell:setCooldown( 12 );
curSpell:addAdditionalSpellOnTarget( spellDatabase["sylphiricacideffect1"], 1.0 );
curSpell:setRequiredClass( Enums.Ranger );

TextureManager:SetTextureAtlas("spells", TextureAtlasCreator:Get():getAtlas());