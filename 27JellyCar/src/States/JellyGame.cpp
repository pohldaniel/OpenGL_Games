#include <engine/Fontrenderer.h>

#include "JellyGame.h"
#include "JellyMenu.h"
#include "JellyDialog.h"

#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"
#include "SceneManager.h"

JellyGame::JellyGame(StateMachine& machine, std::string sceneName) : State(machine, States::JELLYGAME) {
	EventDispatcher::AddKeyboardListener(this);
	Mouse::instance().attach(Application::GetWindow(), false);

	m_sceneName = sceneName;

	JellyHellper::Instance()->LoadShaders();
	
	m_screenBounds = Vector4f(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f);

	m_fastCar = false;
	m_slowCar = true;

	m_showMap = false;

	m_hitTime = 0.0f;
	m_chassisHit = 0.0f;

	

	//jumping
	m_isJumping = false;
	m_inTheAir = false;
	m_jumpStartPosition = 0.0f;
	m_bestJumpLenght = 0.0f;
	m_time = 0.0f;

	m_transformMeter = 1.0f;
	m_transformMeterRechargeSpeed = 1.0f / 13.0f;
	m_transformMeterGrowDir = 0;

	//checkpoint
	m_checkpoint = false;

	//ballon and tire initialization
	m_haveBallon = false;
	m_haveTire = false;

	m_ballonActive = false;
	m_tireActive = false;

	m_ballonPressed = false;
	m_tirePressed = false;

	m_ballonTime = 0.0f;
	m_tireTime = 0.0f;

	m_ballonAABB.expandToInclude(Vector2((static_cast<float>(Application::Width) / 2.0f) + 128.0f + 64.0f, 450.0f + 64.0f));
	m_ballonAABB.expandToInclude(Vector2((static_cast<float>(Application::Width) / 2.0f) + 128.0f - 64.0f, 450.0f - 64.0f));

	m_tireAABB.expandToInclude(Vector2((static_cast<float>(Application::Width) / 2.0f) - 128.0f + 64.0f, 450.0f + 64.0f));
	m_tireAABB.expandToInclude(Vector2((static_cast<float>(Application::Width) / 2.0f) - 128.0f - 64.0f, 450.0f - 64.0f));

	m_sceneFile = SceneManager::Get().getScene(m_sceneName).getCurrentSceneInfo().file;
	m_levelName = SceneManager::Get().getScene(m_sceneName).getCurrentSceneInfo().name;

	m_world = new World();

	Scene& scene = SceneManager::Get().getScene(m_sceneName);
	scene.InitPhysic(m_world);
	scene.loadLevel("Assets/Jelly/Scenes_new/" + m_sceneFile);
	scene.buildLevel(m_world, m_gameBodies);
	scene.buildCar(m_world, m_car, "Assets/Jelly/car_and_truck.car");

	const SkinInfo& skinInfo = scene.getCurrentSkinInfo();
	m_car->SetChassisTextures(skinInfo.skinTexture.chassisSmall, skinInfo.skinTexture.chassisBig);
	m_car->SetTireTextures(skinInfo.skinTexture.tireSmall, skinInfo.skinTexture.tireBig);

	// filter callbacks (for sound playback)
	m_world->setMaterialPairFilterCallback(0, 2, this);
	m_world->setMaterialPairFilterCallback(1, 2, this);
	m_world->setMaterialPairFilterCallback(4, 2, this);

	m_world->setMaterialPairFilterCallback(0, 3, this);
	m_world->setMaterialPairFilterCallback(1, 3, this);
	m_world->setMaterialPairFilterCallback(4, 3, this);

	//ballon and tire collision
	m_world->setMaterialPairFilterCallback(5, 2, this);
	m_world->setMaterialPairFilterCallback(5, 3, this);

	//set map size
	m_wholeMapPosition = scene.getWorldCenter();
	m_wholeMapSize = scene.getWorldSize();
	m_worldLimits = scene.getWorldLimits();

	//compute map view
	//mapLimits
	if (m_wholeMapSize.X > m_wholeMapSize.Y){
		float sizeX = m_wholeMapSize.X;
		float sizeY = m_wholeMapSize.X  * 0.56f;

		float startX, startY;
		float endX, endY;

		if (sizeY < m_wholeMapSize.Y){
			startY = m_worldLimits.Min.Y - 5.0f;
			endY = m_worldLimits.Max.Y + 5.0f;

			sizeX = ((m_wholeMapSize.Y + 10.0f) / 0.56f);

			startX = m_wholeMapPosition.X - (sizeX / 2.0f);
			endX = m_wholeMapPosition.X + (sizeX / 2.0f);

		}else{
			startX = m_worldLimits.Min.X;
			endX = m_worldLimits.Max.X;

			startY = m_wholeMapPosition.Y - (sizeY / 2.0f);
			endY = m_wholeMapPosition.Y + (sizeY / 2.0f);
		}

		m_mapLimits.Min.X = startX;
		m_mapLimits.Min.Y = startY;
		m_mapLimits.Max.X = endX;
		m_mapLimits.Max.Y = endY;

	}else{
		float sizeX = m_wholeMapSize.Y / 0.56f;
		float sizeY = m_wholeMapSize.Y;

		float startX = 0.0f, startY = 0.0f;
		float endX = 0.0f, endY = 0.0f;

		if (sizeX < m_wholeMapSize.X){
			//??
		}else{
			startY = m_worldLimits.Min.Y;
			endY = m_worldLimits.Max.Y;

			startX = m_wholeMapPosition.X - (sizeX / 2.0f);
			endX = m_wholeMapPosition.X + (sizeX / 2.0f);
		}

		m_mapLimits.Min.X = startX;
		m_mapLimits.Min.Y = startY;
		m_mapLimits.Max.X = endX;
		m_mapLimits.Max.Y = endY;
	}

	m_wholeMapSize.X += 5;
	m_wholeMapSize.Y = (m_wholeMapSize.X *0.56f) / 2.0f;
	m_wholeMapSize.X = m_wholeMapSize.X / 2.0f;

	m_levelTarget = scene.getLevelTarget();
	m_levelLine = scene.getLevelLine();

	m_carBreakCount = 0;

	m_newTimeRecord = false;
	m_newJumpRecord = false;

	m_mainRT.create(Application::Width, Application::Height);
	m_mainRT.attachTexture2D(AttachmentTex::RGBA);
	m_mainRT.attachRenderbuffer(AttachmentRB::DEPTH24);

	m_orthographic.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);
	
	m_camera = Camera();
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.orthographic(-20.0f, 20.0f, -11.2f, 11.2f, -1.0f, 1.0f);
}

