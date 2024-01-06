scene.loadResource("Player.obj", "Player")
scene.loadResource("Enemy.obj", "Enemy")
scene.loadResource("Floor.obj", "Floor")
scene.loadResource("Wall.obj", "Wall")
scene.loadResource("Door.obj", "Door")
scene.loadResource("Goal.obj", "Goal")
scene.setCamera(vector(0, 2, 0), vector(90, 0, 0), 90)

local playerEntity = scene.createEntity();
scene.setComponent(playerEntity, ComponentType.Behaviour, "player.lua")
scene.setComponent(playerEntity, ComponentType.MeshComp, "Player")

local entity = scene.createEntity()
scene.setComponent(entity, ComponentType.Behaviour, "map.lua")

local map = scene.getComponent(entity, ComponentType.Behaviour)
map.playerID = playerEntity
scene.getComponent(playerEntity, ComponentType.Behaviour).mapID = entity
map:makelayout()
map:spawnroom(1)
