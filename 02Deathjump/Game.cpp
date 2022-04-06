#include "Game.h"
#include "Menu.h"
#include "Pause.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME){
	initEntities();
	initWalls();
	initSprites();
	initLights();
	initCountdown();
	initText();
	initTimers();
	
	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_quad = new Quad(false);

	m_quadBackground = new Quad(false, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.2f);
	m_transBackground = Matrix4f::Translate(m_transBackground, 0.0, -0.2, 0.0);
	
	m_fog = new Quad(false);
	m_shaderFog = Globals::shaderManager.getAssetPointer("fog");

	Globals::musicManager.get("main").play();
	Globals::musicManager.get("main").setLooping(true);
}

Game::~Game() {
	delete m_player;
	delete m_fog;
	delete m_quad;
	delete m_quadBackground;
	delete m_text;
	delete m_countdown;
}

void Game::fixedUpdate() {
	if (m_player->isAlive())
		fixedUpdateEntities();
}

void Game::update() {
	if (m_player->isAlive()) {
		updateEntities();
		m_bestTime = m_timeClock.getElapsedTimeSec();
	}else {
		Transition::get().setFunction([&]() {
			if (m_bestTime > Globals::bestTime) {
				std::stringstream ss;
				ss << std::fixed << std::setprecision(2) << m_bestTime;
				Globals::bestTime = std::stof(ss.str());
			}
			m_machine.clearAndPush(new Menu(m_machine));

			Globals::musicManager.get("main").stop();

			Transition::get().start(Mode::Unveil);
		});
		Transition::get().start(Mode::Veil);
	}

	updateCountdown();

	if((Globals::CONTROLLS & Globals::KEY_ESCAPE)) {
		m_machine.addStateAtTop(new Pause(m_machine));
		Globals::musicManager.get("main").stop();
	}
}

void Game::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	#if !DEBUGCOLLISION
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", m_transBackground);
	m_quadBackground->render(m_sprites["background"]);
	glUseProgram(0);
	#endif
	m_player->render();

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	m_quad->render(m_sprites["foreground"]);
	glUseProgram(0);

	#if DEBUGCOLLISION
	for (auto& o : m_walls)
	o.render();
	#endif

	glEnable(GL_BLEND);
	m_countdown->render();	
	if (m_countdown->currentFrame() >= 4)
		m_text->render(m_timer);
	glDisable(GL_BLEND);

	float time = m_clock.getElapsedTimeSec();
	for (const auto& f : m_fireballs) {
		f->render(time);
	}

	for (const auto& g : m_ghosts)
		g->render(time);

	if (m_heart)
		m_heart->render(time);

	#if !DEBUGCOLLISION
	glEnable(GL_BLEND);
	glUseProgram(m_shaderFog->m_program);
	m_shaderFog->loadFloat("u_time", m_clock.getElapsedTimeSec());
	m_quad->render();
	glUseProgram(0);
	
	for (const auto& l : m_lights) {
		glUseProgram(l.getShader().m_program);
		l.getShader().loadVector("u_color", Vector4f(0.65, 0.50, 0.28, 0.8));
		l.getShader().loadFloat("u_time", time);
		glUseProgram(0);
		l.render();

	}
	
	glDisable(GL_BLEND);
	#endif
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::spawnHeart() {
	if (m_heartSpawnClock.getElapsedTimeSec() > 27.5f) {
		m_heart = new Heart(m_dt, m_fdt);
	}
}

void Game::fixedUpdateEntities() {
	m_player->fixedUpdate();

	if (m_heart)
		m_heart->fixedUpdate();

	for(auto& g : m_ghosts)
		g->fixedUpdate();

	for(auto& f : m_fireballs)
		f->fixedUpdate();
}

void Game::updateEntities() {
	m_player->resolveCollision(m_walls);
	if (m_heart)
		m_player->resolveCollision(m_heart);
	m_player->update();
	
	if (m_heart) {
		if (!m_heart->isAlive) {
			delete m_heart;
			m_heart = nullptr;
		}else {
			m_heart->update();
			m_heart->resolveCollision(m_walls);
			m_heartSpawnClock.restart();
		}
	}else {
		spawnHeart();
	}

	// Fireballs
	for (int i = 0; i < m_fireballs.size(); i++) {
		auto& fireball = m_fireballs[i];
		if (!fireball->isAlive()) {
			delete fireball;
			m_fireballs.erase(m_fireballs.begin() + i);
			
			continue;
		}
		
		m_player->resolveCollision(fireball);

		fireball->update();
	}

	// Ghosts
	for (int i = 0; i < m_ghosts.size(); i++) {
		auto& ghost = m_ghosts[i];

		if (!ghost->m_isAlive) {
			delete ghost;
			m_ghosts.erase(m_ghosts.begin() + i);

			continue;
		}

		m_player->resolveCollision(ghost, m_ghosts);

		ghost->update(m_player->getCollider().getBody());
	}

	if (m_countdown->currentFrame() >= 4)
		updateTimers();
}

void Game::initEntities() {
	m_player = new Player(m_dt, m_fdt);
}