JellyGame::~JellyGame() {
	EventDispatcher::RemoveKeyboardListener(this);
	Scene::ClearLevel(m_world, m_gameBodies, m_car);
}

void JellyGame::fixedUpdate() {}

void JellyGame::update() {

	processInput();

	m_time += m_dt;
	m_hitTime += m_dt;

	//hit info - zero before update/callback event
	m_chassisHit = 0;

	//jumping info - true before update/callback event
	m_isJumping = true;

	//Update physic
	for (int i = 0; i < 6; i++){
		m_world->update(0.004f);

		for (size_t i = 0; i < m_gameBodies.size(); i++)
			m_gameBodies[i]->Update(0.004f);

		m_car->clearForces();
		m_car->update(0.004f);
		
		UpdateTransformMeter(0.004f);
	}

	if(!m_showMap)
	  m_camera.setPosition(m_car->getPosition().X, m_car->getPosition().Y, 0.0f);

	if (m_ballonActive){
		m_ballonTime -= m_dt;

		if (m_ballonTime <= 0.0f){
			m_car->UseBallon(false);
			m_ballonActive = false;
			m_haveBallon = false;
		}
	}

	if (m_tireActive){
		m_tireTime -= m_dt;

		if (m_tireTime <= 0.0f){
			m_car->UseNearestGracity(false);
			m_tireActive = false;
			m_haveTire = false;
		}
	}

	//check level ending
	if (m_car->getChassisBody()->getAABB().contains(m_levelTarget)){
		if (m_car->getChassisBody()->contains(m_levelTarget)){
			//_audioHelper->StopEngineSound();

			if (m_time < SceneManager::Get().getScene(m_sceneName).getTime(m_levelName)) {
				m_newTimeRecord = true;

				SceneManager::Get().getScene(m_sceneName).setTime(m_levelName, m_time);
				Scene::SaveScores("JellyScore.xml", SceneManager::Get().getScene(m_sceneName).getSceneInfos());
			}

			if (m_bestJumpLenght > SceneManager::Get().getScene(m_sceneName).getJump(m_levelName)) {
				m_newJumpRecord = true;

				SceneManager::Get().getScene(m_sceneName).setJump(m_levelName, m_bestJumpLenght);
				Scene::SaveScores("JellyScore.xml", SceneManager::Get().getScene(m_sceneName).getSceneInfos());
			}

			m_machine.addStateAtTop(new JellyDialogFinish(m_machine, m_mainRT, m_newJumpRecord, m_newTimeRecord));
			return;
		}
	}

	for (int i = 0; i < 2; i++){
		if (m_car->getTire(i)->getAABB().contains(m_levelTarget)){
			if (m_car->getTire(i)->contains(m_levelTarget)){
				//_audioHelper->StopEngineSound();

				if (m_time < SceneManager::Get().getScene(m_sceneName).getTime(m_levelName)){
					m_newTimeRecord = true;

					SceneManager::Get().getScene(m_sceneName).setTime(m_levelName, m_time);
					Scene::SaveScores("JellyScore.xml", SceneManager::Get().getScene(m_sceneName).getSceneInfos());
				}

				if (m_bestJumpLenght > SceneManager::Get().getScene(m_sceneName).getJump(m_levelName)){
					m_newJumpRecord = true;

					SceneManager::Get().getScene(m_sceneName).setJump(m_levelName, m_bestJumpLenght);
					Scene::SaveScores("JellyScore.xml", SceneManager::Get().getScene(m_sceneName).getSceneInfos());
				}

				m_machine.addStateAtTop(new JellyDialogFinish(m_machine, m_mainRT, m_newJumpRecord, m_newTimeRecord));
				return;
			}
		}
	}

	//check out of bounds
	if (m_car->getPosition().Y < m_levelLine){
		if (m_checkpoint){
			Vector2 pos = Vector2(m_checkpointPosition[0], m_checkpointPosition[1]);
			Vector2 scale = Vector2(1.0f, 1.0f);

			m_car->getChassisBody()->setVelocity(Vector2(0, 0));
			m_car->getTire(0)->setVelocity(Vector2(0, 0));
			m_car->getTire(1)->setVelocity(Vector2(0, 0));

			m_car->getChassisBody()->setPositionAngle(pos, 0.0f, scale);
			m_car->getTire(0)->setPositionAngle(Vector2(m_checkpointPosition[0] + 1.5f, m_checkpointPosition[1] - 0.3f), 0.0f, scale);
			m_car->getTire(1)->setPositionAngle(Vector2(m_checkpointPosition[0] - 1.3f, m_checkpointPosition[1] - 0.3f), 0.0f, scale);
		}else{
			//_audioHelper->StopEngineSound();
			m_machine.addStateAtTop(new JellyDialog(m_machine, m_mainRT, "Out of level"));
			return;
		}
	}

	//check car broken
	AABB chassisAABB = m_car->getChassisBody()->getAABB();
	Vector2 chassisSize = chassisAABB.Max - chassisAABB.Min;
	if ((fabsf(chassisSize.X) > 17.0f) || (fabsf(chassisSize.Y) > 17.0f)){
		//_audioHelper->StopEngineSound();
		m_machine.addStateAtTop(new JellyDialog(m_machine, m_mainRT, "Car broken"));
		return;
	}

	//car broken
	if (m_world->getPenetrationCount() > 20){
		m_carBreakCount++;
		if (m_carBreakCount > 5){
			//_audioHelper->StopEngineSound();
			m_machine.addStateAtTop(new JellyDialog(m_machine, m_mainRT, "Car broken"));
			return;
		}
	}

	//jumping
	if (m_isJumping && !m_inTheAir){
		//we just started jumping
		m_jumpStartPosition = m_car->getPosition().X;
		m_inTheAir = true;
	}

	if (!m_isJumping && m_inTheAir){
		//end of jump - check if there is new jump record
		float jumpLenght = fabsf(m_jumpStartPosition - m_car->getPosition().X);

		if (jumpLenght > m_bestJumpLenght){
			m_bestJumpLenght = jumpLenght;
		}

		m_inTheAir = false;
	}

	//hit sound
	if (m_chassisHit > 0.0f){
		if ((m_chassisHit > 3.0f) && (m_hitTime > 0.3f)){
			//play hit sound
			//_audioHelper->PlayHitSound();

			//set timer
			m_hitTime = 0.0f;
		}
	}
}

