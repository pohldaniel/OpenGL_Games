local room = require("Scripts/room")
local menu = {}

function menu:init()
	self.bColour = vector(162, 237, 255)
	self.bHoverColour = vector(212, 247, 255)
	self.minTiles = 7
	self.maxTiles = 21

	self.gridState = {}
	self.gridState.Floor = 1
	self.gridState.Wall = 2
	self.gridState.Enemy = 3
	self.gridState.Door = 4
	self.gColour = { vector(235, 235, 235), vector(25, 25, 255), vector(255, 25, 25), vector(255, 255, 0) }
	self.gHoverColour = { vector(255, 255, 255), vector(100, 100, 255), vector(255, 100, 100), vector(255, 255, 150) }

	self.width = self.minTiles
	self.height = self.minTiles
	self.grids = {}
	self.grids.IDs = {}
	self.grids.states = {}

	self.sliders = {}
	local element = {}
	element.visibleBG = true
	element.text = ""
	element.position = vector(220, 10)
	element.dimensions = vector(400, 50)
	element.colour = vector(50, 50, 50)
	element.fontSize = 36

	-- Width slider
	self.sliders[1] = {}

	element.visibleBG = false
	element.text = "width: " .. tostring(self.minTiles)
	self.sliders[1].text = scene.createEntity()
	scene.setComponent(self.sliders[1].text, ComponentType.UIElement, element)

	element.visibleBG = true
	element.text = ""
	element.colour = self.bColour
	element.dimensions = vector(0, 50)
	self.sliders[1].slider = scene.createEntity()
	scene.setComponent(self.sliders[1].slider, ComponentType.UIElement, element)

	element.dimensions = vector(400, 50)
	element.colour = vector(50, 50, 50)
	self.sliders[1].bg = scene.createEntity()
	scene.setComponent(self.sliders[1].bg, ComponentType.UIElement, element)

	-- Height slider
	self.sliders[2] = {}

	element.position.x = 660
	element.visibleBG = false
	element.text = "height: " .. tostring(self.minTiles)
	self.sliders[2].text = scene.createEntity()
	scene.setComponent(self.sliders[2].text, ComponentType.UIElement, element)

	element.visibleBG = true
	element.text = ""
	element.colour = self.bColour
	element.dimensions = vector(0, 50)
	self.sliders[2].slider = scene.createEntity()
	scene.setComponent(self.sliders[2].slider, ComponentType.UIElement, element)

	element.dimensions = vector(400, 50)
	element.colour = vector(50, 50, 50)
	self.sliders[2].bg = scene.createEntity()
	scene.setComponent(self.sliders[2].bg, ComponentType.UIElement, element)

	-- Save button
	element.visibleBG = true
	element.text = "Save"
	element.position = vector(490, 550)
	element.dimensions = vector(300, 75)
	element.colour = self.bColour
	self.saveButton = scene.createEntity()
	scene.setComponent(self.saveButton, ComponentType.UIElement, element)

	-- Back button
	element.text = "back"
	element.position = vector(565, 650)
	element.dimensions = vector(150, 50)
	self.backButton = scene.createEntity()
	scene.setComponent(self.backButton, ComponentType.UIElement, element)

	-- Grid
	self:spawnGrid()
end

