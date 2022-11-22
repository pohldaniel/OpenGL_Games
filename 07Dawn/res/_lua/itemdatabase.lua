-- ===============================================================================================
-- Information about adding triggerspells.
--
-- addTriggerSpellOnSelf( CSpellActionBase *spellToTrigger, float chanceToTrigger, Enums::Enums Enums );
-- addTriggerSpellOnTarget( CSpellActionBase *spellToTrigger, float chanceToTrigger, Enums::Enums Enums );
--
-- addTriggerSpellOnSelf will execute the spell on the player if triggered.
-- addTriggerSpellOnTarget will execute the spell on the current target on the player (if any).
--
-- Two Enumss to choose from so far, more might come: 
-- Enums.TAKING_DAMAGE - To be used if we want to trigger the spell when taking damage.
-- Enums.EXECUTING_ACTION - To be used if we want to trigger the spell when casting / attacking.
--
-- Example:
-- curItem:addTriggerSpellOnSelf( spellDatabase["leatherskin"], 0.05, Enums.TAKING_DAMAGE );
-- This will cast give the item a 5% chance to cast the spell leatherskin when we're taking damage.
-- ===============================================================================================

-- == item stacks ==
-- Itemstacks can be used to allow us to have several items of one sort in one item.
-- For example "Spellcharges" work this way now. If we want to let the player have 5 charges of an healing potion in one inventory position, then we need to set setMaxStackSize( 5 ) to that item.
-- Same goes for any other item we want to stack up.

items = {};

TextureAtlasCreator:get():init(1024, 1024);
items["axe2"] = DawnInterface.loadimage("res/items/Axe2.tga");
items["sword1"] = DawnInterface.loadimage("res/items/Sword1.tga");
items["ring3"] = DawnInterface.loadimage("res/items/Ring3.tga");
items["shield1"] = DawnInterface.loadimage("res/items/Shield1.tga");
items["axe1"] = DawnInterface.loadimage("res/items/Axe1.tga");
items["ring1"] = DawnInterface.loadimage("res/items/Ring1.tga");
items["ring2"] = DawnInterface.loadimage("res/items/Ring2.tga");
items["scroll1"] = DawnInterface.loadimage("res/items/Scroll1.tga");
items["potion1"] = DawnInterface.loadimage("res/items/Potion1.tga");
items["belt1"] = DawnInterface.loadimage("res/items/Belt1.tga");
items["spellbook1"] = DawnInterface.loadimage("res/items/Spellbook1.tga");
items["necklace1"] = DawnInterface.loadimage("res/items/Necklace1.tga");
items["dagger1"] = DawnInterface.loadimage("res/items/Dagger1.tga");
items["boots1"] = DawnInterface.loadimage("res/items/Boots1.tga");
items["food1"] = DawnInterface.loadimage("res/items/Food1.tga");
items["skin1"] = DawnInterface.loadimage("res/items/Skin1.tga");
items["web1"] = DawnInterface.loadimage("res/items/Web1.tga");
items["meat2"] = DawnInterface.loadimage("res/items/Meat2.tga");
items["food2"] = DawnInterface.loadimage("res/items/Food2.tga");
items["bow1"] = DawnInterface.loadimage("res/items/Bow1.tga");
items["spellbook2"] = DawnInterface.loadimage("res/items/Spellbook2.tga");
items["helmet1"] = DawnInterface.loadimage("res/items/Helmet1.tga");
items["fang1"] = DawnInterface.loadimage("res/items/Fang1.tga");
items["potion3"] = DawnInterface.loadimage("res/items/Potion3.tga");
TextureManager:SetTextureAtlas("items", TextureAtlasCreator:get():getAtlas());
TextureAtlasCreator:get():safeAtlas("tmp/items")

itemDatabase = {};

