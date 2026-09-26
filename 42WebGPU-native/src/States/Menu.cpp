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

	uiInit(static_cast<float>(Application::Width), static_cast<float>(Application::Height));

	m_surface = new Surface();
	m_surface->setScale(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
	m_surface->setColor(Vector4f::ZERO);

	Button* nested = m_surface->addChild<Button>();
	nested->setScale(0.1f, 0.1f);
	nested->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	nested->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	nested->setPosition(0.05f, 0.05f);
	nested->setOutlineThickness(5.0f);
	nested->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Wireframe(m_machine));
	});

	nested = m_surface->addChild<Button>();
	nested->setScale(0.1f, 0.1f);
	nested->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	nested->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	nested->setPosition(0.2f, 0.05f);
	nested->setOutlineThickness(5.0f);
	nested->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Compute(m_machine));
	});

	nested = m_surface->addChild<Button>();
	nested->setScale(0.1f, 0.1f);
	nested->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	nested->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	nested->setPosition(0.35f, 0.05f);
	nested->setOutlineThickness(5.0f);
	nested->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Specularity(m_machine));
	});

	nested = m_surface->addChild<Button>();
	nested->setScale(0.1f, 0.1f);
	nested->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	nested->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	nested->setPosition(0.5f, 0.05f);
	nested->setOutlineThickness(5.0f);
	nested->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new NormalMap(m_machine));
	});

	wgpContext.OnDraw = std::bind(&Menu::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
}

Menu::~Menu() {
	EventDispatcher::RemoveKeyboardListener(this);
	delete m_surface;
}

void Menu::fixedUpdate() {

}

void Menu::update() {
	Mouse& mouse = Mouse::instance();
	m_surface->input(mouse.xPos(), mouse.yPos(), mouse.buttonDown(Mouse::MouseButton::BUTTON_LEFT));
	m_surface->draw();
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