void JellyGame::render() {
	
	renderScene();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	m_mainRT.bindColorTexture();
	Globals::shapeManager.get("quad").drawRaw();
	m_mainRT.unbindColorTexture();
	shader->unuse();
}

void JellyGame::renderScene() {
	m_mainRT.bindWrite();
	glClearBufferfv(GL_COLOR, 0, std::vector<float>{0.494f, 0.686f, 0.796f, 1.0f}.data());
	glClearBufferfv(GL_DEPTH, 0, &std::vector<float>{1.0f}[0]);

	//_backSprite->SetScale(glm::vec2(0.1f, 0.1f));
	if (m_showMap) {
		float x = m_mapLimits.Min.X - 51.0f;
		float y = m_mapLimits.Min.Y - 51.0f;

		for (float xx = x; xx <= m_mapLimits.Max.X + 51.0f; xx += 51.0f) {
			for (float yy = y; yy <= m_mapLimits.Max.Y + 51.0f; yy += 51.0f) {
				const Texture& texture = Globals::textureManager.get("paper");
				texture.bind(0);
				auto shader = Globals::shaderManager.getAssetPointer("quad");
				shader->use();
				shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_camera.getViewMatrix() * Matrix4f::Translate(xx, yy, 0.0f) * Matrix4f::Scale(static_cast<float>(texture.getWidth()) * 0.1f, static_cast<float>(texture.getHeight()) * 0.1f, 1.0f));
				shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
				Globals::shapeManager.get("quad_half").drawRaw();
				shader->unuse();
				texture.unbind();


			}
		}
	}else {
		float x = m_worldLimits.Min.X - 51.0f;
		float y = m_worldLimits.Min.Y - 51.0f;

		for (float xx = x; xx <= m_worldLimits.Max.X + 51.0f; xx += 51.0f) {
			for (float yy = y; yy <= m_worldLimits.Max.Y + 51.0f; yy += 51.0f) {
				const Texture& texture = Globals::textureManager.get("paper");
				texture.bind(0);
				auto shader = Globals::shaderManager.getAssetPointer("quad");
				shader->use();
				shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_camera.getViewMatrix() * Matrix4f::Translate(xx, yy, 0.0f) * Matrix4f::Scale(static_cast<float>(texture.getWidth()) * 0.1f, static_cast<float>(texture.getHeight()) * 0.1f, 1.0f));
				shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
				Globals::shapeManager.get("quad_half").drawRaw();
				shader->unuse();
				texture.unbind();
			}
		}
	}

	//camera aabb
	Vector2 camMin = m_car->getPosition() - Vector2(20.0f, 11.2f);
	Vector2 camMax = m_car->getPosition() + Vector2(20.0f, 11.2f);
	JellyPhysics::AABB camAABB(camMin, camMax);

	//ignore aabb
	Vector2 ignoreMin = m_car->getPosition() - Vector2(40.0f, 22.4f);
	Vector2 ignoreMax = m_car->getPosition() + Vector2(40.0f, 22.4f);
	JellyPhysics::AABB ignoreAABB(ignoreMin, ignoreMax);

	//render level bodies
	for (size_t i = 0; i < m_gameBodies.size(); i++) {
		if (m_showMap) {
			m_gameBodies[i]->Draw(GlmFromMatrix(m_camera.getOrthographicMatrix() * m_camera.getViewMatrix()));

			if (ignoreAABB.intersects(m_gameBodies[i]->GetIgnoreAABB())) {
				m_gameBodies[i]->SetIgnore(false);
			}else {
				m_gameBodies[i]->SetIgnore(true);
			}

		}
		else {

			if (ignoreAABB.intersects(m_gameBodies[i]->GetIgnoreAABB())) {
				m_gameBodies[i]->SetIgnore(false);

				if (m_gameBodies[i]->GetIgnoreAABB().intersects(camAABB)) {
					m_gameBodies[i]->Draw(GlmFromMatrix(m_camera.getOrthographicMatrix() * m_camera.getViewMatrix()));
				}
			}else {
				m_gameBodies[i]->SetIgnore(true);
			}
		}
	}

	//car
	m_car->Draw(GlmFromMatrix(m_camera.getOrthographicMatrix() * m_camera.getViewMatrix()));

	//ballon - tire
	for (size_t i = 0; i < m_gameBodies.size(); i++) {
		if (!m_gameBodies[i]->GetBody()->getDisable()) {
			if (m_gameBodies[i]->GetName() == "itemballoon") {
				//draw ballon
				const Texture& texture = Globals::textureManager.get("ballon");
				texture.bind(0);
				auto shader = Globals::shaderManager.getAssetPointer("quad");
				shader->use();
				shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_camera.getViewMatrix() * Matrix4f::Translate(m_gameBodies[i]->GetStartPosition()[0], m_gameBodies[i]->GetStartPosition()[1], 0.0f) * Matrix4f::Scale(static_cast<float>(texture.getWidth()) * 0.02f, static_cast<float>(texture.getHeight()) * 0.02f, 1.0f));
				shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
				Globals::shapeManager.get("quad_half").drawRaw();
				shader->unuse();
				texture.unbind();
			}else if (m_gameBodies[i]->GetName() == "itemstick") {
				const Texture& texture = Globals::textureManager.get("tire");
				texture.bind(0);
				auto shader = Globals::shaderManager.getAssetPointer("quad");
				shader->use();
				shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_camera.getViewMatrix() * Matrix4f::Translate(m_gameBodies[i]->GetStartPosition()[0], m_gameBodies[i]->GetStartPosition()[1], 0.0f) * Matrix4f::Scale(static_cast<float>(texture.getWidth()) * 0.02f, static_cast<float>(texture.getHeight()) * 0.02f, 1.0f));
				shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
				Globals::shapeManager.get("quad_half").drawRaw();
				shader->unuse();
				texture.unbind();
			}
		}
	}

	//_transformMeter
	if (m_transformMeter >= 0.0f) {
		const Texture& texture = Globals::textureManager.get("transform_meter");
		texture.bind(0);

		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate(64.0f, 40.0f, 0.0f) * Matrix4f::Scale(128.0f * m_transformMeter, 32.0f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, m_transformMeter, 1.0f));
		Globals::shapeManager.get("quad_aligned_x").drawRaw();
		shader->unuse();
		texture.unbind();
	}

	if (!m_showMap && m_haveBallon && m_ballonTime > 0.0f) {
		float step = 100.0f / 30.0f;
		float scale = (m_ballonTime * step) / 100.0f;

		const Texture* texture = &Globals::textureManager.get("ballon_back");
		texture->bind(0);
		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate((Application::Width / 2) + 128, 450, 0.0f) * Matrix4f::Scale(static_cast<float>(texture->getWidth()) * scale, -1.0f * static_cast<float>(texture->getHeight()) * scale, 1.0f));
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 0.5f));
		Globals::shapeManager.get("quad_half").drawRaw();

		texture = &Globals::textureManager.get("ballon");
		texture->bind(0);
		shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		Globals::shapeManager.get("quad_half").drawRaw();
		shader->unuse();
		texture->unbind();
	}

	

	if (!m_showMap && m_haveTire && m_tireTime > 0.0f){
		float step = 100.0f / 30.0f;
		float scale = (m_tireTime * step) / 100.0f;

		const Texture* texture = &Globals::textureManager.get("tire_back");
		texture->bind(0);
		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadMatrix("u_transform", m_orthographic * Matrix4f::Translate((Application::Width / 2) - 128, 450, 0.0f) * Matrix4f::Scale(static_cast<float>(texture->getWidth()) * scale, -1.0f * static_cast<float>(texture->getHeight()) * scale, 1.0f));
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 0.5f));
		Globals::shapeManager.get("quad_half").drawRaw();

		texture = &Globals::textureManager.get("tire");
		texture->bind(0);
		shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		Globals::shapeManager.get("quad_half").drawRaw();
		shader->unuse();
		texture->unbind();
	}

	const Texture& texture = Globals::textureManager.get("target");
	texture.bind(0);

	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_camera.getViewMatrix() * Matrix4f::Translate(m_levelTarget.X, m_levelTarget.Y, 0.0f) * Matrix4f::Scale(static_cast<float>(texture.getWidth()) * 0.04f, static_cast<float>(texture.getHeight()) * 0.04f, 1.0f));
	shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	Globals::shapeManager.get("quad_half").drawRaw();
	shader->unuse();

	texture.unbind();


	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);
	std::string time = Fontrenderer::FloatToString(m_time, 2);
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width - 125), Application::Height - 52, time, Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width - 125), Application::Height - 50, time, Vector4f(0.0f, 0.84f, 0.0f, 1.0f));

	std::string jump = Fontrenderer::FloatToString(m_bestJumpLenght, 2);
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width - 125), Application::Height - 82, jump, Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width - 125), Application::Height - 80, jump, Vector4f(0.71f, 0.16f, 0.18f, 1.0f));
	Fontrenderer::Get().drawBuffer();

	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);

	m_mainRT.unbindWrite();
}

