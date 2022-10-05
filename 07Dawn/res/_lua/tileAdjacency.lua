local tileSet = EditorInterface.getTileSet(Enums.ENVIRONMENT);

-- only add adjacency information if the tiles are currently defined which should mean they exist in the current tileset
function addAdjacencyIfExists( tile1, Enums, tile2, offsetX, offsetY )
	if ( tile1 == nil or tile2 == nil )
	then
		return;
	else
		if ( offsetX == nil )
		then
			offsetX = 0;
		end
		if ( offsetY == nil )
		then
			offsetY = 0;
		end
		tileSet:addAdjacency( tile1, Enums, tile2, offsetX, offsetY );
	end
end

function addTileToEquivalenceClassIfExists( equivalenceClass, tile, offsetX, offsetY )
	if ( tile == nil )
	then
		return;
	else
		if ( offsetX == nil )
		then
			offsetX = 0;
		end
		if ( offsetY == nil )
		then
			offsetY = 0;
		end
		equivalenceClass:addEquivalentTile( tile, offsetX, offsetY );
	end
end

-- create adjacence equivalence classes
-- This are groups of tiles where each can be exchanged by another with respect to the tiles that fit next to them

-- all dungeon walls of houseinterior tile group

-- left top tiles <--> centre top tiles <--> right top tiles
--
-- top left tiles                                  top right tiles
--         /\                                              /\
--         ||                                              ||
--         \/                                              \/
-- centre left tiles                               centre right tiles
--         /\                                              /\
--         ||                                              ||
--         \/                                              \/
-- bottom left tiles                               bottom right tiles
--
-- left bottom tiles <--> centre bottom tiles <--> right bottom tiles

-- left top tiles 
local houseInteriorLeftTop = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( houseInteriorLeftTop, MapData.DontSave.houseinterior_insidewall1_tga );
addTileToEquivalenceClassIfExists( houseInteriorLeftTop, MapData.DontSave.houseinterior_insidewall14_tga );
addTileToEquivalenceClassIfExists( houseInteriorLeftTop, MapData.DontSave.houseinterior_insidewalldamaged1_tga );
addTileToEquivalenceClassIfExists( houseInteriorLeftTop, MapData.DontSave.houseinterior_insidewalldamaged14_tga );

-- centre top tiles
local houseInteriorCentreTop = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( houseInteriorCentreTop, MapData.DontSave.houseinterior_insidewall2_tga );
addTileToEquivalenceClassIfExists( houseInteriorCentreTop, MapData.DontSave.houseinterior_insidewall3_tga );
addTileToEquivalenceClassIfExists( houseInteriorCentreTop, MapData.DontSave.houseinterior_insidewalldamaged2_tga );
addTileToEquivalenceClassIfExists( houseInteriorCentreTop, MapData.DontSave.houseinterior_insidewalldamaged3_tga );
addTileToEquivalenceClassIfExists( houseInteriorCentreTop, MapData.DontSave.houseinterior_insidewall_door_tga );

-- right top tiles
local houseInteriorRightTop = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( houseInteriorRightTop, MapData.DontSave.houseinterior_insidewall4_tga );
addTileToEquivalenceClassIfExists( houseInteriorRightTop, MapData.DontSave.houseinterior_insidewall13_tga );
addTileToEquivalenceClassIfExists( houseInteriorRightTop, MapData.DontSave.houseinterior_insidewalldamaged4_tga );
addTileToEquivalenceClassIfExists( houseInteriorRightTop, MapData.DontSave.houseinterior_insidewalldamaged13_tga );

-- add equivalence rules for X top tiles
tileSet:addEquivalenceAdjacency( houseInteriorCentreTop, Enums.RIGHT, houseInteriorCentreTop, 0, 0 );
tileSet:addEquivalenceAdjacency( houseInteriorCentreTop, Enums.RIGHT, houseInteriorRightTop, 0, 0 );
tileSet:addEquivalenceAdjacency( houseInteriorLeftTop, Enums.RIGHT, houseInteriorCentreTop, 0, 0 );
tileSet:addEquivalenceAdjacency( houseInteriorLeftTop, Enums.RIGHT, houseInteriorRightTop, 0, 0 );

