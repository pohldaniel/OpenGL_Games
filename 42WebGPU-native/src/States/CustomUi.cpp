#include <WebGPU/WgpContext.h>

#include "CustomUi.h"
#include "Application.h"

CustomUi::CustomUi(StateMachine& machine) : State(machine, States::CUSTOM_UI) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	Widget::Init(static_cast<float>(Application::Width), static_cast<float>(Application::Height));

	m_surface = new Surface();
	m_surface->setScale(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
	Button* nested = m_surface->addChild<Button>();
	nested->setScale(0.5f, 0.5f);
	nested->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	nested->setOutlineColor(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));

	//Surface* nested = m_surface->addChild<Surface>();
	//nested->setScale(0.5f, 0.5f);
	//nested->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

	

	wgpContext.OnDraw = std::bind(&CustomUi::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
}

CustomUi::~CustomUi() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void CustomUi::fixedUpdate() {

}

void CustomUi::update() {
	m_surface->draw();
}

void CustomUi::render() {
	wgpDraw();
}

void CustomUi::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	Widget::Draw(commandEncoder, renderPassDescriptor);
}