function menu:update(deltaTime)
	-- Width slider
	local sliderBG = scene.getComponent(self.sliders[1].bg, ComponentType.UIElement)
	local sliderVal = scene.getComponent(self.sliders[1].slider, ComponentType.UIElement)
	local sliderText = scene.getComponent(self.sliders[1].text, ComponentType.UIElement)
	if (UI.isHover(sliderBG)) then
		sliderVal.colour = self.bHoverColour
		if (input.isMouseButtonDown(Mouse.LEFT)) then
			local oldWidth = self.width
			sliderVal.dimensions.x = input.getMousePosition().x - sliderBG.position.x

			self.width = math.floor((self.maxTiles - self.minTiles) * (sliderVal.dimensions.x / sliderBG.dimensions.x) + self.minTiles + 0.5)
			if(oldWidth ~= self.width) then
				sliderText.text = "width: " .. tostring(self.width)
				scene.setComponent(self.sliders[1].text, ComponentType.UIElement, sliderText)
				self:spawnGrid()
				return
			end
		end
	elseif (sliderVal.colour == self.bHoverColour) then
		sliderVal.colour = self.bColour
	end
	scene.setComponent(self.sliders[1].slider, ComponentType.UIElement, sliderVal)

	-- Height slider
	sliderBG = scene.getComponent(self.sliders[2].bg, ComponentType.UIElement)
	sliderVal = scene.getComponent(self.sliders[2].slider, ComponentType.UIElement)
	sliderText = scene.getComponent(self.sliders[2].text, ComponentType.UIElement)
	if (UI.isHover(sliderBG)) then
		sliderVal.colour = self.bHoverColour
		if (input.isMouseButtonDown(Mouse.LEFT)) then
			local oldHeight = self.height
			sliderVal.dimensions.x = input.getMousePosition().x - sliderBG.position.x

			self.height = math.floor((self.maxTiles - self.minTiles) * (sliderVal.dimensions.x / sliderBG.dimensions.x) + self.minTiles + 0.5)
			if(oldHeight ~= self.height) then
				sliderText.text = "height: " .. tostring(self.height)
				scene.setComponent(self.sliders[2].text, ComponentType.UIElement, sliderText)
				self:spawnGrid()
				return
			end
		end
	elseif (sliderVal.colour == self.bHoverColour) then
		sliderVal.colour = self.bColour
	end
	scene.setComponent(self.sliders[2].slider, ComponentType.UIElement, sliderVal)

	-- Save button
	local button = scene.getComponent(self.saveButton, ComponentType.UIElement)
	if (UI.isHover(button)) then
		button.colour = self.bHoverColour
		scene.setComponent(self.saveButton, ComponentType.UIElement, button)
		if (input.isMouseButtonPressed(Mouse.LEFT)) then
			local r = 0
			local counter = 0
			while(r ~= nil) do
				counter = counter + 1
				r = room.newFromFile("Resources/Rooms/" .. tostring(counter) .. ".room")
			end
			local f = io.open("Resources/Rooms/" .. tostring(counter) .. ".room", "w")
			if (f) then 
				local index = 1
				for i = 1, self.height do
					local line = ""
					for j = 1, self.width do
						if (self.grids.states[index] == self.gridState.Floor or self.grids.states[index] == self.gridState.Wall) then
							line = line .. "-"
						elseif (self.grids.states[index] == self.gridState.Door) then
							line = line .. "+"
						elseif (self.grids.states[index] == self.gridState.Enemy) then
							line = line .. "*"
						end
						index = index + 1
					end
					f:write(line .. "\n")
				end
				f:close()
			end

			scene.setScene("menuScene.lua")
			return
		end
	elseif (button.colour == self.bHoverColour) then
		button.colour = self.bColour
		scene.setComponent(self.saveButton, ComponentType.UIElement, button)
	end

	button = scene.getComponent(self.backButton, ComponentType.UIElement)
	if (UI.isHover(button)) then
		button.colour = self.bHoverColour
		scene.setComponent(self.backButton, ComponentType.UIElement, button)
		if (input.isMouseButtonPressed(Mouse.LEFT)) then
			scene.setScene("menuScene.lua")
			return
		end
	elseif (button.colour == self.bHoverColour) then
		button.colour = self.bColour
		scene.setComponent(self.backButton, ComponentType.UIElement, button)
	end

	-- Grid cells
	for i, v in ipairs(self.grids.IDs) do
		button = scene.getComponent(v, ComponentType.UIElement)
		if (UI.isHover(button)) then
			button.colour = self.gHoverColour[self.grids.states[i]]
			scene.setComponent(v, ComponentType.UIElement, button)
			if (input.isMouseButtonPressed(Mouse.LEFT)) then
				self.grids.states[i] = math.fmod(self.grids.states[i] + 1, 4) + 1
			end
		elseif (button.colour == self.gHoverColour[self.grids.states[i]]) then
			button.colour = self.gColour[self.grids.states[i]]
			scene.setComponent(v, ComponentType.UIElement, button)
		end
	end
end

function menu:spawnGrid()
	self:destroyGrid()
	self.grids.IDs = {}
	self.grids.states = {}

	local maxNum = math.max(self.width, self.height)
	local cellSize = 400 / maxNum
	local margin = 50 / maxNum

	local pos = vector(640, 360) - 
	vector(self.width * 0.5 * cellSize + (self.width - 1) * 0.5 * margin,
	self.height * 0.5 * cellSize + (self.height - 1) * 0.5 * margin) - 
	vector(0, 50)

	local element = {}
	element.visibleBG = true
	element.text = ""
	element.position = vector(pos.x, pos.y)
	element.dimensions = vector(cellSize, cellSize)
	element.colour = vector(255, 255, 255)
	element.fontSize = 36

	for y = 1, self.height do
		for x = 1, self.width do
			local entity = scene.createEntity()
			table.insert(self.grids.IDs, entity)

			local state = self.gridState.Floor
			if(x == 1 or y == 1 or x == self.width or y == self.height) then
				state = self.gridState.Wall
				if(x == math.ceil(self.width * 0.5) or y == math.ceil(self.height * 0.5)) then
					state = self.gridState.Door
				end
			end
			table.insert(self.grids.states, state)
			element.colour = self.gColour[state]

			scene.setComponent(entity, ComponentType.UIElement, element)

			element.position.x = element.position.x + cellSize + margin
		end
		element.position.y = element.position.y + cellSize + margin
		element.position.x = pos.x
	end
end

function menu:destroyGrid()
	for _, v in ipairs(self.grids.IDs) do
		scene.removeEntity(v)
	end
end

return menu