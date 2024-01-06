local room = {}

function room.new(width, height, doors, enemies)
	local t = {}
	t.width = width
	t.height = height
	t.doors = {}
	t.enemies = {}

	for x = 1, width or 0 do
		t[x] = {}
		for y = 1, height or 0 do
			t[x][y] = "Floor"
		end
	end

	for i = 1, width or 0 do
		t[i][1] = "Wall"
		t[i][height] = "Wall"
	end

	for i = 1, height or 0 do
		t[1][i] = "Wall"
		t[width][i] = "Wall"
	end

	for _, v in ipairs(doors) do
		table.insert(t.doors, vector(v.x, v.y, 0))
		t[v.x][v.y] = "Door"
	end

	for _, v in ipairs(enemies) do
		table.insert(t.enemies, vector(v.x, v.y, 0))
		t[v.x][v.y] = "Enemy"
	end

	setmetatable(t, room)
	return t
end

function room.newFromFile(path)
	local f = io.open(path, "r")
	-- Make sure file exist
	if (not f) then 
		return 
	else
		f:close()
	end

	local width = 0
	local height = 0
	local doors = {}
	local enemies = {}
	for line in io.lines(path) do
		height = height + 1
		width = #line
		for i = 1, #line do
			local c = line:sub(i, i)
			if (c == '+') then -- Door
				table.insert(doors, vector(i, height, 0))
			elseif (c == '*') then -- Enemy
				table.insert(enemies, vector(i, height, 0))
			end
		end
	end

	for _, v in ipairs(doors) do
		v.x = width - v.x + 1
		v.y = height - v.y + 1
	end
	for _, v in ipairs(enemies) do
		v.x = width - v.x + 1
		v.y = height - v.y + 1
	end

	return room.new(width, height, doors, enemies)
end

function room.writeToFile(r, path)

end

return setmetatable(room, {
	__call = function(_, ...)
	return room.new(...)
end
})