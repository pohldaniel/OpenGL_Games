local player = {}

player.type = "Player"
player.speed = 10
player.lastMove = vector(0, 0, 0)

function boolToInt(t)
	if(t) then
		return 1
	end
	return 0
end

function player:init()
	self.health = 100
	self.damageCooldown = 0
	self.shootCooldown = 0
end

function player:update(deltaTime)
	local map = scene.getComponent(self.mapID, ComponentType.Behaviour)
	if(map.paused) then
		return
	end

	local dir = vector(
		boolToInt(input.isKeyDown(Keys.A)) - boolToInt(input.isKeyDown(Keys.D)), 0,
		boolToInt(input.isKeyDown(Keys.W)) - boolToInt(input.isKeyDown(Keys.S)))

	dir = dir:normalize()
	local transform = scene.getComponent(self.ID, ComponentType.Transform)
	self.lastMove = dir * self.speed * deltaTime
	transform.position = transform.position + self.lastMove
	scene.setCameraPos(transform.position + vector(0, 4, 0))

	local centerVec = input.getMousePositionCenter():normalize()
	transform.rotation = vector(0, math.acos(centerVec.x / centerVec:length()) * (180 / math.pi) + 90, 0)

	if(centerVec.y < 0) then
		transform.rotation.y = 180 - transform.rotation.y
	end
	local lookDir = vector(-centerVec.x, 0, -centerVec.y)

	scene.setComponent(self.ID, ComponentType.Transform, transform)

	if (input.isMouseButtonPressed(Mouse.LEFT)) then
		local entity = scene.createEntity()
		scene.setComponent(entity, ComponentType.Behaviour, "bullet.lua")
		local bullet = scene.getComponent(entity, ComponentType.Behaviour)
		bullet.velocity = lookDir

		local t = scene.getComponent(entity, ComponentType.Transform)
		t.position = transform.position + lookDir * 0.5
		scene.setComponent(entity, ComponentType.Transform, t)
		self.shootCooldown = 0.25
	end

	if (self.damageCooldown > 0) then
		self.damageCooldown = self.damageCooldown - deltaTime
	end

	if (self.shootCooldown > 0) then
		self.shootCooldown = self.shootCooldown - deltaTime
	end
end

function player:collision(other)
	if (other.type == "Enemy") then
		if (self.damageCooldown <= 0) then
			self.health = self.health - 20
			self.damageCooldown = 0.5
		end
		local transform = scene.getComponent(self.ID, ComponentType.Transform)
		transform.position = transform.position - self.lastMove
		scene.setComponent(self.ID, ComponentType.Transform, transform)
	end
end

return player