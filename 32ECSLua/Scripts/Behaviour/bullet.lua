local bullet = {}

bullet.type = "Bullet"
bullet.timer = 0.25
bullet.speed = 35
bullet.damage = 40
bullet.velocity = vector(0, 0, 1)

function bullet:init()
	scene.setComponent(self.ID, ComponentType.MeshComp, "Bullet")
end

function bullet:update(deltaTime)
	local transform = scene.getComponent(self.ID, ComponentType.Transform)
	transform.position = transform.position + self.velocity * self.speed * deltaTime
	transform.scale = vector(1, 1, 1) * 0.5
	scene.setComponent(self.ID, ComponentType.Transform, transform)

	self.timer = self.timer - deltaTime
	if(self.timer <= 0) then
		scene.removeEntity(self.ID)
	end
end

function bullet:collision(other)
	if(other.type == "Enemy") then
		other.health = other.health - self.damage
		scene.removeEntity(self.ID)
	end
end

return bullet