void JellyGame::resize(int deltaW, int deltaH) {
	m_screenBounds = Vector4f(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f);
	m_orthographic.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);

	m_mainRT.resize(Application::Width, Application::Height);
	
	if (!m_isActive) {		
		renderScene();
	}	
}

void JellyGame::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new JellyMenu(m_machine));
	}
}


Vector2f JellyGame::touchToScreen(Vector4f screenBound, Vector2f touch) {
	float width = fabsf(screenBound[0]) + fabsf(screenBound[1]);
	float widthFactor = width / static_cast<float>(Application::Width);
	float dragX = (touch[0] * widthFactor) + screenBound[0];

	float height = fabsf(screenBound[3]) + fabsf(screenBound[2]);
	float heightFactor = height / static_cast<float>(Application::Height);

	float dragY = 0.0f;

	if (screenBound[3] < screenBound[2])
		dragY = (touch[1] * heightFactor) + screenBound[3];
	else {
		touch[1] = static_cast<float>(Application::Height) - touch[1];
		dragY = (touch[1] * heightFactor) + screenBound[2];
	}

	return Vector2f(dragX, dragY);
}

void JellyGame::processInput() {
	Keyboard& keyboard = Keyboard::instance();
	Mouse& mouse = Mouse::instance();

	if (keyboard.keyPressed(Keyboard::KEY_ENTER) && !keyboard.keyDown(Keyboard::KEY_RALT)){
		m_machine.addStateAtTop(new JellyDialogPause(m_machine, m_mainRT, this));
		return;
	}
	
	if (keyboard.keyPressed(Keyboard::KEY_W)) {
		m_showMap = !m_showMap;

		if (m_showMap) {
			m_camera.orthographic(m_mapLimits.Min.X, m_mapLimits.Max.X, m_mapLimits.Min.Y, m_mapLimits.Max.Y, -1.0f, 1.0f);
			m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
		}else {
			m_camera.orthographic(-20.0f, 20.0f, -11.2f, 11.2f, -1.0f, 1.0f);
		}
	}

	if (!keyboard.keyDown(Keyboard::KEY_LEFT) && !keyboard.keyDown(Keyboard::KEY_RIGHT))
		m_car->setTorque(0);

	if (keyboard.keyDown(Keyboard::KEY_LEFT)) {
		m_car->setTorque(-1);
	}

	if (keyboard.keyDown(Keyboard::KEY_RIGHT)) {
		m_car->setTorque(1);
	}

	m_car->mChassis->torque = 0.0f;

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		m_car->mChassis->torque = -1.0f;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		m_car->mChassis->torque = 1.0f;
	}

	if (keyboard.keyPressed(Keyboard::KEY_S)) {
		if (m_transformMeter > 0.0f){
			if (m_car->Transform())
				m_transformMeterGrowDir = (m_transformMeterGrowDir >= 0) ? -1 : 1;
		}
	}

	if (m_haveBallon && keyboard.keyPressed(Keyboard::KEY_D)){
		m_ballonActive = !m_ballonActive;
		m_car->UseBallon(m_ballonActive);
	}

	if (m_haveTire &&keyboard.keyPressed(Keyboard::KEY_A)){
		m_tireActive = !m_tireActive;
		m_car->UseNearestGracity(m_tireActive);
	}

	if (keyboard.keyDown(Keyboard::KEY_LEFT) || keyboard.keyDown(Keyboard::KEY_RIGHT)){
		if (m_car->getVelocity().length() > 2.0f && m_slowCar){
			//_audioHelper->PlayFastEngine();
			m_slowCar = false;
			m_fastCar = true;
		}

	}else{
		if (m_fastCar){
			//_audioHelper->PlaySlowEngine();
			m_slowCar = true;
			m_fastCar = false;
		}
	}

	//touch support
	if (m_haveBallon || m_haveTire){
		if (mouse.buttonPressed(Mouse::BUTTON_LEFT)){
			Vector2f touch = touchToScreen(m_screenBounds, Vector2f(mouse.xPos(), mouse.yPos()));
			if (m_haveBallon && m_ballonAABB.contains(Vector2(touch[0], touch[1]))){
				if (!m_ballonPressed){
					m_ballonActive = !m_ballonActive;
					m_car->UseBallon(m_ballonActive);
					m_ballonPressed = true;
				}
			}

			if (m_haveTire && m_tireAABB.contains(Vector2(touch[0], touch[1]))){
				if (!m_tirePressed){
					m_tireActive = !m_tireActive;
					m_car->UseNearestGracity(m_tireActive);

					m_tirePressed = true;
				}
			}
		}else{
			m_ballonPressed = false;
			m_tirePressed = false;
		}
	}
}

