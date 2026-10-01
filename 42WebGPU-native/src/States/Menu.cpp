#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

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
#include "Globals.h"

Menu::Menu(StateMachine& machine) : State(machine, States::MENU) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	m_characterSet.loadFromFile("res/fonts/upheavtt.ttf", 24.0f);
	uiContext.textureView = m_characterSet.texture.getTextureView();
	uiInit(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
	float paddingBottom = 3.0f;

	m_uiScene = new Empty();
	m_uiScene->setPadding(20.0f, 20.0f);
	m_uiScene->setSpacing(25.0f, 25.0f);
	m_uiScene->setLayout(Layout::MASONRY);

	Surface* surface = m_uiScene->addChild<Surface>();
	surface->setColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
	surface->setPadding(20.0f, 20.0f);
	surface->setSpacing(25.0f, 25.0f);
	surface->setLayout(Layout::GRID);

	Button* button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setPosition(0.05f, 0.05f);
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Wireframe(m_machine));
	});

	Label* label = button->addChild<Label>(m_characterSet);
	label->setText("Wireframe");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setPosition(0.05f, 0.35f);
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Compute(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Compute");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setPosition(0.35f, 0.35f);
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Specularity(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Specularity");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setPosition(0.05f, 0.5f);
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new NormalMap(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Normal Map");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	surface = m_uiScene->addChild<Surface>();
	surface->setColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
	surface->setPadding(20.0f, 20.0f);
	surface->setSpacing(25.0f, 25.0f);
	surface->setLayout(Layout::GRID);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new MSDFFont(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("MSDF Font");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new InstancedCube(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Instanced Cube");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(5.0f, paddingBottom);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new ImageBasedLighting(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Image Based Lighting");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);
	
	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new ShadowMapping(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Shadow Mapping");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	surface = m_uiScene->addChild<Surface>();
	surface->setColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
	surface->setPadding(20.0f, 20.0f);
	surface->setSpacing(25.0f, 25.0f);
	surface->setLayout(Layout::GRID);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new SkinnedMesh(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Skinned Mesh");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new ComputeParticleLogo(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Compute Particle Logo");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new PrimitivePicking(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Primitive Picking");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new StencilMask(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Stencil Mask");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	surface = m_uiScene->addChild<Surface>();
	surface->setColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
	surface->setPadding(20.0f, 20.0f);
	surface->setSpacing(25.0f, 25.0f);
	surface->setLayout(Layout::GRID);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new DeferredRendering(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Defferred Rendering");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new VolumeRendering(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Volume Rendering");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new OcclusionQuery(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Occlusion Query");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new RenderBundles(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Render Bundles");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	surface = m_uiScene->addChild<Surface>();
	surface->setColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
	surface->setPadding(20.0f, 20.0f);
	surface->setSpacing(25.0f, 25.0f);
	surface->setLayout(Layout::GRID);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new NuklearUi(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Nuklear UI");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new AudioDecode(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Audio Decode");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new VideoDecode(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Video Decode");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Cubes(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Cubes");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

	surface = m_uiScene->addChild<Surface>();
	surface->setColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
	surface->setPadding(20.0f, 20.0f);
	surface->setSpacing(15.0f, 0.0f);

	button = surface->addChild<Button>();
	button->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	button->setOutlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	button->setOutlineThickness(5.0f);
	button->setPadding(5.0f, 5.0f);
	button->setOnClick([&]() {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Isometric(m_machine));
	});

	label = button->addChild<Label>(m_characterSet);
	label->setText("Isomeric");
	label->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	label->setPadding(0.0f, paddingBottom);

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
	if(!ImGui::GetIO().WantCaptureMouse)
		m_uiScene->input(mouse.xPos(), mouse.yPos(), mouse.buttonDown(Mouse::MouseButton::BUTTON_LEFT));
	m_uiScene->draw();
}

void Menu::render() {
	wgpDraw();
}

void Menu::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	uiDraw(commandEncoder, renderPassDescriptor);

	if (m_drawUi)
	{
		WGPURenderPassColorAttachment renderPassColorAttachment = renderPassDescriptor.colorAttachments[0];
		renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Load;

		WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
		rndrPssDscrptor.colorAttachments = &renderPassColorAttachment;

		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &rndrPssDscrptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
		renderUi(renderPassEncoder);
		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}
}

void Menu::resize(int deltaW, int deltaH) {
	uiResize(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
}

void Menu::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
	ImGui_ImplWGPU_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceId = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();

	if (m_initUi) {
		m_initUi = false;
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_down);
	}

	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	int currentLayout = m_layout;
	if (ImGui::Combo("Model", &currentLayout, "Horizontal\0Vertical\0Grid\0\0")) {
		m_layout = static_cast<SelectedLayout>(currentLayout);
		if (m_layout == SelectedLayout::M_VERTICAL) {
			std::vector<Surface*>& surfaces = m_uiScene->getChildren<Surface>();
			for (auto& surface : surfaces) {
				surface->setLayout(Layout::VERTICAL);
				m_uiScene->setLayout(Layout::HORIZONTAL);
			}
			m_uiScene->updateLayout();
		}

		if (m_layout == SelectedLayout::M_HORIZONTAL) {
			std::vector<Surface*>& surfaces = m_uiScene->getChildren<Surface>();
			for (auto& surface : surfaces) {
				surface->setLayout(Layout::HORIZONTAL);
				m_uiScene->setLayout(Layout::VERTICAL);
			}
			m_uiScene->updateLayout();
		}

		if (m_layout == SelectedLayout::M_GRID) {
			std::vector<Surface*>& surfaces = m_uiScene->getChildren<Surface>();
			for (auto& surface : surfaces) {
				surface->setLayout(Layout::GRID);
				m_uiScene->setLayout(Layout::MASONRY);
			}
			m_uiScene->updateLayout();
		}
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

void Menu::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		uiShutDown();
		wgpCleanState();
		m_isRunning = false;
	}
}