-- left tiles are with shadow to the right
-- top left tiles
local houseInteriorTopLeft = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( houseInteriorTopLeft, MapData.DontSave.houseinterior_insidewall1_tga, 0, -40 );
addTileToEquivalenceClassIfExists( houseInteriorTopLeft, MapData.DontSave.houseinterior_insidewalldamaged1_tga, 0, -40 );
addTileToEquivalenceClassIfExists( houseInteriorTopLeft, MapData.DontSave.houseinterior_insidewall15_tga, -11, 0 );
addTileToEquivalenceClassIfExists( houseInteriorTopLeft, MapData.DontSave.houseinterior_insidewalldamaged15_tga, -11, 0 );

-- centre left tiles
local houseInteriorCentreLeft = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( houseInteriorCentreLeft, MapData.DontSave.houseinterior_insidewall7_tga, 5, 0 );
addTileToEquivalenceClassIfExists( houseInteriorCentreLeft, MapData.DontSave.houseinterior_insidewalldamaged7_tga, 5, 0 );

-- bottom left tiles
local houseInteriorBottomLeft = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( houseInteriorBottomLeft, MapData.DontSave.houseinterior_insidewall9_tga, 0, 0 );
addTileToEquivalenceClassIfExists( houseInteriorBottomLeft, MapData.DontSave.houseinterior_insidewalldamaged9_tga, 0, 0 );
addTileToEquivalenceClassIfExists( houseInteriorBottomLeft, MapData.DontSave.houseinterior_insidewall13_tga, -11, 0 );
addTileToEquivalenceClassIfExists( houseInteriorBottomLeft, MapData.DontSave.houseinterior_insidewalldamaged13_tga, -11, 0 );

tileSet:addEquivalenceAdjacency( houseInteriorCentreLeft, Enums.TOP, houseInteriorCentreLeft, 0, 0 );
tileSet:addEquivalenceAdjacency( houseInteriorCentreLeft, Enums.TOP, houseInteriorTopLeft, 0, 0 );
tileSet:addEquivalenceAdjacency( houseInteriorBottomLeft, Enums.TOP, houseInteriorCentreLeft, 0, 0 );
tileSet:addEquivalenceAdjacency( houseInteriorBottomLeft, Enums.TOP, houseInteriorTopLeft, 0, 0 );

-- right tiles are without shadow to the right

-- top right tiles
local houseInteriorTopRight = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( houseInteriorTopRight, MapData.DontSave.houseinterior_insidewall4_tga, 0, -40 );
addTileToEquivalenceClassIfExists( houseInteriorTopRight, MapData.DontSave.houseinterior_insidewalldamaged4_tga, 0, -40 );
addTileToEquivalenceClassIfExists( houseInteriorTopRight, MapData.DontSave.houseinterior_insidewall16_tga, 37, 0 );
addTileToEquivalenceClassIfExists( houseInteriorTopRight, MapData.DontSave.houseinterior_insidewalldamaged16_tga, 37, 0 );

-- centre right tiles
local houseInteriorCentreRight = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( houseInteriorCentreRight, MapData.DontSave.houseinterior_insidewall5_tga, 42, 0 );
addTileToEquivalenceClassIfExists( houseInteriorCentreRight, MapData.DontSave.houseinterior_insidewalldamaged5_tga, 42, 0 );
addTileToEquivalenceClassIfExists( houseInteriorCentreRight, MapData.DontSave.houseinterior_insidewall6_tga, 42, 0 );
addTileToEquivalenceClassIfExists( houseInteriorCentreRight, MapData.DontSave.houseinterior_insidewalldamaged6_tga, 41, 0 );
addTileToEquivalenceClassIfExists( houseInteriorCentreRight, MapData.DontSave.houseinterior_insidewall8_tga, 42, 0 );
addTileToEquivalenceClassIfExists( houseInteriorCentreRight, MapData.DontSave.houseinterior_insidewalldamaged8_tga, 41, 0 );