void JellyGame::UpdateTransformMeter(float dt){
	if (m_transformMeterGrowDir != 0){
		m_transformMeter += (m_transformMeterRechargeSpeed * dt * (float)m_transformMeterGrowDir);

		if (m_transformMeter <= 0.0f){
			if (m_car->TransformStatus != Car::Normal)
				m_car->Transform();
		}

		if (m_transformMeter <= -0.2f){
			m_transformMeter = -0.2f;
			m_transformMeterGrowDir = 1;
		}

		if ((m_transformMeter > 1.0f)){
			m_transformMeter = 1.0f;
			m_transformMeterGrowDir = 0;
		}
	}
}

bool JellyGame::collisionFilter(Body* bodyA, int bodyApm, Body* bodyB, int bodyBpm1, int bodyBpm2, Vector2 hitPt, float normalVel) {
	if (bodyB->getMaterial() == 5 || bodyA->getMaterial() == 5){
		Body* itemBody = 0;

		if (bodyB->getMaterial() == 5){
			itemBody = bodyB;
		}else{
			itemBody = bodyA;
		}

		if (itemBody->getDisable()){
			return false;
		}

		if (itemBody->GetName() == "itemstick"){
			m_haveTire = true;
			m_tireTime = 30.0f;
		}else if (itemBody->GetName() == "itemballoon"){
			m_haveBallon = true;
			m_ballonTime = 30.0f;
		}else if (itemBody->GetName() == "checkpoint"){
			m_checkpoint = true;
			m_checkpointPosition = Vector2f(itemBody->mDerivedPos.X, itemBody->mDerivedPos.Y + 1);
		}

		itemBody->setDisable(true);

		return false;
	}

	m_isJumping = false;

	//hit sound
	float hitVel = fabsf(normalVel);

	if (hitVel > m_chassisHit){
		m_chassisHit = hitVel;
	}

	return true;
}

