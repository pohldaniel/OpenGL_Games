#include <engine/Fontrenderer.h>

#include <States/Grass/GrassComp.h>
#include <States/Grass/GrassGeom.h>

#include <States/Stencil/Stencil.h>
#include <States/Shadow/Shadow.h>

#include <States/SSS/Separable.h>
#include <States/SSS/SSSApproximation.h>
#include <States/SSS/SSSGems.h>
#include <States/SSS/SSSOGLP.h>

#include <States/Shell/Shell1.h>
#include <States/Shell/Shell2.h>
#include <States/Shell/Shell3.h>

#include <States/Particle/BlendedParticle.h>
#include <States/Particle/SmoothParticle.h>
#include <States/Particle/SoftParticle.h>
#include <States/Particle/Fire.h>

#include <States/Blobshoot.h>
#include <States/RayMarch.h>
#include <States/OctreeInterface.h>
#include <States/OcclusionQuery.h>
#include <States/AnimationInterface.h>
#include <States/KCCInterface.h>
#include <States/SkinnedArmor.h>
#include <States/SixDegreeOfFreedom.h>

#include <States/Settings.h>
#include <States/Controls.h>
#include <States/Default.h>

#include "Menu.h"
#include "Application.h"
#include "Globals.h"

Menu::Menu(StateMachine& machine) : State(machine, States::MENU) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	m_headline.setCharset(Globals::fontManager.get("upheaval_200"));
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 220), static_cast<float>(Application::Height - 200)));
	m_headline.setOutlineThickness(5.0f);
	m_headline.setText("Menu");
	m_headline.setOffset(5.0f, -7.0f);

	m_buttons = std::initializer_list<std::pair<const std::string, Button>>({
		{ "grass_comp",       Button() },
		{ "grass_geom",       Button() },
		{ "shadow",           Button() },	
		{ "stencil",          Button() },	
		{ "separable",        Button() },
		{ "approximation",    Button() },
		{ "gems",             Button() },
		{ "sss_oglp",         Button() },
		{ "shell_base",       Button() },
		{ "shell_red",        Button() },
		{ "shell_oglp",       Button() },
		{ "raymarch",         Button() },
		{ "smooth_particle",  Button() },
		{ "soft_particle",    Button() },
		{ "blended_particle", Button() },
		{ "fire",             Button() },
		{ "blobshoot",        Button() },
		{ "octree",           Button() },
		{ "occlusion",        Button() },
		{ "animation",        Button() },
		{ "kcc",              Button() },
		{ "skinned",          Button() },
		{ "6dof",             Button() }
	});

	m_buttons.at("grass_comp").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("grass_comp").setPosition(50.0f, 550.0f);
	m_buttons.at("grass_comp").setOutlineThickness(5.0f);
	m_buttons.at("grass_comp").setText("Grass Comp");
	m_buttons.at("grass_comp").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new GrassComp(m_machine));
	});

	m_buttons.at("grass_geom").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("grass_geom").setPosition(50.0f, 450.0f);
	m_buttons.at("grass_geom").setOutlineThickness(5.0f);
	m_buttons.at("grass_geom").setText("Grass Geom");
	m_buttons.at("grass_geom").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new GrassGeom(m_machine));
	});

	m_buttons.at("shadow").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("shadow").setPosition(50.0f, 350.0f);
	m_buttons.at("shadow").setOutlineThickness(5.0f);
	m_buttons.at("shadow").setText("Shadow");
	m_buttons.at("shadow").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Shadow(m_machine));
	});

	m_buttons.at("stencil").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("stencil").setPosition(50.0f, 250.0f);
	m_buttons.at("stencil").setOutlineThickness(5.0f);
	m_buttons.at("stencil").setText("Stencil");
	m_buttons.at("stencil").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Stencil(m_machine));
	});

	m_buttons.at("blobshoot").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("blobshoot").setPosition(50.0f, 150.0f);
	m_buttons.at("blobshoot").setOutlineThickness(5.0f);
	m_buttons.at("blobshoot").setText("Blob Shoot");
	m_buttons.at("blobshoot").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new BlobShoot(m_machine));
	});

	m_buttons.at("kcc").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("kcc").setPosition(50.0f, 50.0f);
	m_buttons.at("kcc").setOutlineThickness(5.0f);
	m_buttons.at("kcc").setText("KCC");
	m_buttons.at("kcc").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new KCCInterface(m_machine));
	});

	m_buttons.at("separable").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("separable").setPosition(350.0f, 550.0f);
	m_buttons.at("separable").setOutlineThickness(5.0f);
	m_buttons.at("separable").setText("SSS Separable");
	m_buttons.at("separable").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Separable(m_machine));
	});

	m_buttons.at("approximation").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("approximation").setPosition(350.0f, 450.0f);
	m_buttons.at("approximation").setOutlineThickness(5.0f);
	m_buttons.at("approximation").setText("SSS Approximation");
	m_buttons.at("approximation").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new SSSApproximation(m_machine));
	});

	m_buttons.at("gems").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("gems").setPosition(350.0f, 350.0f);
	m_buttons.at("gems").setOutlineThickness(5.0f);
	m_buttons.at("gems").setText("SSS Gems");
	m_buttons.at("gems").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new SSSGems(m_machine));
	});

	m_buttons.at("sss_oglp").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("sss_oglp").setPosition(350.0f, 250.0f);
	m_buttons.at("sss_oglp").setOutlineThickness(5.0f);
	m_buttons.at("sss_oglp").setText("SSS OGLP");
	m_buttons.at("sss_oglp").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new SSSOGLP(m_machine));
	});

	m_buttons.at("octree").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("octree").setPosition(350.0f, 150.0f);
	m_buttons.at("octree").setOutlineThickness(5.0f);
	m_buttons.at("octree").setText("Octree");
	m_buttons.at("octree").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new OctreeInterface(m_machine));
	});

	m_buttons.at("skinned").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("skinned").setPosition(350.0f, 50.0f);
	m_buttons.at("skinned").setOutlineThickness(5.0f);
	m_buttons.at("skinned").setText("Skinned Armor");
	m_buttons.at("skinned").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new SkinnedArmor(m_machine));
	});

	m_buttons.at("shell_base").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("shell_base").setPosition(750.0f, 550.0f);
	m_buttons.at("shell_base").setOutlineThickness(5.0f);
	m_buttons.at("shell_base").setText("Shell Base");
	m_buttons.at("shell_base").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Shell3(m_machine));
	});

	m_buttons.at("shell_red").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("shell_red").setPosition(750.0f, 450.0f);
	m_buttons.at("shell_red").setOutlineThickness(5.0f);
	m_buttons.at("shell_red").setText("Shell Red");
	m_buttons.at("shell_red").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Shell1(m_machine));
	});

	m_buttons.at("shell_oglp").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("shell_oglp").setPosition(750.0f, 350.0f);
	m_buttons.at("shell_oglp").setOutlineThickness(5.0f);
	m_buttons.at("shell_oglp").setText("Shell Oglp");
	m_buttons.at("shell_oglp").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Shell2(m_machine));
	});

	m_buttons.at("raymarch").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("raymarch").setPosition(750.0f, 250.0f);
	m_buttons.at("raymarch").setOutlineThickness(5.0f);
	m_buttons.at("raymarch").setText("Ray March");
	m_buttons.at("raymarch").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new RayMarch(m_machine));
	});

	m_buttons.at("occlusion").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("occlusion").setPosition(750.0f, 150.0f);
	m_buttons.at("occlusion").setOutlineThickness(5.0f);
	m_buttons.at("occlusion").setText("Occlusion Query");
	m_buttons.at("occlusion").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new OcclusionQuery(m_machine));
	});

	m_buttons.at("6dof").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("6dof").setPosition(750.0f, 50.0f);
	m_buttons.at("6dof").setOutlineThickness(5.0f);
	m_buttons.at("6dof").setText("6 DoF");
	m_buttons.at("6dof").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new SixDegreeOfFreedom(m_machine));
	});

	m_buttons.at("smooth_particle").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("smooth_particle").setPosition(1130.0f, 550.0f);
	m_buttons.at("smooth_particle").setOutlineThickness(5.0f);
	m_buttons.at("smooth_particle").setText("Smooth Particle");
	m_buttons.at("smooth_particle").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new SmoothParticle(m_machine));
	});

	m_buttons.at("soft_particle").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("soft_particle").setPosition(1130.0f, 450.0f);
	m_buttons.at("soft_particle").setOutlineThickness(5.0f);
	m_buttons.at("soft_particle").setText("Soft Particle");
	m_buttons.at("soft_particle").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new SoftParticle(m_machine));
	});

	m_buttons.at("blended_particle").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("blended_particle").setPosition(1130.0f, 350.0f);
	m_buttons.at("blended_particle").setOutlineThickness(5.0f);
	m_buttons.at("blended_particle").setText("Blended Particle");
	m_buttons.at("blended_particle").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new BlendedParticle(m_machine));
	});

	m_buttons.at("fire").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("fire").setPosition(1130.0f, 250.0f);
	m_buttons.at("fire").setOutlineThickness(5.0f);
	m_buttons.at("fire").setText("Fire");
	m_buttons.at("fire").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Fire(m_machine));
	});

	m_buttons.at("animation").setCharset(Globals::fontManager.get("upheaval_50"));
	m_buttons.at("animation").setPosition(1130.0f, 150.0f);
	m_buttons.at("animation").setOutlineThickness(5.0f);
	m_buttons.at("animation").setText("Animation");
	m_buttons.at("animation").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new AnimationInterface(m_machine));
	});

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

Menu::~Menu() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void Menu::fixedUpdate() {}

void Menu::update() {}

void Menu::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_headline.draw();

	for (auto&& b : m_buttons)
		b.second.draw();

}

void Menu::OnMouseMotion(Event::MouseMoveEvent& event) {
	for (auto&& b : m_buttons)
		b.second.processInput(event.x, Application::Height - event.y);
}

void Menu::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	for (auto&& b : m_buttons)
		b.second.processInput(event.x, Application::Height - event.y, event.button);
}

void Menu::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void Menu::resize(int deltaW, int deltaH) {
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 220), static_cast<float>(Application::Height - 200)));
}