itemDatabase["axeofanguish"]=DawnInterface.createNewItem( "Axe of Anguish", 2, 3, "res/items/Axe2.tga", Enums.ENHANCED, EquipPosition.MAIN_HAND, Enums.WEAPON, Enums.NO_ARMOR, Enums.TWOHAND_AXE );
curItem = itemDatabase["axeofanguish"];
curItem:setStats( Enums.Strength, 2 );
curItem:setStats( Enums.HitModifier, 5 );
curItem:setMinDamage( 7 );
curItem:setMaxDamage( 15 );
curItem:setRequiredLevel( 1 );
curItem:setStats( Enums.MeleeCritical, 25 );
curItem:setValue( 56818 );
curItem:setItemTextureRect(items["axe2"]);

itemDatabase["arustysword"]=DawnInterface.createNewItem( "A rusty sword", 1, 3, "res/items/Sword1.tga", Enums.POOR, EquipPosition.MAIN_HAND, Enums.WEAPON, Enums.NO_ARMOR, Enums.ONEHAND_SWORD );
curItem = itemDatabase["arustysword"];
curItem:setStats( Enums.Strength, -1 );
curItem:setMinDamage( 3 );
curItem:setMaxDamage( 6 );
curItem:setValue( 80 );
curItem:setItemTextureRect(items["sword1"]);

itemDatabase["swordofkhazom"]=DawnInterface.createNewItem( "Sword of Khazom", 1, 3, "res/items/Sword1.tga", Enums.LORE, EquipPosition.MAIN_HAND, Enums.WEAPON, Enums.NO_ARMOR, Enums.ONEHAND_SWORD );
curItem = itemDatabase["swordofkhazom"];
curItem:setStats( Enums.Dexterity, 5 );
curItem:setStats( Enums.Strength, 10 );
curItem:setStats( Enums.Health, 75 );
curItem:setMinDamage( 10 );
curItem:setMaxDamage( 25 );
curItem:setDescription( "Once wielded by Irk the Unholy." );
curItem:setResistElementModifierPoints( Enums.Dark, 15 );
curItem:setValue( 101280 );
curItem:setItemTextureRect(items["sword1"]);

itemDatabase["shortsword"]=DawnInterface.createNewItem( "Shortsword", 1, 3, "res/items/Sword1.tga", Enums.NORMAL, EquipPosition.MAIN_HAND, Enums.WEAPON, Enums.NO_ARMOR, Enums.ONEHAND_SWORD );
curItem = itemDatabase["shortsword"];
curItem:setMinDamage( 6 );
curItem:setMaxDamage( 10 );
curItem:setValue( 100 );
curItem:setItemTextureRect(items["sword1"]);

