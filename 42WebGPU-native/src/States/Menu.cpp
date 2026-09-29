#include <WebGPU/WgpContext.h>
#include <engine/ui/UiContext.h>

#include <States/Wireframe.h>
#include <States/Compute.h>
#include <States/Specularity.h>
#include <States/NormalMap.h>

#include "Menu.h"
#include "Application.h"

Menu::Menu(StateMachine& machine) : State(machine, States::MENU) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	m_characterSet.loadFromFile("res/fonts/upheavtt.ttf", 32.0f);
	uiContext.textureView = m_characterSet.texture.getTextureView();
	uiInit(static_cast<float>(Application::Width), static_cast<float>(Application::Height));

	m_uiScene = new Empty();
	m_uiScene->setScale(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
	m_uiScene->setLayout(Layout::VERTICAL);

	Surface* surface = m_uiScene->addChild<Surface>();
	surface->setColor(Vector4f(0.2f, 0.7f, 0.2f, 1.0f));
	surface->setScale(0.5f, 0.5f);

	surface  = m_uiScene->addChild<Surface>();
	surface->setScale( 0.5f,  0.5f);
	surface->setColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
	surface->setPadding(10.0f, 5.0f);
	surface->setSpacing(15.0f, 0.0f);
	surface->setDrag(true);
	surface->setPosition(200.0f / static_cast<float>(Application::Width), 200.0f / static_cast<float>(Application::Height));

	Button* button = surface->addChild<Button>();
	button->setScale(0.1f, 0.1f);
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setPosition(0.05f, 0.05f);
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Wireframe(m_machine));
	});

	Label* label = button->addChild<Label>(m_characterSet);
	label->setText("Wireframe");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setScale(0.2f, 0.2f);
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setPosition(0.05f, 0.35f);
	button->setOutlineThickness(5.0f);
	button->setPadding(0.0f, 0.0f);
	button->setOnClick([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Compute(m_machine));
	});

	/*label = button->addChild<Label>(m_characterSet);
	label->setText("Compute");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);*/

	button = surface->addChild<Button>();
	button->setScale(0.1f, 0.1f);
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setPosition(0.35f, 0.35f);
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Specularity(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Specularity");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setScale(0.1f, 0.1f);
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setPosition(0.05f, 0.5f);
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new NormalMap(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Normal Map");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	wgpContext.setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	wgpContext.OnDraw = std::bind(&Menu::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
}

Menu::~Menu() {
	EventDispatcher::RemoveKeyboardListener(this);
	delete m_uiScene;
}

void Menu::fixedUpdate() {

}

void Menu::update() {
	Mouse& mouse = Mouse::instance();
	m_uiScene->inputTree(mouse.xPos(), mouse.yPos(), mouse.buttonDown(Mouse::MouseButton::BUTTON_LEFT));
	m_uiScene->createTree();
}

void Menu::render() {
	wgpDraw();
}

void Menu::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	uiDraw(commandEncoder, renderPassDescriptor);
}

void Menu::resize(int deltaW, int deltaH) {
	uiResize(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
}

void Menu::OnKeyDown(const Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		uiShutDown();
		wgpCleanState();
		m_isRunning = false;
	}
}