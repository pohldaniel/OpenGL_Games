local intMenu = {}

function intMenu:spawn(title)
	self.colour = vector(162, 237, 255)
	self.hoverColour = vector(212, 247, 255)
	
	local element = {}
	element.visibleBG = true
	element.text = ""
	element.position = vector(490, 300)
	element.dimensions = vector(300, 100)
	element.colour = self.colour
	element.fontSize = 36

	element.text = "Resume"
	self.resumeButton = scene.createEntity()
	scene.setComponent(self.resumeButton, ComponentType.UIElement, element)

	element.text = "Main Menu"
	element.position.y = 450
	self.menuButton = scene.createEntity()
	scene.setComponent(self.menuButton, ComponentType.UIElement, element)

	element.visibleBG = false
	element.text = title
	element.position = vector(320, 50)
	element.dimensions = vector(600, 100)
	element.fontSize = 80
	self.title = scene.createEntity()
	scene.setComponent(self.title, ComponentType.UIElement, element)

	element.visibleBG = true
	element.text = ""
	element.position = vector(0, 0)
	element.dimensions = vector(1280, 720)
	element.colour = vector(130, 130, 130)
	self.background = scene.createEntity()
	scene.setComponent(self.background, ComponentType.UIElement, element)
	self.active = true
end

function intMenu:destroy()
	self.active = false
	scene.removeEntity(self.title)
	scene.removeEntity(self.resumeButton)
	scene.removeEntity(self.menuButton)
	scene.removeEntity(self.exitButton)
	scene.removeEntity(self.background)
end

function intMenu:init()
	self.active = false
end

function intMenu:update(deltaTime)
	if (self.active) then
		local button = scene.getComponent(self.resumeButton, ComponentType.UIElement)
		if (UI.isHover(button)) then
			button.colour = self.hoverColour
			scene.setComponent(self.resumeButton, ComponentType.UIElement, button)
		elseif (button.colour == self.hoverColour) then
			button.colour = self.colour
			scene.setComponent(self.resumeButton, ComponentType.UIElement, button)
		end

		button = scene.getComponent(self.menuButton, ComponentType.UIElement)
		if (UI.isHover(button)) then
			button.colour = self.hoverColour
			scene.setComponent(self.menuButton, ComponentType.UIElement, button)
			if (input.isMouseButtonPressed(Mouse.LEFT)) then
				scene.setScene("menuScene.lua")
				return
			end
		elseif (button.colour == self.hoverColour) then
			button.colour = self.colour
			scene.setComponent(self.menuButton, ComponentType.UIElement, button)
		end
	end
end

return intMenu