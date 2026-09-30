#include <WebGPU/WgpContext.h>
#include <engine/ui/UiContext.h>

#include <States/Wireframe.h>
#include <States/Compute.h>
#include <States/Specularity.h>
#include <States/NormalMap.h>
#include <States/MSDFFont.h>
#include <States/InstancedCube.h>
#include <States/ImageBasedLighting.h>
#include <States/ShadowMapping.h>
#include <States/SkinnedMesh.h>
#include <States/ComputeParticleLogo.h>
#include <States/PrimitivePicking.h>
#include <States/StencilMask.h>
#include <States/DeferredRendering.h>
#include <States/VolumeRendering.h>
#include <States/OcclusionQuery.h>
#include <States/RenderBundles.h>
#include <States/NuklearUi.h>
#include <States/AudioDecode.h>
#include <States/VideoDecode.h>
#include <States/Cubes.h>
#include <States/Isometric.h>

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
	m_uiScene->setLayout(Layout::VERTICAL);
	m_uiScene->setSpacing(0.0f, 50.0f);

	Surface* surface = m_uiScene->addChild<Surface>();
	surface->setColor(Vector4f(0.2f, 0.7f, 0.2f, 1.0f));
	surface->setPadding(20.0f, 20.0f);
	surface->setSpacing(15.0f, 0.0f);

	Button* button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setPosition(0.05f, 0.05f);
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Wireframe(m_machine));
	});

	Label* label = button->addChild<Label>(m_characterSet);
	label->setText("Wireframe");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setPosition(0.05f, 0.35f);
	button->setOutlineThickness(5.0f);
	button->setPadding(0.0f, 0.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Compute(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Compute");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setPosition(0.35f, 0.35f);
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Specularity(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Specularity");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setPosition(0.05f, 0.5f);
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new NormalMap(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Normal Map");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	surface = m_uiScene->addChild<Surface>();
	surface->setColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
	surface->setPadding(20.0f, 20.0f);
	surface->setSpacing(15.0f, 0.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new MSDFFont(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("MSDF Font");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new InstancedCube(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Instanced Cube");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new ImageBasedLighting(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Image Based Lighting");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);
	
	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new ShadowMapping(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Shadow Mapping");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	surface = m_uiScene->addChild<Surface>();
	surface->setColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
	surface->setPadding(20.0f, 20.0f);
	surface->setSpacing(15.0f, 0.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new SkinnedMesh(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Skinned Mesh");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new ComputeParticleLogo(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Compute Particle Logo");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new PrimitivePicking(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Primitive Picking");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new StencilMask(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Stencil Mask");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	surface = m_uiScene->addChild<Surface>();
	surface->setColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
	surface->setPadding(20.0f, 20.0f);
	surface->setSpacing(15.0f, 0.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new DeferredRendering(m_machine));
		});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Defferred Rendering");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new VolumeRendering(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Volume Rendering");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new OcclusionQuery(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Occlusion Query");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new RenderBundles(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Render Bundles");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	surface = m_uiScene->addChild<Surface>();
	surface->setColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
	surface->setPadding(20.0f, 20.0f);
	surface->setSpacing(15.0f, 0.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new NuklearUi(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Nuklear UI");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new AudioDecode(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Audio Decode");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new VideoDecode(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Video Decode");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Cubes(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Cubes");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, 5.0f);

	surface = m_uiScene->addChild<Surface>();
	surface->setColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
	surface->setPadding(20.0f, 20.0f);
	surface->setSpacing(15.0f, 0.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Isometric(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Isomeric");
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
	m_uiScene->input(mouse.xPos(), mouse.yPos(), mouse.buttonDown(Mouse::MouseButton::BUTTON_LEFT));
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