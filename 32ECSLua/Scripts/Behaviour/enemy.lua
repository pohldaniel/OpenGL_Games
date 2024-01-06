local enemy = {}
enemy.speed = 1
enemy.type = "Enemy"
enemy.health = 100

function enemy:init()
	self.time = math.random(1, 3)
end

function enemy:update(deltaTime)
	local map = scene.getComponent(self.mapID, ComponentType.Behaviour)
	if(map.paused) then
		return
	end

	if (self.health <= 0) then
		scene.removeEntity(self.ID)
	elseif (self.playerID ~= nil) then
		local transform = scene.getComponent(self.ID, ComponentType.Transform)
		local playerTransform = scene.getComponent(self.playerID, ComponentType.Transform)

		local dir = (playerTransform.position - transform.position):normalize()
		self.lastMove = dir * self.speed * deltaTime
		transform.position = transform.position + self.lastMove
		scene.setComponent(self.ID, ComponentType.Transform, transform)
	end
end

function enemy:collision(other)
	if (other.type == "Player") then
		local transform = scene.getComponent(self.ID, ComponentType.Transform)
		transform.position = transform.position - self.lastMove
		scene.setComponent(self.ID, ComponentType.Transform, transform)
	end
end

return enemy