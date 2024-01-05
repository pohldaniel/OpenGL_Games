function saveGlobals( t )	
	for name, value in pairs(_G) do	
		t[name] = 1
	end
end

restoreQuest = {};
restoreInit = {};
hash = {};
restoreTables = {};
hashTables = {};

function printTables( surroundingName, varname, value )
local prefix=''
	if( surroundingName ) then
		prefix=surroundingName..'.'
	end

	if( type(value) == "table" ) then
		if (not hashTables[prefix..varname]) then
				restoreTables[#restoreTables+1] = prefix..varname..'='..'{}'
				hashTables[prefix..varname] = true
		end
		
		if( varname ~= "DontSave" ) then
			for innervarname,innervalue in pairs(value) do
				printTables(prefix..varname, innervarname, innervalue )
			end
		end
	end
end

function printValue( surroundingName, varname, value )
	local prefix=''
	if( surroundingName ) then
		prefix=surroundingName..'.'
	end

	if(varname == "quest" or varname == "quest_one" or varname == "quest_two") then
		local restoreString = DawnInterface.getItemReferenceRestore(prefix..varname, value );
		restoreQuest[prefix..varname]=restoreString;
		return
	end
	
	if( value == nil ) then
		io.write( prefix..varname..'='..'nil'..'\n' )
	elseif( type(value) == "number" ) then
		io.write( prefix..varname..'='..value..'\n' )
	elseif( type(value) == "boolean" ) then
		if( value == true ) then
			io.write( prefix..varname..'='..'true'..'\n' )
			if(varname == "additionalload") then
				io.write(prefix..'additionalinit()\n')
			end
		else
			io.write( prefix..varname..'='..'false'..'\n' )
		end
	elseif( type(value) == "string" ) then
		io.write( prefix..varname..'='..'"'..value..'"'..'\n' )
		if(varname == "path") then
			if (not hash[value]) then
				restoreInit[#restoreInit+1] = "dofile('"..value.."')"
				hash[value] = true
			end
		end
	elseif( type(value) == "table" ) then
		-- all variables in tables/namespaces with name DontSave are not saved. This allows for some temporary data
		if( varname ~= "DontSave" ) then
			for innervarname,innervalue in pairs(value) do
				printValue(prefix..varname, innervarname, innervalue )
			end
		end
	elseif( type(value) == "userdata" ) then
		local restoreString = DawnInterface.getItemReferenceRestore(prefix..varname, value );
		if(restoreString ~= "") then
			io.write(restoreString..'\n' );
		end
		--local furtherReinitializationString = DawnInterface.getReinitialisationString( prefix..varname, value );
		--io.write( furtherReinitializationString );
	end
end

function saveGame( fileprefix )

	local oldOut = io.output()
	io.output( fileprefix..'.lua' )

	local player = DawnInterface.getPlayer();
	io.write( player:getSaveText() );
	
	for varname,value in pairs(_G) do
		if( varname == "Zones") then
			printTables(nil,varname,value)
		end
	end
	
	for varname,value in pairs(_G) do
		if( varname == "Quests") then
			printTables(nil,varname,value)
		end
	end
	
	for varname,value in pairs(_G) do
		if( varname == "Eventhandlers") then
			printTables(nil,varname,value)
		end
	end
	
	for varname,value in pairs(_G) do
		if( varname == "SpawnPoints") then
			printTables(nil,varname,value)
		end
	end
	
	for varname,value in pairs(_G) do
		if( varname == "InteractionPoints") then
			printTables(nil,varname,value)
		end
	end
	
	for varname,value in pairs(_G) do
		if( varname == "InteractionRegions") then
			printTables(nil,varname,value)
		end
	end
	
	for k,v in pairs(restoreTables) do
		io.write(v..'\n');
	end
	
	io.write('\n')
	for varname,value in pairs(_G) do
		if( varname == "Zones") then
			printValue(nil,varname,value)
		end
	end
	
	for varname,value in pairs(_G) do
		if( varname == "Quests") then
			printValue(nil,varname,value)
		end
	end
		
	for k,v in pairs(restoreQuest) do
		io.write( v..'\n' );
	end
	
	for k,v in pairs(restoreInit) do
		io.write(v..'\n');
	end
	io.write( DawnInterface.storeGroundloot());
	
	io.write('\n')
	for varname,value in pairs(_G) do
		if( varname == "Eventhandlers") then
			printValue(nil,varname,value)
		end
	end
	
	for varname,value in pairs(_G) do
		if( varname == "SpawnPoints") then
			printValue(nil,varname,value)
		end
	end
	
	for varname,value in pairs(_G) do
		if( varname == "InteractionPoints") then
			printValue(nil,varname,value)
		end
	end
	
	for varname,value in pairs(_G) do
		if( varname == "InteractionRegions") then
			printValue(nil,varname,value)
		end
	end

	for varname,value in pairs(_G) do
		if( not initialGlobals[varname] and (varname ~= "zone1" and varname ~= "arinoxGeneralShop" and varname ~= "MapData" and varname ~= "Eventhandlers" and varname ~= "InteractionPoints" and varname ~= "SpawnPoints" and varname ~= "Quests" and varname ~= "Zones" and varname ~= "InteractionRegions")) then
			printValue(nil,varname,value)
		end
	end
	
	io.write( DawnInterface.getInventorySaveText() );
	io.write( DawnInterface.getSpellbookSaveText() );
	io.write( DawnInterface.getActionbarSaveText() );

	io.write( DawnInterface.getReenterCurrentZoneText() );
	local saveAllowedBoolText = "false";
	if ( DawnInterface.isSavingAllowed() ) then
		saveAllowedBoolText = "true";
	end
	io.write( "DawnInterface.setSavingAllowed( " .. saveAllowedBoolText .. " );\n" );
	-- map = LuaInterface.getCurrentMap()
	-- io.write( 'LuaInterface.enterMap( "',map:getFileName(),'", 3, 3 )' )
	io.close()
	io.output( oldOut )
end

function reinitGlobalsRecursive( surroundingName, varname, value )
	local prefix=''
	if( surroundingName ) then
		prefix=surroundingName..'.'
	end

	if( value == nil ) then
		return true
	elseif( type(value) == "table" ) then
		haveOnlyNil = true
		for innervarname,innervalue in pairs(value) do
			if ( reinitGlobalsRecursive(prefix..varname, innervarname, innervalue) == false ) then
				haveOnlyNil = false;
			end
		end
		if( haveOnlyNil ) then
			value = nil;
			return true;
		end
	else
		_G[''..prefix..varname..'']=nil
	end
	return false;
end

function loadGame( fileprefix )
	for varname,value in pairs(_G) do
		if( not initialGlobals[varname] and varname ~= "MapData") then
			_G[''..varname..'']=nil
		end
	end
	dofile( fileprefix..'.lua' )
end

initialGlobals = {}
saveGlobals(initialGlobals)