-- bottom right tiles
local houseInteriorBottomRight = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( houseInteriorBottomRight, MapData.DontSave.houseinterior_insidewall12_tga, 0, 0 );
addTileToEquivalenceClassIfExists( houseInteriorBottomRight, MapData.DontSave.houseinterior_insidewalldamaged12_tga, 0, 0 );
addTileToEquivalenceClassIfExists( houseInteriorBottomRight, MapData.DontSave.houseinterior_insidewall14_tga, 33, 0 );
addTileToEquivalenceClassIfExists( houseInteriorBottomRight, MapData.DontSave.houseinterior_insidewalldamaged14_tga, 33, 0 );

tileSet:addEquivalenceAdjacency( houseInteriorCentreRight, Enums.TOP, houseInteriorCentreRight, 0, 0 );
tileSet:addEquivalenceAdjacency( houseInteriorCentreRight, Enums.TOP, houseInteriorTopRight, 0, 0 );
tileSet:addEquivalenceAdjacency( houseInteriorBottomRight, Enums.TOP, houseInteriorCentreRight, 0, 0 );
tileSet:addEquivalenceAdjacency( houseInteriorBottomRight, Enums.TOP, houseInteriorTopRight, 0, 0 );

-- bottom tiles

-- left bottom tiles
local houseInteriorLeftBottom = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( houseInteriorLeftBottom, MapData.DontSave.houseinterior_insidewall9_tga, 0, 0 );
addTileToEquivalenceClassIfExists( houseInteriorLeftBottom, MapData.DontSave.houseinterior_insidewalldamaged9_tga, 0, 0 );
addTileToEquivalenceClassIfExists( houseInteriorLeftBottom, MapData.DontSave.houseinterior_insidewall16_tga, 0, -6 );
addTileToEquivalenceClassIfExists( houseInteriorLeftBottom, MapData.DontSave.houseinterior_insidewalldamaged16_tga, 0, -6 );

-- centre bottom tiles
local houseInteriorCentreBottom = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( houseInteriorCentreBottom, MapData.DontSave.houseinterior_insidewall10_tga, 0, 0 );
addTileToEquivalenceClassIfExists( houseInteriorCentreBottom, MapData.DontSave.houseinterior_insidewalldamaged10_tga, 0, 0 );
addTileToEquivalenceClassIfExists( houseInteriorCentreBottom, MapData.DontSave.houseinterior_insidewall11_tga, 0, -2 );
addTileToEquivalenceClassIfExists( houseInteriorCentreBottom, MapData.DontSave.houseinterior_insidewalldamaged11_tga, 0, -2 );

-- right bottom tiles
local houseInteriorRightBottom = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( houseInteriorRightBottom, MapData.DontSave.houseinterior_insidewall12_tga, 0, -3 );
addTileToEquivalenceClassIfExists( houseInteriorRightBottom, MapData.DontSave.houseinterior_insidewalldamaged12_tga, 0, -3 );
addTileToEquivalenceClassIfExists( houseInteriorRightBottom, MapData.DontSave.houseinterior_insidewall15_tga, 0, -5 );
addTileToEquivalenceClassIfExists( houseInteriorRightBottom, MapData.DontSave.houseinterior_insidewalldamaged15_tga, 0, -5 );

tileSet:addEquivalenceAdjacency( houseInteriorCentreBottom, Enums.RIGHT, houseInteriorCentreBottom, 0, 0 );
tileSet:addEquivalenceAdjacency( houseInteriorCentreBottom, Enums.RIGHT, houseInteriorRightBottom, 0, 0 );
tileSet:addEquivalenceAdjacency( houseInteriorLeftBottom, Enums.RIGHT, houseInteriorCentreBottom, 0, 0 );
tileSet:addEquivalenceAdjacency( houseInteriorLeftBottom, Enums.RIGHT, houseInteriorRightBottom, 0, 0 );


-- village houses

local villageHouseTopLeft = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( villageHouseTopLeft, MapData.DontSave.village_village1_tga );

local villageHouseLeft = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( villageHouseLeft, MapData.DontSave.village_village2_tga );

local villageHouseBottomLeft = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( villageHouseBottomLeft, MapData.DontSave.village_village3_tga );

