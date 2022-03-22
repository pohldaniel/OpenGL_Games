#include "Game.h"

#include "Random.h"
#include "Menu.h"
#include "Pause.h"
#include "AssetManger.h"
Game::Game(StateMachine& machine) : State(machine){	
	InitEntities();
	initWalls();
	initSprites();
	initLights();
	InitCountdown();
	initTimers();

	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_quad = new Quad(false);

	m_quadBackground = new Quad(false, 0.0f, 0.0f, 1.0f, 1.2f, 1.0f, 1.0f);
	//m_transBackground = Matrix4f::Translate(0.0, -0.2, 0.0);
	m_transBackground = Matrix4f::Translate(m_transBackground, 0.0, -0.2, 0.0);
	
	m_fog = new Quad(false);
	m_shaderFog = Globals::shaderManager.getAssetPointer("fog");

	m_healthBar = new HealthBar();
	m_healthBar->setPosition(Vector2f(0.0f, HEIGHT));
}

Game::~Game() {
	delete m_player;
	delete m_fog;
	delete m_quad;
	delete m_quadBackground;
}

void Game::fixedUpdate() {
	if (m_player->isAlive())
		FixedUpdateEntities();
}

void Game::update() {
	if (m_player->isAlive()) {
		UpdateEntities();
		//m_bestTime = m_timeClock.getElapsedTime().asSeconds();
	}

	//UpdateCountdown();

	if ((Globals::CONTROLLS & Globals::KEY_Q)) {
		i_machine.addStateAtTop(new Pause(i_machine));
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

	m_healthBar->render();		
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

	float time = m_clock.getElapsedTimeSec();
	for (const auto& f : m_fireballs) {
		f->render(time);
	}

	for (const auto& g : m_ghosts)
		g->render(time);

	#if !DEBUGCOLLISION
	glEnable(GL_BLEND);
	glUseProgram(m_shaderFog->m_program);
	m_shaderFog->loadVector("u_resolution", Vector2f(WIDTH, HEIGHT));
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
	#endif
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Game::FixedUpdateEntities() {
	m_player->fixedUpdate();

	for(auto& g : m_ghosts)
		g->fixedUpdate();

	for(auto& f : m_fireballs)
		f->fixedUpdate();
}

void Game::UpdateEntities() {
	m_player->resolveCollision(m_walls);
	m_player->update();
	updateTimers();

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
}

void Game::InitEntities() {
	m_player = new Player(i_dt, i_fdt);
}

void Game::updateTimers() {
	m_enemySpawnTimer.Update(i_dt);
	m_ghostSpawnTimer.Update(i_dt);
	m_gameSpeedTimer.Update(i_dt);
}

void Game::initTimers() {

	/*m_enemySpawnTimer.SetFunction(1.0f, [&]() {
		if (m_fireballs.size() > 8)
			return;

		const float velocity = 450.0f * Random::RandFloat(1.01f, 1.82f);

		m_fireballs.push_back(new Fireball(i_dt, i_fdt, velocity, Random::RandInt(0, 1)));

	});*/

	m_gameSpeedTimer.SetFunction(5.5f, [&]() {
		const float sub = Random::RandFloat(0.085f, 0.125f);
		const float uTime = m_enemySpawnTimer.GetUpdateTime() - sub;

		m_enemySpawnTimer.SetUpdateTime
		(
			uTime < 0.285f ? 0.285f : uTime
		);

		m_ghostSpawnTimer.SetUpdateTime(m_ghostSpawnTimer.GetUpdateTime() - sub < 7.185f ? 7.185f : m_ghostSpawnTimer.GetUpdateTime() - sub);
	});

	m_ghostSpawnTimer.SetFunction(1.0f, [&]() {
		if (m_ghosts.size() > 2)
			return;

		m_ghosts.push_back(new Ghost(i_dt, i_fdt));
	});
}

void Game::UpdateCountdown() {
	//m_timeClock.restart();
}

void Game::InitCountdown() {
	
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
	//m_walls.push_back(Wall(Vector2f(0.0f, 0.0f), Vector2f(192.0f, 32.0f)));
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