glm::mat4 JellyGame::GlmFromMatrix(const Matrix4f& _m) {
	glm::mat4 m;

	m[0][0] = _m[0][0]; m[0][1] = _m[0][1]; m[0][2] = _m[0][2]; m[0][3] = _m[0][3];
	m[1][0] = _m[1][0]; m[1][1] = _m[1][1]; m[1][2] = _m[1][2]; m[1][3] = _m[1][3];
	m[2][0] = _m[2][0]; m[2][1] = _m[2][1]; m[2][2] = _m[2][2]; m[2][3] = _m[2][3];
	m[3][0] = _m[3][0]; m[3][1] = _m[3][1]; m[3][2] = _m[3][2]; m[3][3] = _m[3][3];

	return m;
}

void JellyGame::spawnAtCheckpoint() {
	Vector2 pos = Vector2(m_checkpointPosition[0], m_checkpointPosition[1]);
	Vector2 scale = Vector2(1.0f, 1.0f);

	m_car->getChassisBody()->setVelocity(Vector2(0, 0));
	m_car->getTire(0)->setVelocity(Vector2(0, 0));
	m_car->getTire(1)->setVelocity(Vector2(0, 0));

	m_car->getChassisBody()->setPositionAngle(pos, 0.0f, scale);
	m_car->getTire(0)->setPositionAngle(Vector2(m_checkpointPosition[0] + 1.5f, m_checkpointPosition[1] - 0.3f), 0.0f, scale);
	m_car->getTire(1)->setPositionAngle(Vector2(m_checkpointPosition[0] - 1.3f, m_checkpointPosition[1] - 0.3f), 0.0f, scale);
}