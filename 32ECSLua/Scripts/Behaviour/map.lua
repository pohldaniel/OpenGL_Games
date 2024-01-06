local room = require("Scripts/room")
local map = {}

map.playerID = 0
map.startroom = room(9, 9, { vector(4, 1), vector(4, 9), vector(1, 4), vector(9, 4) }, {})
map.endroom = room(7, 7, { vector(3, 1), vector(3, 7), vector(1, 3), vector(7, 3) }, {})
map.roomCount = 10
map.template = room(9, 9, { vector(5, 1), vector(5, 9), vector(1, 5), vector(9, 5) }, { vector(5, 5) })
map.rooms = {}
map.curRoom = 0
map.curRoomIndex = 0
map.locked = true
map.IDs = {}
map.EnemyIDs = {}
map.DoorIDs = {}

function map:makelayout()
	self.rooms = {}
	self.startroom = room(self.startroom.width, self.startroom.height, self.startroom.doors, {})
	self.endroom = room(self.endroom.width, self.endroom.height, self.endroom.doors, {})

	self.rooms[1] = self.startroom
	self.IDs = {}
	self.EnemyIDs = {}
	self.DoorIDs = {}

	local i = 2
	while(i <= self.roomCount) do
		local connectRoomIndex = 0
		if (i == self.roomCount) then
			self.rooms[i] = self.endroom
			connectRoomIndex = math.random(2, i - 1)
		else
			local newR = self.roomTemplates[math.random(1, #self.roomTemplates)]
			self.rooms[i] = room(newR.width, newR.height, newR.doors, newR.enemies)
			connectRoomIndex = math.random(1, i - 1)
		end

		local ownDoor = math.random(1, #self.rooms[i].doors)
		if (self.rooms[i].doors[ownDoor].z == 0) then
			local r1 = self.rooms[i]
			local v1 = r1.doors[ownDoor]
			local r2 = self.rooms[connectRoomIndex]
			for j, v2 in ipairs(r2.doors) do
				if (v2.z == 0 and v1.z == 0 and (
				(v1.x == 1 and v2.x == r2.width) or
				(v1.y == 1 and v2.y == r2.height) or 
				(v2.x == 1 and v1.x == r1.width or
				(v2.y == 1 and v1.y == r1.height)))) then
					-- Using z value for connectingIndex
					self.rooms[i].doors[ownDoor].z = connectRoomIndex
					self.rooms[connectRoomIndex].doors[j].z = i
					i = i + 1
					break
				end
			end
		end
	end
end

function map:spawnroom(index)
	local r = self.rooms[index]
	local offset = vector(r.width, 0, r.height) / 2 + vector(1, 0, 1) * 0.5
	for x = 1, r.width do
		for z = 1, r.height do
			local entity = scene.createEntity()
			local transform = scene.getComponent(entity, ComponentType.Transform)
			transform.position = vector(x, -1, z) - offset
			if (r[x][z] == "Floor") then
				scene.setComponent(entity, ComponentType.MeshComp, "Floor")
			elseif (r[x][z] == "Wall") then
				scene.setComponent(entity, ComponentType.MeshComp, "Wall")
			elseif (r[x][z] == "Door") then
				for i, v in ipairs(r.doors) do
					if(v.x == x and v.y == z) then
						if (v.z == 0) then
							scene.setComponent(entity, ComponentType.MeshComp, "Wall")
							r[x][z] = "Wall"
							table.remove(r.doors, i)
						else
							scene.setComponent(entity, ComponentType.MeshComp, "Door")
							table.insert(self.DoorIDs, entity)
						end
						break
					end
				end
			elseif (r[x][z] == "Enemy") then
				scene.setComponent(entity, ComponentType.MeshComp, "Floor")

				local enemy = scene.createEntity();
				local enemyTransform = scene.getComponent(enemy, ComponentType.Transform)
				enemyTransform.position = transform.position + vector(0, 1, 0)

				scene.setComponent(enemy, ComponentType.Transform, enemyTransform)
				scene.setComponent(enemy, ComponentType.MeshComp, "Enemy")
				scene.setComponent(enemy, ComponentType.Behaviour, "enemy.lua")
				scene.getComponent(enemy, ComponentType.Behaviour).playerID = self.playerID
				scene.getComponent(enemy, ComponentType.Behaviour).mapID = self.ID

				table.insert(self.EnemyIDs, enemy)
			end
			table.insert(self.IDs, entity)
			scene.setComponent(entity, ComponentType.Transform, transform)
		end
	end
	if (index == self.roomCount) then
		local entity = scene.createEntity()
		scene.setComponent(entity, ComponentType.MeshComp, "Goal")
		table.insert(self.IDs, entity)
	end

	self.curRoom = r
	self.curRoomIndex = index
	self.locked = true
end

function map:despawnroom(index)
	for _, v in ipairs(self.IDs) do
		scene.removeEntity(v)
	end
	for _, v in ipairs(self.EnemyIDs) do
		scene.removeEntity(v)
	end
	self.IDs = {}
	self.EnemyIDs = {}
	self.DoorIDs = {}
end

function map:init()
	self.roomTemplates = {}
	table.insert(self.roomTemplates, self.template)
	local r = room.newFromFile("Resources/Rooms/1.room")
	local counter = 2

	while(r ~= nil) do
		table.insert(self.roomTemplates, r)
		r = room.newFromFile("Resources/Rooms/" .. tostring(counter) .. ".room")
		counter = counter + 1
	end

	local element = {}
	element.visibleBG = false
	element.text = "1"
	element.position = vector(0, 0)
	element.dimensions = vector(100, 100)
	element.colour = vector(255, 255, 255)
	element.fontSize = 72

	-- Current level text
	self.curLevel = 1
	self.levelDisplay = scene.createEntity()
	scene.setComponent(self.levelDisplay, ComponentType.UIElement, element)

	element.visibleBG = true
	element.text = ""
	element.position = vector(340, 650)
	element.dimensions = vector(600, 35)
	element.colour = vector(255, 0, 0)

	-- Healthbar
	self.healthbar = scene.createEntity()
	scene.setComponent(self.healthbar, ComponentType.UIElement, element)

	element.colour = vector(50, 50, 50)
	self.healthBG = scene.createEntity()
	scene.setComponent(self.healthBG, ComponentType.UIElement, element)

	-- Intermediate menu (for pause and gameover)
	self.paused = false
	self.intMenu = scene.createEntity()
	scene.setComponent(self.intMenu, ComponentType.Behaviour, "intMenu.lua")
end

function map:update()
	local menu = scene.getComponent(self.intMenu, ComponentType.Behaviour)
	local player = scene.getComponent(self.playerID, ComponentType.Behaviour)

	-- Update slider
	local slider = scene.getComponent(self.healthbar, ComponentType.UIElement)
	local sliderBG = scene.getComponent(self.healthBG, ComponentType.UIElement)
	slider.dimensions.x = sliderBG.dimensions.x * (player.health / 100)
	scene.setComponent(self.healthbar, ComponentType.UIElement, slider)

	if (self.paused) then
		local button = scene.getComponent(menu.resumeButton, ComponentType.UIElement)
		if (UI.isHover(button) and input.isMouseButtonPressed(Mouse.LEFT) and player.health > 0) then
			self.paused = false
			menu:destroy()

			-- Put back Game UI
			local element = scene.getComponent(self.levelDisplay, ComponentType.UIElement)
			element.position = vector(0, 0)
			scene.setComponent(self.levelDisplay, ComponentType.UIElement, element)
			element = scene.getComponent(self.healthbar, ComponentType.UIElement)
			element.position = vector(340, 650)
			scene.setComponent(self.healthbar, ComponentType.UIElement, element)
			element = scene.getComponent(self.healthBG, ComponentType.UIElement)
			element.position = vector(340, 650)
			scene.setComponent(self.healthBG, ComponentType.UIElement, element)
		elseif (UI.isHover(button) and input.isMouseButtonPressed(Mouse.LEFT) and player.health <= 0) then
			scene.setScene("gameScene.lua")
			return
		end
	elseif (player.health <= 0) then
		self.paused = true
		local element = {}
		element.visibleBG = false
		element.text = "You reached level: " .. tostring(self.curLevel)
		element.position = vector(500, 150)
		element.dimensions = vector(200, 100)
		element.colour = vector(255, 255, 255)
		element.fontSize = 64
		local entity = scene.createEntity()
		scene.setComponent(entity, ComponentType.UIElement, element)

		menu:spawn("Game Over")
		element = scene.getComponent(menu.resumeButton, ComponentType.UIElement)
		element.text = "Play again"
		scene.setComponent(menu.resumeButton, ComponentType.UIElement, element)

		-- "Remove" Game UI
		element = scene.getComponent(self.levelDisplay, ComponentType.UIElement)
		element.position = vector(-100, -100)
		scene.setComponent(self.levelDisplay, ComponentType.UIElement, element)
		element = scene.getComponent(self.healthbar, ComponentType.UIElement)
		element.position = vector(-100, -100)
		scene.setComponent(self.healthbar, ComponentType.UIElement, element)
		element = scene.getComponent(self.healthBG, ComponentType.UIElement)
		element.position = vector(-100, -100)
		scene.setComponent(self.healthBG, ComponentType.UIElement, element)
	end

	if (input.isKeyPressed(Keys.P) and player.health > 0) then
		self.paused = not self.paused

		if (self.paused) then
			menu:spawn("Paused")

			-- "Remove" Game UI
			local element = scene.getComponent(self.levelDisplay, ComponentType.UIElement)
			element.position = vector(-100, -100)
			scene.setComponent(self.levelDisplay, ComponentType.UIElement, element)
			element = scene.getComponent(self.healthbar, ComponentType.UIElement)
			element.position = vector(-100, -100)
			scene.setComponent(self.healthbar, ComponentType.UIElement, element)
			element = scene.getComponent(self.healthBG, ComponentType.UIElement)
			element.position = vector(-100, -100)
			scene.setComponent(self.healthBG, ComponentType.UIElement, element)
		else
			menu:destroy()

			-- Put back Game UI
			local element = scene.getComponent(self.levelDisplay, ComponentType.UIElement)
			element.position = vector(0, 0)
			scene.setComponent(self.levelDisplay, ComponentType.UIElement, element)
			element = scene.getComponent(self.healthbar, ComponentType.UIElement)
			element.position = vector(340, 650)
			scene.setComponent(self.healthbar, ComponentType.UIElement, element)
			element = scene.getComponent(self.healthBG, ComponentType.UIElement)
			element.position = vector(340, 650)
			scene.setComponent(self.healthBG, ComponentType.UIElement, element)
		end
	end

	if (self.paused) then
		return
	end
	
	local playerTransform = scene.getComponent(self.playerID, ComponentType.Transform)
	local pos = playerTransform.position + vector(self.curRoom.width, 0, self.curRoom.height) / 2 - vector(1, 0, 1) * 0.5
	pos = vector(math.floor(pos.x + 0.5), 0, math.floor(pos.z + 0.5)) + vector(1, 0, 1)

	-- Collision with wall (edge)
	if(pos.x <= 1 or pos.z <= 1 or pos.x > self.curRoom.width - 1 or pos.z > self.curRoom.height - 1) then
		-- Collision with door
		if(self.curRoom[pos.x][pos.z] == "Door" and self.locked == false) then
			-- Find right door
			for _, v in ipairs(self.curRoom.doors) do
				if(v.x == pos.x and v.y == pos.z) then
					-- Set player pos next to door in new room
					local spawnPos = vector()
					for __, v1 in ipairs(self.rooms[v.z].doors) do
						if(v1.z == self.curRoomIndex) then
							spawnPos = vector(v1.x, 0, v1.y) + player.lastMove * 5
							player.damageCooldown = 1
							break
						end
					end
					playerTransform.position = spawnPos - vector(self.rooms[v.z].width, 0, self.rooms[v.z].height) / 2 - vector(1, 0, 1) * 0.5

					-- Spawn new room
					self:despawnroom(self.curRoomIndex)
					self:spawnroom(v.z)
					break
				end
			end
		else
			playerTransform.position = playerTransform.position - player.lastMove
		end
	end

	-- Clean enemy ID table
	for i, v in ipairs(self.EnemyIDs) do
		if (not scene.entityValid(v)) then
			table.remove(self.EnemyIDs, i)
		end
	end

	-- Enemies are all dead
	if (#self.EnemyIDs == 0 and self.locked == true) then
		self.locked = false
		for _, v in ipairs(self.DoorIDs) do
			scene.setComponent(v, ComponentType.MeshComp, "Floor")
		end
		for _, v in ipairs(self.curRoom.enemies) do
			self.curRoom[v.x][v.y] = "Floor" 
		end
		self.curRoom.enemies = {}
	end

	-- Found goal
	if (self.curRoomIndex == self.roomCount and pos.x == math.ceil(self.curRoom.width * 0.5) and pos.z == math.ceil(self.curRoom.height * 0.5)) then
		self:despawnroom(self.curRoomIndex)
		self:makelayout()
		self:spawnroom(1)

		playerTransform.position = vector()
		player.health = math.min(player.health + 20, 100)

		self.curLevel = self.curLevel + 1
		local element = scene.getComponent(self.levelDisplay, ComponentType.UIElement)
		element.text = tostring(self.curLevel)
		scene.setComponent(self.levelDisplay, ComponentType.UIElement, element)
	end
	scene.setComponent(self.playerID, ComponentType.Transform, playerTransform)
end

return map