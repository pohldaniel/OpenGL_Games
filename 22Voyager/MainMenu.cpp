#include "MainMenu.h"
#include "Application.h"
#include "Constants.h"
#include "Game.h"
#include "About.h"

MainMenu::MainMenu(StateMachine& machine) : State(machine, CurrentState::MAINMENU) {
	Application::SetCursorIconFromFile("res/cursors/black.cur");
	EventDispatcher::AddMouseListener(this);

	if(!Globals::musicManager.get("background").isPlaying())
		Globals::musicManager.get("background").play("res/Audio/MainMenu.mp3");
}

MainMenu::~MainMenu() {
	EventDispatcher::RemoveMouseListener(this);
}

void MainMenu::fixedUpdate() {}

void MainMenu::update() {
	processInput();
}

void MainMenu::processInput() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyPressed(Keyboard::KEY_2)) {
		
	}
}

void MainMenu::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("hud");

	shader->use();
	shader->loadMatrix("projection", Matrix4f::IDENTITY);
	shader->loadMatrix("model", Matrix4f::IDENTITY);

	Globals::textureManager.get("mainMenu").bind(0);
	Globals::shapeManager.get("quad").drawRaw();
	
	if (m_highlight) {
		glDepthFunc(GL_ALWAYS);
		shader->loadMatrix("projection", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f));
		shader->loadMatrix("model", m_transform.getTransformationMatrix());

		Globals::textureManager.get("indicator").bind(0);
		Globals::shapeManager.get("quad").drawRaw();
		glDepthFunc(GL_LESS);
	}
	
	shader->unuse();

#if DEBUG_MENU
	glDepthFunc(GL_ALWAYS);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f)[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);

	float imageWidth = static_cast<float>(Globals::textureManager.get("mainMenu").getWidth());
	float imageHeight = static_cast<float>(Globals::textureManager.get("mainMenu").getHeight());

	float x = static_cast<float>(Application::Width) * (1.0f / imageWidth) * 800;
	float y = static_cast<float>(Application::Height) - static_cast<float>(Application::Height) * (1.0f / imageHeight)  * (265.0f + 40.0f);
	float w = static_cast<float>(Application::Width) * (1.0f / imageWidth) * 115.0f;
	float h = static_cast<float>(Application::Height) * (1.0f / imageHeight) * 40.0f;

	glBegin(GL_QUADS);
	glVertex3f(x, y, 0.0f);
	glVertex3f(x + w, y, 0.0f);
	glVertex3f(x + w, y + h, 0.0f);
	glVertex3f(x, y + h, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);

	x = static_cast<float>(Application::Width) * (1.0f / imageWidth) * 790;
	y = static_cast<float>(Application::Height) - static_cast<float>(Application::Height) * (1.0f / imageHeight)  * (343.0f + 40.0f);
	w = static_cast<float>(Application::Width) * (1.0f / imageWidth) * 142.0f;
	h = static_cast<float>(Application::Height) * (1.0f / imageHeight) * 40.0f;

	glBegin(GL_QUADS);
	glVertex3f(x, y, 0.0f);
	glVertex3f(x + w, y, 0.0f);
	glVertex3f(x + w, y + h, 0.0f);
	glVertex3f(x, y + h, 0.0f);
	glEnd();


	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);

	x = static_cast<float>(Application::Width) * (1.0f / imageWidth) * 816;
	y = static_cast<float>(Application::Height) - static_cast<float>(Application::Height) * (1.0f / imageHeight)  * (418.0f + 40.0f);
	w = static_cast<float>(Application::Width) * (1.0f / imageWidth) * 86.0f;
	h = static_cast<float>(Application::Height) * (1.0f / imageHeight) * 40.0f;

	glBegin(GL_QUADS);
	glVertex3f(x, y, 0.0f);
	glVertex3f(x + w, y, 0.0f);
	glVertex3f(x + w, y + h, 0.0f);
	glVertex3f(x, y + h, 0.0f);
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthFunc(GL_LESS);
#endif
}

void MainMenu::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_transform.reset();
	

	float imageWidth = static_cast<float>(Globals::textureManager.get("mainMenu").getWidth());
	float imageHeight = static_cast<float>(Globals::textureManager.get("mainMenu").getHeight());

	float x = static_cast<float>(Application::Width) * (1.0f / imageWidth);
	float y = static_cast<float>(Application::Height) * (1.0f / imageHeight);

	if ((event.x > x * 800.0f &&  event.x < x * 915.0f) && (event.y >= y * 265.0f && event.y <= y * 305.0f)) {	
		if(!m_highlight)
			Globals::soundManager.get("mainMenu").playChannel(0u);
		m_transform.scale(x * 25.0f, y * 25.0f, 1.0f);
		m_transform.translate(x * (800.0f - 40.0f), static_cast<float>(Application::Height) - y * (265.0f + 15.0f), 0.0f);
		m_highlight = true;

	}else if ((event.x > x * 790.0f &&  event.x < x * 932.0f) && (event.y >= y * 343.0f && event.y <= y * 383.0f)) {
		if (!m_highlight)
			Globals::soundManager.get("mainMenu").playChannel(0u);
		m_transform.scale(x * 25.0f, y * 25.0f, 1.0f);
		m_transform.translate(x * (790.0f - 40.0f), static_cast<float>(Application::Height) - y * (343.0f + 15.0f), 0.0f);
		m_highlight = true;
		
	}else if ((event.x > x * 816.0f &&  event.x < x * 902.0f) && (event.y >= y * 418.0f && event.y <= y * 458.0f)){
		if (!m_highlight)
			Globals::soundManager.get("mainMenu").playChannel(0u);
		m_transform.scale(x * 25.0f, y * 25.0f, 1.0f);
		m_transform.translate(x * (816.0f - 40.0f), static_cast<float>(Application::Height) - y * (418.0f + 15.0f), 0.0f);
		m_highlight = true;	
	}else {
		m_highlight = false;
	}
}

void MainMenu::OnMouseButtonDown(Event::MouseButtonEvent& event) {

	float imageWidth = static_cast<float>(Globals::textureManager.get("mainMenu").getWidth());
	float imageHeight = static_cast<float>(Globals::textureManager.get("mainMenu").getHeight());

	float x = static_cast<float>(Application::Width) * (1.0f / imageWidth);
	float y = static_cast<float>(Application::Height) * (1.0f / imageHeight);

	if ((event.x > x * 800.0f &&  event.x < x * 915.0f) && (event.y >= y * 265.0f && event.y <= y * 305.0f)) {
		Globals::soundManager.get("mainMenu").playChannel(1u);
		Globals::musicManager.get("background").stop();	
		Mouse::instance().attach(Application::GetWindow());
		m_isRunning = false;
		m_machine.addStateAtBottom(new Game(m_machine));
	} else if ((event.x > x * 790.0f &&  event.x < x * 932.0f) && (event.y >= y * 343.0f && event.y <= y * 383.0f)) {
		Globals::soundManager.get("mainMenu").playChannel(1u);
		m_isRunning = false;
		m_machine.addStateAtBottom(new About(m_machine));
	} else if ((event.x > x * 816.0f &&  event.x < x * 902.0f) && (event.y >= y * 418.0f && event.y <= y * 458.0f)) {
		Globals::soundManager.get("mainMenu").playChannel(1u);
		m_isRunning = false;
	}
}