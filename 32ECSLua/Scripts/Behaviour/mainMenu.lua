local menu = {}
local element = {}

function menu:init()
	self.timer = 0.5
	self.colour = vector(162, 237, 255)
	self.hoverColour = vector(212, 247, 255)
	
	element.visibleBG = true
	element.text = ""
	element.position = vector(490, 200)
	element.dimensions = vector(300, 100)
	element.colour = self.colour
	element.fontSize = 36

	element.text = "Play!"
	self.playButton = scene.createEntity()
	scene.setComponent(self.playButton, ComponentType.UIElement, element)

	element.text = "Create Room"
	element.position.y = 350
	self.roomButton = scene.createEntity()
	scene.setComponent(self.roomButton, ComponentType.UIElement, element)

	element.text = "Exit"
	element.position.y = 500
	self.exitButton = scene.createEntity()
	scene.setComponent(self.exitButton, ComponentType.UIElement, element)

	element.visibleBG = false
	element.text = "Crimson Rush"
	element.position = vector(320, 50)
	element.dimensions = vector(600, 100)
	element.fontSize = 80
	local entity = scene.createEntity()
	scene.setComponent(entity, ComponentType.UIElement, element)
end

function menu:update(deltaTime)
	if (self.timer > 0) then
		self.timer = self.timer - deltaTime
		return
	end
	
	local button = scene.getComponent(self.playButton, ComponentType.UIElement)
	if (UI.isHover(button)) then
		button.colour = self.hoverColour
		scene.setComponent(self.playButton, ComponentType.UIElement, button)
		if (input.isMouseButtonPressed(Mouse.LEFT)) then
			scene.setScene("gameScene.lua")
			return
		end
	elseif (button.colour == self.hoverColour) then
		button.colour = self.colour
		scene.setComponent(self.playButton, ComponentType.UIElement, button)
	end

	button = scene.getComponent(self.roomButton, ComponentType.UIElement)
	if (UI.isHover(button)) then
		button.colour = self.hoverColour
		scene.setComponent(self.roomButton, ComponentType.UIElement, button)
		if (input.isMouseButtonPressed(Mouse.LEFT)) then
			scene.setScene("roomScene.lua")
			return
		end
	elseif (button.colour == self.hoverColour) then
		button.colour = self.colour
		scene.setComponent(self.roomButton, ComponentType.UIElement, button)
	end

	button = scene.getComponent(self.exitButton, ComponentType.UIElement)
	if (UI.isHover(button)) then
		button.colour = self.hoverColour
		scene.setComponent(self.exitButton, ComponentType.UIElement, button)
		if (input.isMouseButtonPressed(Mouse.LEFT)) then
			scene.quit()
		end
	elseif (button.colour == self.hoverColour) then
		button.colour = self.colour
		scene.setComponent(self.exitButton, ComponentType.UIElement, button)
	end
end

return menu