itemDatabase["eyeoflicor"]=DawnInterface.createNewItem( "Eye of Licor", 1, 1, "res/items/Ring3.tga", Enums.RARE, EquipPosition.RING, Enums.JEWELRY, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["eyeoflicor"];
curItem:setStats( Enums.Intellect, 4 );
curItem:setStats( Enums.Strength, -1 );
curItem:setDescription( "No'dre entoras lictor." );
curItem:setValue( 1090 );
curItem:setItemTextureRect(items["ring3"]);

itemDatabase["gnollshield"]=DawnInterface.createNewItem( "Gnoll shield", 2, 2, "res/items/Shield1.tga", Enums.ENHANCED, EquipPosition.OFF_HAND, Enums.WEAPON, Enums.NO_ARMOR, Enums.SHIELD );
curItem = itemDatabase["gnollshield"];
curItem:setStats( Enums.Vitality, 3 );
curItem:setStats( Enums.ParryModifier, 3 );
curItem:setStats( Enums.Armor, 70 );
curItem:addTriggerSpellOnSelf( spellDatabase["steelgrip"], 0.05, Enums.TAKING_DAMAGE );
curItem:setValue( 225 );
curItem:setItemTextureRect(items["shield1"]);

itemDatabase["gutteraxe"]=DawnInterface.createNewItem( "Gutteraxe", 2, 3, "res/items/Axe1.tga", Enums.RARE, EquipPosition.MAIN_HAND, Enums.WEAPON, Enums.NO_ARMOR, Enums.TWOHAND_AXE );
curItem = itemDatabase["gutteraxe"];
curItem:setStats( Enums.Dexterity, 10 );
curItem:setStats( Enums.Strength, 10 );
curItem:setMinDamage( 32 );
curItem:setMaxDamage( 81 );
curItem:setRequiredLevel( 5 );
curItem:setValue( 11500 );
curItem:setItemTextureRect(items["axe1"]);

itemDatabase["snakeloop"]=DawnInterface.createNewItem( "Snakeloop", 1, 1, "res/items/Ring1.tga", Enums.ENHANCED, EquipPosition.RING, Enums.JEWELRY, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["snakeloop"];
curItem:setStats( Enums.Intellect, 3 );
curItem:setRequiredLevel( 1 );
curItem:setSpellEffectElementModifierPoints( Enums.Light, 20 );
curItem:setValue( 2500 );
curItem:setItemTextureRect(items["ring1"]);

itemDatabase["bladeofstrength"]=DawnInterface.createNewItem( "Blade of Strength", 1, 3, "res/items/Sword1.tga", Enums.ENHANCED, EquipPosition.MAIN_HAND, Enums.WEAPON, Enums.NO_ARMOR, Enums.ONEHAND_SWORD );
curItem = itemDatabase["bladeofstrength"];
curItem:setStats( Enums.Strength, 4 );
curItem:setStats( Enums.Health, 5 );
curItem:setMinDamage( 5 );
curItem:setMaxDamage( 13 );
curItem:setRequiredLevel( 3 );
curItem:setValue( 300 );
curItem:setItemTextureRect(items["sword1"]);

itemDatabase["ringofdefense"]=DawnInterface.createNewItem( "Ring of Defense", 1, 1, "res/items/Ring1.tga", Enums.ENHANCED, EquipPosition.RING, Enums.JEWELRY, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["ringofdefense"];
curItem:setStats( Enums.Armor, 30 );
curItem:setStats( Enums.EvadeModifier, 5 );
curItem:setStats( Enums.ResistAll, 1 );
curItem:setValue( 215 );
curItem:setItemTextureRect(items["ring1"]);

itemDatabase["shadering"]=DawnInterface.createNewItem( "Shade ring", 1, 1, "res/items/Ring2.tga", Enums.ENHANCED, EquipPosition.RING, Enums.JEWELRY, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["shadering"];
curItem:setStats( Enums.Dexterity, 2 );
curItem:setStats( Enums.EvadeModifier, 10 );
curItem:setRequiredLevel( 3 );
curItem:setValue( 110 );
curItem:setItemTextureRect(items["ring2"]);

itemDatabase["scrolloftheboar"]=DawnInterface.createNewItem( "Scroll of the Boar", 1, 1, "res/items/Scroll1.tga", Enums.NORMAL, EquipPosition.NONE, Enums.SCROLL, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["scrolloftheboar"];
curItem:setRequiredLevel( 1 );
curItem:setSpell( spellDatabase["spiritoftheboar"] );
curItem:setValue( 100 );
curItem:setMaxStackSize( 5 );
curItem:setItemTextureRect(items["scroll1"]);

itemDatabase["smallhealingpotion"]=DawnInterface.createNewItem( "Small healing potion", 1, 1, "res/items/Potion1.tga", Enums.NORMAL, EquipPosition.NONE, Enums.POTION, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["smallhealingpotion"];
curItem:setSpell( spellDatabase["smallhealingpotion"] );
curItem:setValue( 500 );
curItem:setMaxStackSize( 5 );
curItem:setItemTextureRect(items["potion1"]);

itemDatabase["tornleatherbelt"]=DawnInterface.createNewItem( "Torn leather belt", 2, 1, "res/items/Belt1.tga", Enums.NORMAL, EquipPosition.BELT, Enums.ARMOR, Enums.LEATHER, Enums.NO_WEAPON );
curItem = itemDatabase["tornleatherbelt"];
curItem:setStats( Enums.Armor, 12 );
curItem:setValue( 86 );
curItem:setItemTextureRect(items["belt1"]);

itemDatabase["bookofmagicmissilerank2"]=DawnInterface.createNewItem( "Book of Magic Missile", 2, 2, "res/items/Spellbook1.tga", Enums.NORMAL, EquipPosition.NONE, Enums.NEWSPELL, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["bookofmagicmissilerank2"];
curItem:setSpell( spellDatabase["magicmissilerank2"] );
curItem:setValue( 2500 );
curItem:setItemTextureRect(items["spellbook1"]);

itemDatabase["coppernecklace"]=DawnInterface.createNewItem( "Copper necklace", 1, 1, "res/items/Necklace1.tga", Enums.ENHANCED, EquipPosition.AMULET,Enums.JEWELRY, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["coppernecklace"];
curItem:setStats( Enums.Vitality, 1 );
curItem:setStats( Enums.Wisdom, 1 );
curItem:setValue( 20 );
curItem:setItemTextureRect(items["necklace1"]);

itemDatabase["daggerofflowingthought"]=DawnInterface.createNewItem( "Dagger of flowing thought", 1, 2, "res/items/Dagger1.tga", Enums.RARE, EquipPosition.MAIN_HAND, Enums.WEAPON, Enums.NO_ARMOR, Enums.DAGGER );
curItem = itemDatabase["daggerofflowingthought"];
curItem:setStats( Enums.SpellCritical, 7 );
curItem:setStats( Enums.Wisdom, 3 );
curItem:setStats( Enums.ManaRegen, 2 );
curItem:setMinDamage( 2 );
curItem:setMaxDamage( 6 );
curItem:setValue( 216 );
curItem:addTriggerSpellOnSelf( spellDatabase["flowingthought"], 0.1, Enums.EXECUTING_ACTION );
curItem:setItemTextureRect(items["dagger1"]);

itemDatabase["fungalboots"]=DawnInterface.createNewItem( "Fungal boots", 2, 2, "res/items/Boots1.tga", Enums.ENHANCED, EquipPosition.BOOTS, Enums.ARMOR, Enums.LEATHER, Enums.NO_WEAPON );
curItem = itemDatabase["fungalboots"];
curItem:setStats( Enums.Armor, 7 );
curItem:setStats( Enums.HealthRegen, 2 );
curItem:setValue( 78 );
curItem:setItemTextureRect(items["boots1"]);

itemDatabase["bookofleatherskinrank2"]=DawnInterface.createNewItem( "Book of Leatherskin", 2, 2, "res/items/Spellbook1.tga", Enums.NORMAL, EquipPosition.NONE, Enums.NEWSPELL, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["bookofleatherskinrank2"];
curItem:setSpell( spellDatabase["leatherskinrank2"] );
curItem:setValue( 2500 );
curItem:setItemTextureRect(items["spellbook1"]);

itemDatabase["ajuicyapple"]=DawnInterface.createNewItem( "A juicy apple", 1, 1, "res/items/Food1.tga", Enums.NORMAL, EquipPosition.NONE, Enums.FOOD, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["ajuicyapple"];
curItem:setSpell( spellDatabase["food"] );
curItem:setValue( 25 );
curItem:setMaxStackSize( 5 );
curItem:setItemTextureRect(items["food1"]);

itemDatabase["patchofwolfskin"]=DawnInterface.createNewItem( "Patch of wolf skin", 1, 1, "res/items/Skin1.tga", Enums.POOR, EquipPosition.NONE, Enums.MISCELLANEOUS, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["patchofwolfskin"];
curItem:setValue( 5 );
curItem:setMaxStackSize( 20 );
curItem:setItemTextureRect(items["skin1"]);

itemDatabase["spidersilk"]=DawnInterface.createNewItem( "Spidersilk", 1, 1, "res/items/Web1.tga", Enums.NORMAL, EquipPosition.NONE, Enums.MISCELLANEOUS, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["spidersilk"];
curItem:setValue( 16 );
curItem:setMaxStackSize( 20 );
curItem:setItemTextureRect(items["web1"]);

itemDatabase["wolfmeat"]=DawnInterface.createNewItem( "Wolf meat", 1, 1, "res/items/Meat2.tga", Enums.NORMAL, EquipPosition.NONE, Enums.MISCELLANEOUS, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["wolfmeat"];
curItem:setValue( 10 );
curItem:setMaxStackSize( 20 );
curItem:setItemTextureRect(items["meat2"]);

itemDatabase["awaterpouch"]=DawnInterface.createNewItem( "A waterpouch", 1, 1, "res/items/Food2.tga", Enums.NORMAL, EquipPosition.NONE, Enums.FOOD, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["awaterpouch"];
curItem:setSpell( spellDatabase["drink"] );
curItem:setValue( 40 );
curItem:setMaxStackSize( 5 );
curItem:setItemTextureRect(items["food2"]);

itemDatabase["weakenedbow"]=DawnInterface.createNewItem( "Weakened bow", 2, 3, "res/items/Bow1.tga", Enums.POOR, EquipPosition.MAIN_HAND, Enums.WEAPON, Enums.NO_ARMOR, Enums.BOW );
curItem = itemDatabase["weakenedbow"];
curItem:setMinDamage( 4 );
curItem:setMaxDamage( 7 );
curItem:setValue( 78 );
curItem:setItemTextureRect(items["bow1"]);

itemDatabase["moldytome"]=DawnInterface.createNewItem( "Moldy tome", 2, 2, "res/items/Spellbook2.tga", Enums.ENHANCED, EquipPosition.OFF_HAND, Enums.WEAPON, Enums.NO_ARMOR, Enums.SPELLBOOK );
curItem = itemDatabase["moldytome"];
curItem:setSpellEffectElementModifierPoints( Enums.Dark, 6 );
curItem:setStats( Enums.Wisdom, 2 );
curItem:setStats( Enums.ManaRegen, 1 );
curItem:setItemTextureRect(items["spellbook2"]);

itemDatabase["leynorscap"]=DawnInterface.createNewItem( "Leynor's Cap", 2, 2, "res/items/Helmet1.tga", Enums.ENHANCED, EquipPosition.HEAD, Enums.ARMOR, Enums.CLOTH, Enums.NO_WEAPON );
curItem = itemDatabase["leynorscap"];
curItem:setStats( Enums.Armor, 4 );
curItem:setValue( 276 );
curItem:setStats( Enums.Wisdom, 2 );
curItem:setStats( Enums.Intellect, 1 );
curItem:setItemTextureRect(items["helmet1"]);

itemDatabase["pristinesylphirsilk"]=DawnInterface.createNewItem( "Pristine Sylphir spidersilk", 1, 1, "res/items/Web1.tga", Enums.ENHANCED, EquipPosition.NONE, Enums.QUESTITEM, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["pristinesylphirsilk"];
curItem:setMaxStackSize( 20 );
curItem:setItemTextureRect(items["web1"]);

itemDatabase["sylphirfang"]=DawnInterface.createNewItem( "Sylphir fang", 1, 1, "res/items/Fang1.tga", Enums.NORMAL, EquipPosition.NONE, Enums.QUESTITEM, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["sylphirfang"];
curItem:setMaxStackSize( 20 );
curItem:setItemTextureRect(items["fang1"]);

itemDatabase["sylphirantidote"]=DawnInterface.createNewItem( "Sylphir antidote", 1, 1, "res/items/Potion3.tga", Enums.ENHANCED, EquipPosition.NONE, Enums.QUESTITEM, Enums.NO_ARMOR, Enums.NO_WEAPON );
curItem = itemDatabase["sylphirantidote"];
curItem:setMaxStackSize( 1 );
curItem:setDescription( "Brewed by Jemma the Wicked." );
curItem:setItemTextureRect(items["potion3"]);