local villageHouseTop = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( villageHouseTop, MapData.DontSave.village_village4_tga );

local villageHouseCentre = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( villageHouseCentre, MapData.DontSave.village_village5_tga );

local villageHouseBottom = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( villageHouseBottom, MapData.DontSave.village_village6_tga );
addTileToEquivalenceClassIfExists( villageHouseBottom, MapData.DontSave.village_village10_tga, 0, 11 );
addTileToEquivalenceClassIfExists( villageHouseBottom, MapData.DontSave.village_village11_tga );
addTileToEquivalenceClassIfExists( villageHouseBottom, MapData.DontSave.village_village12_tga );

local villageHouseTopRight = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( villageHouseTopRight, MapData.DontSave.village_village7_tga );

local villageHouseRight = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( villageHouseRight, MapData.DontSave.village_village8_tga );

local villageHouseBottomRight = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( villageHouseBottomRight, MapData.DontSave.village_village9_tga );

-- adjacency of village house tiles
tileSet:addEquivalenceAdjacency( villageHouseTop, Enums.RIGHT, villageHouseTop, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseTop, Enums.RIGHT, villageHouseTopRight, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseTopLeft, Enums.RIGHT, villageHouseTop, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseTopLeft, Enums.RIGHT, villageHouseTopRight, 0, 0 );

tileSet:addEquivalenceAdjacency( villageHouseCentre, Enums.RIGHT, villageHouseCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseCentre, Enums.RIGHT, villageHouseRight, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseLeft, Enums.RIGHT, villageHouseCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseLeft, Enums.RIGHT, villageHouseRight, 0, 0 );

tileSet:addEquivalenceAdjacency( villageHouseBottom, Enums.RIGHT, villageHouseBottom, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseBottom, Enums.RIGHT, villageHouseBottomRight, 0, 1 );
tileSet:addEquivalenceAdjacency( villageHouseBottomLeft, Enums.RIGHT, villageHouseBottom, 0, -3 );
tileSet:addEquivalenceAdjacency( villageHouseBottomLeft, Enums.RIGHT, villageHouseBottomRight, 0, -2 );

tileSet:addEquivalenceAdjacency( villageHouseLeft, Enums.TOP, villageHouseLeft, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseLeft, Enums.TOP, villageHouseTopLeft, 1, 0 );
tileSet:addEquivalenceAdjacency( villageHouseBottomLeft, Enums.TOP, villageHouseLeft, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseBottomLeft, Enums.TOP, villageHouseTopLeft, 1, 0 );

tileSet:addEquivalenceAdjacency( villageHouseCentre, Enums.TOP, villageHouseCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseCentre, Enums.TOP, villageHouseTop, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseBottom, Enums.TOP, villageHouseCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseBottom, Enums.TOP, villageHouseTop, 0, 0 );

tileSet:addEquivalenceAdjacency( villageHouseRight, Enums.TOP, villageHouseRight, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseRight, Enums.TOP, villageHouseTopRight, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseBottomRight, Enums.TOP, villageHouseRight, 0, 0 );
tileSet:addEquivalenceAdjacency( villageHouseBottomRight, Enums.TOP, villageHouseTopRight, 0, 0 );

-- village fence
local villageFenceHorizontalCentre = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( villageFenceHorizontalCentre, MapData.DontSave.village_fence2_tga );
addTileToEquivalenceClassIfExists( villageFenceHorizontalCentre, MapData.DontSave.village_fence6_tga );
addTileToEquivalenceClassIfExists( villageFenceHorizontalCentre, MapData.DontSave.village_fence7_tga );

local villageFenceLeft = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( villageFenceLeft, MapData.DontSave.village_fence1_tga, 0, 15 );
--addTileToEquivalenceClassIfExists( villageFenceLeft, MapData.DontSave.village_fence9_tga );

local villageFenceRight = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( villageFenceRight, MapData.DontSave.village_fence3_tga );
addTileToEquivalenceClassIfExists( villageFenceRight, MapData.DontSave.village_fence8_tga );
addTileToEquivalenceClassIfExists( villageFenceRight, MapData.DontSave.village_fence10_tga, 0, 15 );