void Game::updateTimers() {
	m_fireballSpawnTimer.update(m_dt);
	m_ghostSpawnTimer.update(m_dt);
	m_gameSpeedTimer.update(m_dt);
}

void Game::initTimers() {

	m_gameSpeedTimer.setFunction(5.5f, [&]() {
		const float sub = Random::RandFloat(0.085f, 0.125f);
		const float uTime = m_fireballSpawnTimer.getUpdateTime() - sub;

		m_fireballSpawnTimer.setUpdateTime(uTime < 0.285f ? 0.285f : uTime);
		m_ghostSpawnTimer.setUpdateTime(m_ghostSpawnTimer.getUpdateTime() - sub < 7.185f ? 7.185f : m_ghostSpawnTimer.getUpdateTime() - sub);
	});

	m_fireballSpawnTimer.setFunction(1.0f, [&]() {
		if (m_fireballs.size() > 8)
			return;

		const float velocity = 450.0f * Random::RandFloat(1.01f, 1.82f);

		m_fireballs.push_back(new Fireball(m_dt, m_fdt, velocity, Random::RandInt(0, 1)));

	});

	m_ghostSpawnTimer.setFunction(8.5f, [&]() {
		if (m_ghosts.size() > 2)
			return;

		m_ghosts.push_back(new Ghost(m_dt, m_fdt));
	});
}

void Game::updateCountdown() {
	if (m_countdown->currentFrame() < 4) {
		m_countdown->update();
		m_timeClock.restart();
	}else {
		updateText();
	}
}

void Game::initCountdown() {
	m_countdown = new Countdown();
}

void Game::initText() {
	m_text = new Text(30, Globals::fontManager.get("font_200"), 90.0f / 200.0f);
	m_text->setPosition(WIDTH * 0.5f, HEIGHT - 60.0f);
	m_text->setOrigin(100.0f, m_text->getSize()[1]);
}

void Game::updateText() {
	m_timer = Text::floatToString(m_timeClock.getElapsedTimeSec(), 2);
}

void Game::initLights() {
	m_lights.push_back(Light(Vector2f(460.0f, 490.0f), 275.0f));
	m_lights.push_back(Light(Vector2f(210.0f, 200.0f), 275.0f));
	m_lights.push_back(Light(Vector2f(1125.0f, 395.0f), 275.0f));
	m_lights.push_back(Light(Vector2f(1350.0f, 780.0f), 275.0f));
}


void Game::initSprites() {
	m_sprites["foreground"] = Globals::textureManager.get("foreground").getTexture();
	//m_Sprites["background"] = Globals::textureManager.get("background").getTexture();
	m_sprites["background"] = AssetManagerStatic<Texture>::get().get("background").getTexture();
	m_sprites["player"] = Globals::textureManager.get("player").getTexture();
}

void Game::initWalls() {
	m_walls.push_back(Wall(Vector2f(WIDTH / 2.0f, 855.0f) - Vector2f(WIDTH, 100.0f) * 0.5f, Vector2f(WIDTH, 100.0f)));
	m_walls.push_back(Wall(Vector2f(800.0f, 772.0f)  - Vector2f(64.0f, 64.0f)  * 0.5f, Vector2f(64.0f, 64.0f)));
	m_walls.push_back(Wall(Vector2f(640.0f, 660.0f)  - Vector2f(128.0f, 32.0f) * 0.5f, Vector2f(128.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(1072.0f, 660.0f) - Vector2f(96.0f, 32.0f)  * 0.5f, Vector2f(96.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(192.0f, 596.0f)  - Vector2f(192.0f, 32.0f) * 0.5f, Vector2f(192.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(435.0f, 530.0f)  - Vector2f(160.0f, 32.0f) * 0.5f, Vector2f(160.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(880.0f, 530.0f)  - Vector2f(224.0f, 32.0f) * 0.5f, Vector2f(224.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(658.0f, 436.0f)  - Vector2f(96.0f, 32.0f)  * 0.5f, Vector2f(96.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(80.0f, 466.0f)   - Vector2f(96.0f, 32.0f)  * 0.5f, Vector2f(96.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(1182.0f, 436.0f) - Vector2f(192.0f, 32.0f) * 0.5f, Vector2f(192.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(398.0f, 340.0f)  - Vector2f(224.0f, 32.0f) * 0.5f, Vector2f(224.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(815.0f, 340.0f)  - Vector2f(96.0f, 32.0f)  * 0.5f, Vector2f(96.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(1377.0f, 340.0f) - Vector2f(128.0f, 32.0f) * 0.5f, Vector2f(128.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(176.0f, 244.0f)  - Vector2f(160.0f, 32.0f) * 0.5f, Vector2f(160.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(960.0f, 244.0f)  - Vector2f(192.0f, 32.0f) * 0.5f, Vector2f(192.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(1230.0f, 180.0f) - Vector2f(160.0f, 32.0f) * 0.5f, Vector2f(160.0f, 32.0f)));
	m_walls.push_back(Wall(Vector2f(1490.0f, 116.0f) - Vector2f(160.0f, 32.0f) * 0.5f, Vector2f(160.0f, 32.0f)));
}
