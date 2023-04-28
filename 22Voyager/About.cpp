#include "About.h"
#include "Application.h"
#include "Constants.h"
#include "MainMenu.h"

About::About(StateMachine& machine) : State(machine, CurrentState::ABOUT) {
	Application::SetCursorIcon("res/cursors/black.cur");
	EventDispatcher::AddMouseListener(this);
}

About::~About() {
	EventDispatcher::RemoveMouseListener(this);
}

void About::fixedUpdate() {}

void About::update() {}

void About::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("hud");

	shader->use();
	shader->loadMatrix("projection", Matrix4f::IDENTITY);
	shader->loadMatrix("model", Matrix4f::IDENTITY);

	Globals::textureManager.get("aboutMenu").bind(0);
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

#if MENU
	glDepthFunc(GL_ALWAYS);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f)[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);

	float imageWidth = static_cast<float>(Globals::textureManager.get("aboutMenu").getWidth());
	float imageHeight = static_cast<float>(Globals::textureManager.get("aboutMenu").getHeight());

	float x = static_cast<float>(Application::Width) * (1.0f / imageWidth) * 54;
	float y = static_cast<float>(Application::Height) - static_cast<float>(Application::Height) * (1.0f / imageHeight)  * (804.0f + 55.0f);
	float w = static_cast<float>(Application::Width) * (1.0f / imageWidth) * 95.0f;
	float h = static_cast<float>(Application::Height) * (1.0f / imageHeight) * 55.0f;

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

void About::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_transform.reset();
	

	float imageWidth = static_cast<float>(Globals::textureManager.get("aboutMenu").getWidth());
	float imageHeight = static_cast<float>(Globals::textureManager.get("aboutMenu").getHeight());

	float x = static_cast<float>(Application::Width) * (1.0f / imageWidth);
	float y = static_cast<float>(Application::Height) * (1.0f / imageHeight);

	if ((event.x > x * 54.0f &&  event.x < x * 194.0f) && (event.y >= y * 804.0f && event.y <= y * 859.0f)) {
		if (!m_highlight)
			Globals::soundManager.get("mainMenu").playChannel(0u);
		m_transform.scale(x * 20.0f, y * 20.0f, 1.0f);
		m_transform.translate(x * (54.0f - 25.0f), static_cast<float>(Application::Height) - y * (804.0f + 20.0f), 0.0f);
		m_highlight = true;
	} else {
		m_highlight = false;
	}
}

void About::OnMouseButtonDown(Event::MouseButtonEvent& event) {

	float imageWidth = static_cast<float>(Globals::textureManager.get("aboutMenu").getWidth());
	float imageHeight = static_cast<float>(Globals::textureManager.get("aboutMenu").getHeight());

	float x = static_cast<float>(Application::Width) * (1.0f / imageWidth);
	float y = static_cast<float>(Application::Height) * (1.0f / imageHeight);

	if ((event.x > x * 54.0f &&  event.x < x * 194.0f) && (event.y >= y * 804.0f && event.y <= y * 859.0f)) {
		Globals::soundManager.get("mainMenu").playChannel(1u);
		m_isRunning = false;
		m_machine.addStateAtBottom(new MainMenu(m_machine));
	}
}