-- adjacency of village fence
tileSet:addEquivalenceAdjacency( villageFenceHorizontalCentre, Enums.RIGHT, villageFenceHorizontalCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( villageFenceHorizontalCentre, Enums.RIGHT, villageFenceRight, 0, 0 );
tileSet:addEquivalenceAdjacency( villageFenceLeft, Enums.RIGHT, villageFenceHorizontalCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( villageFenceLeft, Enums.RIGHT, villageFenceRight, 0, 0 );

addAdjacencyIfExists( MapData.DontSave.village_fence4_tga, Enums.TOP, MapData.DontSave.village_fence4_tga, 0, -16 )
addAdjacencyIfExists( MapData.DontSave.village_fence4_tga, Enums.TOP, MapData.DontSave.village_fence5_tga, 0, -16 )
--addAdjacencyIfExists( MapData.DontSave.village_fence4_tga, Enums.TOP, MapData.DontSave.village_fence9_tga, -7, -16 )
addAdjacencyIfExists( MapData.DontSave.village_fence4_tga, Enums.TOP, MapData.DontSave.village_fence10_tga, -7, -40 )
addAdjacencyIfExists( MapData.DontSave.village_fence5_tga, Enums.TOP, MapData.DontSave.village_fence4_tga, 0, -21 )
addAdjacencyIfExists( MapData.DontSave.village_fence5_tga, Enums.TOP, MapData.DontSave.village_fence5_tga, 0, -21 )
--addAdjacencyIfExists( MapData.DontSave.village_fence5_tga, Enums.TOP, MapData.DontSave.village_fence9_tga, -7, -21 )
addAdjacencyIfExists( MapData.DontSave.village_fence5_tga, Enums.TOP, MapData.DontSave.village_fence10_tga, -7, -45 )

addAdjacencyIfExists( MapData.DontSave.village_fence4_tga, Enums.BOTTOM, MapData.DontSave.village_fence1_tga, -6, 32 )
addAdjacencyIfExists( MapData.DontSave.village_fence4_tga, Enums.BOTTOM, MapData.DontSave.village_fence3_tga, -6, 17 )
addAdjacencyIfExists( MapData.DontSave.village_fence5_tga, Enums.BOTTOM, MapData.DontSave.village_fence1_tga, -6, 32 )
addAdjacencyIfExists( MapData.DontSave.village_fence5_tga, Enums.BOTTOM, MapData.DontSave.village_fence3_tga, -6, 17 )

addAdjacencyIfExists( MapData.DontSave.village_fence9_tga, Enums.BOTTOM, MapData.DontSave.village_fence1_tga, 0, 32 )
addAdjacencyIfExists( MapData.DontSave.village_fence9_tga, Enums.BOTTOM, MapData.DontSave.village_fence3_tga, 1, 20 )
addAdjacencyIfExists( MapData.DontSave.village_fence9_tga, Enums.BOTTOM, MapData.DontSave.village_fence8_tga, 3, 40 )
addAdjacencyIfExists( MapData.DontSave.village_fence10_tga, Enums.BOTTOM, MapData.DontSave.village_fence1_tga, 0, 56 )
addAdjacencyIfExists( MapData.DontSave.village_fence10_tga, Enums.BOTTOM, MapData.DontSave.village_fence3_tga, 0, 41 )

-- cliffs
local cliffsTopLeftCorner = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( cliffsTopLeftCorner, MapData.DontSave.cliffs_cliff9_tga );

local cliffsTopCentre = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( cliffsTopCentre, MapData.DontSave.cliffs_cliff10_tga );

local cliffsTopRightCorner = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( cliffsTopRightCorner, MapData.DontSave.cliffs_cliff12_tga );

local cliffsRightBeforeTopCorner = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( cliffsRightBeforeTopCorner, MapData.DontSave.cliffs_cliff6_tga );

local cliffsRightCentre = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( cliffsRightCentre, MapData.DontSave.cliffs_cliff5_tga );

local cliffsBottomRightCorner = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( cliffsBottomRightCorner, MapData.DontSave.cliffs_cliff3_tga );

local cliffsBottomCentre = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( cliffsBottomCentre, MapData.DontSave.cliffs_cliff2_tga );
addTileToEquivalenceClassIfExists( cliffsBottomCentre, MapData.DontSave.cliffs_cliff4_tga, 0, 1 );

local cliffsBottomLeftCorner = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( cliffsBottomLeftCorner, MapData.DontSave.cliffs_cliff1_tga );

local cliffsLeftCentre = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( cliffsLeftCentre, MapData.DontSave.cliffs_cliff7_tga );
addTileToEquivalenceClassIfExists( cliffsLeftCentre, MapData.DontSave.cliffs_cliff8_tga );

local cliffsCentre = tileSet:createAdjacencyEquivalenceClass();
addTileToEquivalenceClassIfExists( cliffsCentre, MapData.DontSave.cliffs_cliff13_tga );
addTileToEquivalenceClassIfExists( cliffsCentre, MapData.DontSave.cliffs_cliff14_tga );
addTileToEquivalenceClassIfExists( cliffsCentre, MapData.DontSave.cliffs_cliff15_tga );
addTileToEquivalenceClassIfExists( cliffsCentre, MapData.DontSave.cliffs_cliff16_tga );
addTileToEquivalenceClassIfExists( cliffsCentre, MapData.DontSave.cliffs_cliff17_tga );

-- cliffs adjacency information
tileSet:addEquivalenceAdjacency( cliffsTopCentre, Enums.RIGHT, cliffsTopCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsTopCentre, Enums.RIGHT, cliffsTopRightCorner, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsTopLeftCorner, Enums.RIGHT, cliffsTopCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsTopLeftCorner, Enums.RIGHT, cliffsTopRightCorner, 0, 0 );

tileSet:addEquivalenceAdjacency( cliffsBottomCentre, Enums.RIGHT, cliffsBottomCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsBottomCentre, Enums.RIGHT, cliffsBottomRightCorner, 0, -1 );
tileSet:addEquivalenceAdjacency( cliffsBottomLeftCorner, Enums.RIGHT, cliffsBottomCentre, 0, -1 );
tileSet:addEquivalenceAdjacency( cliffsBottomLeftCorner, Enums.RIGHT, cliffsBottomRightCorner, 0, 0 );

tileSet:addEquivalenceAdjacency( cliffsCentre, Enums.RIGHT, cliffsCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsCentre, Enums.RIGHT, cliffsRightCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsCentre, Enums.RIGHT, cliffsRightBeforeTopCorner, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsLeftCentre, Enums.RIGHT, cliffsCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsLeftCentre, Enums.RIGHT, cliffsRightCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsLeftCentre, Enums.RIGHT, cliffsRightBeforeTopCorner, 0, 0 );

tileSet:addEquivalenceAdjacency( cliffsRightCentre, Enums.TOP, cliffsRightCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsRightCentre, Enums.TOP, cliffsRightBeforeTopCorner, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsRightBeforeTopCorner, Enums.TOP, cliffsTopRightCorner, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsBottomRightCorner, Enums.TOP, cliffsRightCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsBottomRightCorner, Enums.TOP, cliffsRightBeforeTopCorner, 0, 0 );

tileSet:addEquivalenceAdjacency( cliffsLeftCentre, Enums.TOP, cliffsLeftCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsLeftCentre, Enums.TOP, cliffsTopLeftCorner, 8, 0 );
tileSet:addEquivalenceAdjacency( cliffsBottomLeftCorner, Enums.TOP, cliffsLeftCentre, 8, 0 );
tileSet:addEquivalenceAdjacency( cliffsBottomLeftCorner, Enums.TOP, cliffsTopLeftCorner, 16, 0 );

tileSet:addEquivalenceAdjacency( cliffsCentre, Enums.TOP, cliffsCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsCentre, Enums.TOP, cliffsTopCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsBottomCentre, Enums.TOP, cliffsCentre, 0, 0 );
tileSet:addEquivalenceAdjacency( cliffsBottomCentre, Enums.TOP, cliffsTopCentre, 0, 0 );

--tileSet:printTileSet();

