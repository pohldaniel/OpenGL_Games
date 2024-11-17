#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <engine/Fontrenderer.h>
#include <States/Menu.h>
#include <States/MonsterHunter.h>

#include "Battle.h"
#include "Zone.h"
#include "Application.h"
#include "Globals.h"

std::random_device Battle::RandomDevice;
std::mt19937 Battle::MersenTwist(RandomDevice());
Sprite Battle::SurfaceBar;
TextureRect Battle::BackgroundRect = {0.0f, 0.0f, 1.0f, 1.0f, 1280.0f, 720.0f, 0u};

Battle::Battle(StateMachine& machine) : State(machine, States::BATTLE), Icon(BackgroundRect),
m_mapHeight(0.0f), 
m_viewWidth(1280.0f), 
m_viewHeight(720.0f), 
m_drawGeneralUi(false),
m_drawAtacksUi(false),
m_drawSwitchUi(false),
m_drawTargetUI(false),
m_currentSelectedMonster(-1),
m_currentSelectedOption(0),
m_currentMax(4),
m_currentOffset(0),
m_visibleItems(4),
m_cutOff(0),
m_elapsedTime(0.0f),
m_currentFrame(0),
m_frameCount(4),
m_playAbility(false),
m_abilityOffset(0),
m_abilityPosX(0.0f),
m_abilityPosY(0.0f),
m_removeDefeteadMonster(false),
m_exit(false),
m_catchMonster(false),
m_canSwitch(true),
m_alpha(0.0f),
m_fade(m_alpha),
m_canCatch(true)
{

	m_viewWidth = 1280.0f;
	m_viewHeight = 720.0f;

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, m_viewWidth, 0.0f, m_viewHeight, -1.0f, 1.0f);
	m_camera.setRotationSpeed(0.1f);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	positions.push_back({ 360.0f - 96.0f , m_viewHeight - 260.0f - 96.0f });
	positions.push_back({ 190.0f - 96.0f , m_viewHeight - 400.0f - 96.0f });
	positions.push_back({ 410.0f - 96.0f , m_viewHeight - 520.0f - 96.0f });

	positions.push_back({ 900.0f - 96.0f  , m_viewHeight - 260.0f - 96.0f });
	positions.push_back({ 1110.0f - 96.0f , m_viewHeight - 390.0f - 96.0f });
	positions.push_back({ 900.0f - 96.0f  , m_viewHeight - 550.0f - 96.0f });

	centers.push_back({ 360.0f, m_viewHeight - 260.0f });
	centers.push_back({ 190.0f, m_viewHeight - 400.0f });
	centers.push_back({ 410.0f, m_viewHeight - 520.0f });

	centers.push_back({ 900.0f , m_viewHeight - 260.0f });
	centers.push_back({ 1110.0f, m_viewHeight - 390.0f });
	centers.push_back({ 900.0f , m_viewHeight - 550.0f });
	m_cells.reserve(6);
	//setOpponentMonsters();

	TextureAtlasCreator::Get().init(256u, 32u);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/sword.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/shield.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/arrows.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/hand.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/sword_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/shield_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/arrows_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/hand_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/sword_gray.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/shield_gray.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/arrows_gray.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/hand_gray.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/sword_gray_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/shield_gray_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/arrows_gray_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/hand_gray_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileSetGpu();
	//Spritesheet::Safe("battle_icon", TileSetManager::Get().getTileSet("battle_icon").getAtlas());
	m_atlasBattleIcon = TileSetManager::Get().getTileSet("battle_icon").getAtlas();

	TextureAtlasCreator::Get().init(768u, 1152u);
	TileSetManager::Get().getTileSet("attacks").loadTileSetCpu("res/tmx/graphics/attacks/explosion.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("attacks").loadTileSetCpu("res/tmx/graphics/attacks/fire.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("attacks").loadTileSetCpu("res/tmx/graphics/attacks/green.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("attacks").loadTileSetCpu("res/tmx/graphics/attacks/ice.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("attacks").loadTileSetCpu("res/tmx/graphics/attacks/scratch.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("attacks").loadTileSetCpu("res/tmx/graphics/attacks/splash.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("attacks").loadTileSetGpu();
	//Spritesheet::Safe("attacks", TileSetManager::Get().getTileSet("attacks").getAtlas());
	m_atlasAbilities = TileSetManager::Get().getTileSet("attacks").getAtlas();

	TextureAtlasCreator::Get().init(1280u, 720u);
	TileSetManager::Get().getTileSet("backgrounds").loadTileCpu("res/tmx/graphics/backgrounds/forest.png", false, true, false);
	TileSetManager::Get().getTileSet("backgrounds").loadTileCpu("res/tmx/graphics/backgrounds/ice.png", false, true, false);
	TileSetManager::Get().getTileSet("backgrounds").loadTileCpu("res/tmx/graphics/backgrounds/sand.png", false, true, false);
	TileSetManager::Get().getTileSet("backgrounds").loadTileSetGpu();
	//Spritesheet::Safe("backgrounds", TileSetManager::Get().getTileSet("backgrounds").getAtlas());
	m_backgrounds = TileSetManager::Get().getTileSet("backgrounds").getAtlas();
	
	initUI();

	m_battleChoices.push_back({ {30.0f, 60.0f}, 0u});
	m_battleChoices.push_back({ {40.0f, 20.0f}, 1u});
	m_battleChoices.push_back({ {40.0f, -20.0f}, 2u});
	m_battleChoices.push_back({ {30.0f, -60.0f}, 3u});

	m_opponentTimer.setOnTimerEnd(std::bind(&Battle::opponentAttack, this));
	m_exitTimer.setOnTimerEnd(std::bind(&Battle::exit, this));

	m_mainRenderTarget.create(Application::Width, Application::Height);
	m_mainRenderTarget.attachTexture2D(AttachmentTex::RGBA);
	m_mainRenderTarget.attachTexture2D(AttachmentTex::DEPTH24);
	
	m_biomeBackground = "forest";

	m_fade.setTransitionSpeed(2.353f);
	m_fade.fadeIn();

}

Battle::~Battle() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	MonsterIndex::Monsters.shrink_to_fit();
}

void Battle::fixedUpdate() {

}

void Battle::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		direction += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		direction += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		direction += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		exit();
	}

	if (keyboard.keyPressed(Keyboard::KEY_P)) {
		std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));
		std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setHighlight), std::placeholders::_1, false));

		m_drawGeneralUi = false;
		m_currentSelectedMonster = -1;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xDelta();
		dy = mouse.yDelta();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);
		}

		if (move) {
			m_camera.move(direction * m_dt);
		}
	}

	if (m_playAbility) {
		m_elapsedTime += 6.0f * m_dt;
		m_currentFrame = static_cast <int>(std::floor(m_elapsedTime));
		if (m_currentFrame > m_frameCount - 1) {
			m_currentFrame = 0;
			m_abilityOffset = 0;
			m_elapsedTime = 0.0f;
			m_playAbility = false;
			onAbilityEnd();
		}
	}

	m_opponentTimer.update(m_dt);
	m_exitTimer.update(m_dt);
	m_fade.update(m_dt);

	for (auto& monster : m_monsters) {
		monster->update(m_dt);
	}

	if (m_exit)
		return;

	for (auto& monster = m_monsters.begin(); monster != m_monsters.end(); ++monster) {
		if ((*monster)->getInitiative() >= 100.0f && !(*monster)->getPause()) {

			std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setHighlight), std::placeholders::_1, false));
			(*monster)->setInitiative(0.0f);
			(*monster)->setHighlight(true, 300);
			(*monster)->setDefending(false);
			std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::pause));

			if ((*monster)->getCell().flipped) {
				m_drawGeneralUi = true;
			}else {
				if (std::count_if(m_monsters.begin(), m_monsters.end(), [](Monster* monster) { return monster->getCell().flipped && monster->getHealth() > 0.0f; }) != 0) {
					m_opponentTimer.start(600u, false);
				}
			}

			m_currentSelectedMonster = std::distance(m_monsters.begin(), monster);
			m_currentMax = 4;

		}
	}

	if (m_drawGeneralUi || m_drawAtacksUi || m_drawSwitchUi || m_drawTargetUI) {
		processInput();
		if (m_drawTargetUI) {
			std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setHighlight), std::placeholders::_1, false));
			//std::for_each(m_monsters.begin(), m_monsters.end(), MonsterFunctor(&Monster::setHighlight, true, 500u));	
			m_monsters[m_currentSelectedOption + m_cutOff]->setHighlight(true);
		}
	}
	removeDefeteadMonster();
	
}

void Battle::render() {
	m_mainRenderTarget.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Icon::draw();

	{
		std::list<std::shared_ptr<NodeUI>>::iterator  it;
		std::shared_ptr<Monster> currentWidget;
		for (it = getChildren().begin(); it != getChildren().end(); ++it) {
			currentWidget = std::static_pointer_cast<Monster>(*it);
			currentWidget->draw();
		}
	}

	if (m_playAbility) {		
		drawAbilityAnimation(m_abilityPosX, m_abilityPosY);
	}
	
	if(m_drawGeneralUi)
		drawGeneral();

	if (m_drawAtacksUi)
		drawAttacks();

	if (m_drawSwitchUi)
		drawSwitch();

	if (m_drawUi)
		renderUi();

	m_mainRenderTarget.unbind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_mainRenderTarget.bindColorTexture(0u, 0u);
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, m_alpha));
	Globals::shapeManager.get("quad").drawRaw();
	shader->unuse();
}

void Battle::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Battle::OnMouseButtonDown(Event::MouseButtonEvent& event) {

}

void Battle::OnMouseButtonUp(Event::MouseButtonEvent& event) {

}

void Battle::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Battle::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
}

void Battle::OnKeyUp(Event::KeyboardEvent& event) {

}

void Battle::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, m_viewWidth, 0.0f, m_viewHeight, -1.0f, 1.0f);

	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("font");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("dialog");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();
}

void Battle::renderUi() {
	ImGui_ImplOpenGL3_NewFrame();
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
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Battle::setMapHeight(float mapHeight) {
	m_mapHeight = mapHeight;
}

void Battle::setViewHeight(float viewHeight) {
	m_viewHeight = viewHeight;
}

void Battle::drawGeneral() {
	Spritesheet::Bind(m_atlasBattleIcon);
	Monster* currentMonster = m_monsters[m_currentSelectedMonster];
	const Cell& cell = currentMonster->getCell();

	for (size_t i = 0; i < m_battleChoices.size(); i++) {
		const BattleChoice& battleChoice = m_battleChoices[i];
		if (i == m_currentSelectedOption) {
			const TextureRect& rect = TileSetManager::Get().getTileSet("battle_icon").getTextureRects()[battleChoice.graphics + 4 + ((i == 0) && !currentMonster->getCanAttack()) * 8 + ((i == 2) && !m_canSwitch) * 8 + ((i == 3) && !m_canCatch) * 8];
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX + cell.width + battleChoice.pos[0] - 0.5f * rect.width, cell.posY + 0.5f * cell.height + battleChoice.pos[1] - 0.5f * rect.height, rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
		}else {
			const TextureRect& rect = TileSetManager::Get().getTileSet("battle_icon").getTextureRects()[battleChoice.graphics + 8];
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX + cell.width + battleChoice.pos[0] - 0.5f * rect.width, cell.posY + 0.5f * cell.height + battleChoice.pos[1] - 0.5f * rect.height, rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);

		}
	
	}

	Batchrenderer::Get().drawBuffer();
}

void Battle::drawAttacks() {
	Monster* currentMonster = m_monsters[m_currentSelectedMonster];
	const Cell& cell = currentMonster->getCell();

	float width = 150.0f;
	float height = 200.0f;
	int limiter = std::min(m_visibleItems, m_currentMax);

	float itemHeight = height / static_cast<float>(std::max(limiter, 4));
	float lineHeight = Globals::fontManager.get("dialog").lineHeight * 0.045f;

	height = limiter <= 1  ? 50.0f : limiter == 2  ? 100.0f : limiter == 3 ? 150.0f : 200.0f;

	auto shader = Globals::shaderManager.getAssetPointer("list");
	shader->use();
	shader->loadVector("u_dimensions", Vector2f(width, height));
	shader->loadFloat("u_radius", 5.0f);
	shader->loadUnsignedInt("u_edge", Edge::ALL);

	m_surface.setShader(shader);
	m_surface.setPosition(cell.posX + cell.width + 20.0f, cell.posY + 0.5f * cell.height - 0.5f * height);
	m_surface.setScale(width, height);
	m_surface.draw();
	
	int index = 0;	
	for (auto& ability = m_abilitiesFiltered.begin() + m_currentOffset; ability != m_abilitiesFiltered.begin() + m_currentOffset + std::min(m_visibleItems, m_currentMax - m_currentOffset); ++ability){
		
		if (index == m_currentSelectedOption - m_currentOffset) {

			if(index == 0)
				shader->loadUnsignedInt("u_edge", Edge::TOP);
			else if (index == m_visibleItems - 1)
				shader->loadUnsignedInt("u_edge", Edge::BOTTOM);
			else 
				shader->loadUnsignedInt("u_edge", Edge::EDGE_NONE);
			
			shader->loadVector("u_dimensions", Vector2f(width, itemHeight));
			m_surface.setPosition(cell.posX + cell.width + 20.0f, cell.posY + 0.5f * cell.height - 0.5f * height + height - (index + 1) * itemHeight);
			m_surface.setScale(width, itemHeight);
			m_surface.draw(Vector4f(0.78431f, 0.78431f, 0.78431f, 1.0f));
		}

		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"),
			cell.posX + cell.width + 20.0f + 0.5f * width - 0.5f * Globals::fontManager.get("dialog").getWidth((*ability).first) * 0.045f,
			cell.posY + 0.5f * cell.height - 0.5f * height + height - (index + 1) * itemHeight + 0.5f * itemHeight - 0.5f * lineHeight,
			(*ability).first,
			(index == m_currentSelectedOption - m_currentOffset) ? 
			MonsterIndex::_AttackData[(*ability).first].element == "normal" ? Vector4f(0.0f, 0.0f, 0.0f, 1.0f) : MonsterIndex::ColorMap[MonsterIndex::_AttackData[(*ability).first].element] 
			: Vector4f(0.78431f, 0.78431f, 0.78431f, 1.0f), 0.045f);
		index++;
	}
	Globals::fontManager.get("dialog").bind();
	Fontrenderer::Get().drawBuffer();
}

void Battle::drawSwitch() {
	Monster* currentMonster = m_monsters[m_currentSelectedMonster];
	const Cell& cell = currentMonster->getCell();
	const std::vector<TextureRect>& iconRects = TileSetManager::Get().getTileSet("monster_icon").getTextureRects();

	float width = 300.0f;
	float height = 320.0f;
	int limiter = std::min(m_visibleItems, m_currentMax);

	float itemHeight = height / static_cast<float>(std::max(limiter, 4));
	float lineHeight = Globals::fontManager.get("dialog").lineHeight * 0.045f;

	height = limiter <= 1 ? 80.0f : limiter == 2 ? 160.0f : limiter == 3 ? 240.0f : 320.0f;

	auto shader = Globals::shaderManager.getAssetPointer("list");
	shader->use();
	shader->loadVector("u_dimensions", Vector2f(width, height));
	shader->loadFloat("u_radius", 5.0f);
	shader->loadUnsignedInt("u_edge", Edge::ALL);

	m_surface.setShader(shader);
	m_surface.setPosition(cell.posX + cell.width + 20.0f, cell.posY + 0.5f * cell.height - 0.5f * height);
	m_surface.setScale(width, height);
	m_surface.draw();

	if (m_currentSelectedOption == m_currentOffset)
		shader->loadUnsignedInt("u_edge", Edge::TOP);
	else if (m_currentSelectedOption == m_currentOffset + std::min(m_visibleItems, m_currentMax - m_currentOffset) - 1)
		shader->loadUnsignedInt("u_edge", Edge::BOTTOM);
	else
		shader->loadUnsignedInt("u_edge", Edge::EDGE_NONE);

	shader->loadVector("u_dimensions", Vector2f(width, itemHeight));
	m_surface.setPosition(cell.posX + cell.width + 20.0f, cell.posY + 0.5f * cell.height - 0.5f * height + height - (m_currentSelectedOption - m_currentOffset + 1) * itemHeight);
	m_surface.setScale(width, itemHeight);
	m_surface.draw(Vector4f(0.78431f, 0.78431f, 0.78431f, 1.0f));

	for (size_t index = m_currentOffset; index < m_currentOffset + std::min(m_visibleItems, m_currentMax - m_currentOffset); index++) {
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"),
			cell.posX + cell.width + 20.0f + 90.0f,
			cell.posY + 0.5f * cell.height - 0.5f * height + height - (index - m_currentOffset + 1) * itemHeight + 0.5f * itemHeight - 0.5f * lineHeight,
			m_filteredMonsters[index].get().name + " " + "(" + std::to_string(m_filteredMonsters[index].get().level) + ")",
			index == m_currentSelectedOption ? Vector4f(0.94117f, 0.19215f, 0.19215f, 1.0f) : Vector4f(0.0f, 0.0f, 0.0f, 1.0f),
			0.045f);
	}

	Globals::fontManager.get("dialog").bind();
	Fontrenderer::Get().drawBuffer();

	TileSetManager::Get().getTileSet("monster_icon").bind();
	m_surface.resetShader();
	for (size_t index = m_currentOffset; index < m_currentOffset + std::min(m_visibleItems, m_currentMax - m_currentOffset); index++) {
		const TextureRect& iconRect = iconRects[MonsterIndex::MonsterData[m_filteredMonsters[index].get().name].graphic];
		float iconPosX = cell.posX + cell.width + 20.0f + 10.0f;
		float iconPosY = cell.posY + 0.5f * cell.height - 0.5f * height + height - (index - m_currentOffset + 1) * itemHeight + 0.5f * itemHeight - iconRect.height * 0.5f;

		
		m_surface.setPosition(iconPosX, iconPosY);
		m_surface.setScale(iconRect.width, iconRect.height);
		m_surface.draw(iconRect, Vector4f(1.0f, 1.0f, 1.0f, 1.0f));


		float barPosX = cell.posX + cell.width + 20.0f + 90.0f;
		float barPosY = cell.posY + 0.5f * cell.height - 0.5f * height + height - (index - m_currentOffset + 1) * itemHeight + 0.5f * itemHeight - 0.5f * lineHeight;

		Battle::DrawBar({ barPosX, barPosY - 9.0f,  100.0f, 5.0f }, m_filteredMonsters[index].get().health, static_cast<float>(m_filteredMonsters[index].get().level * MonsterIndex::MonsterData[m_filteredMonsters[index].get().name].maxHealth), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.94117f, 0.19215f, 0.19215f, 1.0f), 0.0f);
		Battle::DrawBar({ barPosX, barPosY - 16.0f, 100.0f, 5.0f }, m_filteredMonsters[index].get().energy, static_cast<float>(m_filteredMonsters[index].get().level * MonsterIndex::MonsterData[m_filteredMonsters[index].get().name].maxEnergy), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.4f, 0.84313f, 0.93333f, 1.0f), 0.0f);
	}
}

void Battle::processInput() {
	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyPressed(Keyboard::KEY_UP)) {
		m_currentSelectedOption--;

		if (m_currentOffset > 0 && m_currentSelectedOption - m_currentOffset < 0) {
			m_currentOffset--;
		}
		
		if (m_currentSelectedOption < 0) {
			m_currentOffset = std::max(m_currentMax - m_visibleItems, 0);
			m_currentSelectedOption = m_currentMax - 1;
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_DOWN)) {
		m_currentSelectedOption++;

		if (m_currentSelectedOption - m_currentOffset - (m_visibleItems - 1) > 0) {
			m_currentOffset++;
		}
	
		if (m_currentSelectedOption > m_currentMax - 1) {
			m_currentSelectedOption = 0;
			m_currentOffset = 0;
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {

		if (m_drawTargetUI) {
			if(m_catchMonster){
				m_catchMonster = false;
				m_drawTargetUI = false;
				m_monsters[m_currentSelectedMonster]->setHighlight(false);
				std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));

				Monster* currentMonster = m_monsters[m_currentSelectedOption + m_cutOff];
				if (currentMonster->getHealth() >= currentMonster->getMaxHealth() * 0.1f) {
					currentMonster->showMissing();
					m_currentSelectedMonster = -1;
					m_currentSelectedOption = 0;
					m_cutOff = 0;
					return;
				}
								
				MonsterIndex::Monsters.push_back({ currentMonster->getName(), currentMonster->getLevel(), currentMonster->getHealth(), currentMonster->getEnergy(), currentMonster->getExperience(), false });
				if (m_supplyIndexOpponent < static_cast<int>(m_opponentMonsters.size()) - 1) {
					m_supplyIndexOpponent++;
					m_monsters[m_currentSelectedOption + m_cutOff]->setGraphic(static_cast<int>(MonsterIndex::MonsterData[m_opponentMonsters[m_supplyIndexOpponent].name].graphic * 16u));
					m_monsters[m_currentSelectedOption + m_cutOff]->setMonsterEntry(m_opponentMonsters[m_supplyIndexOpponent]);
					m_monsters[m_currentSelectedOption + m_cutOff]->setInitiative(0.0f);

				}else {
					m_monsters[m_currentSelectedOption + m_cutOff]->eraseSelf();
					m_monsters.erase(m_monsters.begin() + m_currentSelectedOption + m_cutOff);
				}

				m_currentSelectedMonster = -1;
				m_currentSelectedOption = 0;
				m_cutOff = 0;

				if (std::count_if(m_monsters.begin(), m_monsters.end(), [](Monster* monster) { return !monster->getCell().flipped && monster->getHealth() > 0.0f; }) == 0) {
					m_exitTimer.start(1000u, false, true);
					m_exit = true;
					std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::pause));
				}
				m_canSwitch = m_supplyIndexPlayer < static_cast<int>(MonsterIndex::Monsters.size()) - 1;

			}else{
				m_playAbility = true;
				m_drawTargetUI = false;
				m_monsters[m_currentSelectedMonster]->reduceEnergy(MonsterIndex::_AttackData[m_currentAbility.first]);
				m_monsters[m_currentSelectedMonster]->playAttackAnimation();
				m_monsters[m_currentSelectedMonster]->canAttack();

				float amount = m_monsters[m_currentSelectedMonster]->getBaseDamage(m_currentAbility.first);
				m_monsters[m_currentSelectedOption + m_cutOff]->applyAttack(amount, MonsterIndex::_AttackData[m_currentAbility.first]);

				m_abilityPosX = m_monsters[m_currentSelectedOption + m_cutOff]->getCell().centerX;
				m_abilityPosY = m_monsters[m_currentSelectedOption + m_cutOff]->getCell().centerY;
				if (std::count_if(m_monsters.begin(), m_monsters.end(), [](Monster* monster) { return !monster->getCell().flipped && monster->getHealth() > 0.0f; }) == 0) {
					std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setCanAttack), std::placeholders::_1, false));
				}
			}
		}

		if (m_drawSwitchUi) {		
			MonsterEntry& monsterEntry = m_filteredMonsters[m_currentOffset + m_currentSelectedOption];
			MonsterEntry& currentMonsterEntry = m_monsters[m_currentSelectedMonster]->getMonsterEntry();
			std::swap(monsterEntry, currentMonsterEntry);
			
			m_monsters[m_currentSelectedMonster]->setGraphic(static_cast<int>(MonsterIndex::MonsterData[currentMonsterEntry.name].graphic * 16u));
			m_monsters[m_currentSelectedMonster]->calculateStates(currentMonsterEntry);
			m_monsters[m_currentSelectedMonster]->setInitiative(0.0f);

			std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));

			m_drawSwitchUi = false;
			m_visibleItems = 4;
			m_currentSelectedOption = 0;
			m_currentOffset = 0;
			m_currentMax = 4;
			m_currentSelectedMonster = -1;		
		}

		if (m_drawAtacksUi) {
			m_currentAbility = *(m_abilitiesFiltered.begin() + m_currentSelectedOption);
			m_currentTarget = MonsterIndex::_AttackData[m_currentAbility.first].target;
			m_drawAtacksUi = false;
			m_drawTargetUI = true;

			int playerMonsterCount = std::count_if(m_monsters.begin(), m_monsters.end(), [](Monster* monster) { return monster->getCell().flipped; });

			m_currentMax = m_currentTarget == "player" ? playerMonsterCount : static_cast<int>(m_monsters.size()) - playerMonsterCount;
			m_visibleItems = m_currentMax;
			
			m_currentSelectedOption = 0;
			m_currentOffset = 0;
			m_cutOff = m_currentTarget == "player" ? 0 : playerMonsterCount;
			Monster* currentMonster = m_monsters[m_currentSelectedMonster];
			if (m_currentTarget == "player")
				currentMonster->setHighlight(false);

			m_abilityOffset = MonsterIndex::_AttackData[m_currentAbility.first].graphic;
		}

		if (m_drawGeneralUi) {
			if (m_currentSelectedOption == 0) {
				m_drawGeneralUi = false;
				m_drawAtacksUi = true;
				m_visibleItems = 4;
				Monster* currentMonster = m_monsters[m_currentSelectedMonster];
				const tsl::ordered_map<std::string, unsigned int>& abilities = MonsterIndex::MonsterData[m_monsters[m_currentSelectedMonster]->getName()].abilities;
				m_abilitiesFiltered.clear();
				std::copy_if(abilities.begin(), abilities.end(), std::inserter(m_abilitiesFiltered, m_abilitiesFiltered.end()), [currentMonster = currentMonster](std::pair<std::string, unsigned int> ability) {
					return  MonsterIndex::_AttackData[ability.first].cost <= currentMonster->getEnergy();
				});

				m_currentMax = std::count_if(m_abilitiesFiltered.begin(), m_abilitiesFiltered.end(), [&currentMonster = currentMonster](const std::pair<std::string, unsigned int>& ability) { return ability.second <= currentMonster->getLevel(); });
				int opponentMonsterCount = std::count_if(m_monsters.begin(), m_monsters.end(), [](Monster* monster) { return !monster->getCell().flipped && monster->getHealth() > 0.0f; });
				m_currentSelectedOption = 0;
				m_currentOffset = 0;
				
				if (m_currentMax == 0 || opponentMonsterCount == 0) {
					m_drawAtacksUi = false;
					m_drawGeneralUi = true;
					m_visibleItems = 4;
					m_currentMax = 4;				
				}

			}else if (m_currentSelectedOption == 1) {
				m_monsters[m_currentSelectedMonster]->setDefending(true);
				m_drawGeneralUi = false;
				m_visibleItems = 4;
				m_currentSelectedOption = 0;
				m_currentOffset = 0;
				m_currentMax = 4;
				m_currentSelectedMonster = -1;
				std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));

			}else if (m_currentSelectedOption == 2) {
				m_drawGeneralUi = false;
				m_drawSwitchUi = true;			
				m_visibleItems = 4;
				const Monster* currentMonster = m_monsters[m_currentSelectedMonster];
				m_filteredMonsters.clear();
				
				for (size_t index = 0; index < MonsterIndex::Monsters.size(); index++) {
					if (MonsterIndex::Monsters[index].health > 0 && index > m_supplyIndexPlayer)
						m_filteredMonsters.push_back(MonsterIndex::Monsters[index]);
				}

				m_currentMax = static_cast<int>(m_filteredMonsters.size());
				m_currentSelectedOption = 0;
				m_currentOffset = 0;

				if (!m_canSwitch) {
					m_drawSwitchUi = false;
					m_drawGeneralUi = true;
					m_currentMax = 4;
					m_currentSelectedOption = 2;
				}

			}else if (m_currentSelectedOption == 3) {
				m_drawGeneralUi = false;
				m_drawTargetUI = true;

				int playerMonsterCount = std::count_if(m_monsters.begin(), m_monsters.end(), [](const Monster* monster) { return monster->getCell().flipped; });

				m_currentMax = m_currentTarget == "player" ? playerMonsterCount : static_cast<int>(m_monsters.size()) - playerMonsterCount;
				m_visibleItems = m_currentMax;

				m_currentSelectedOption = 0;
				m_currentOffset = 0;
				m_cutOff = playerMonsterCount;
				m_catchMonster = true;

				if (!m_canCatch) {
					m_catchMonster = false;
					m_drawGeneralUi = true;
					m_drawTargetUI = false;
					m_currentMax = 4;
					m_visibleItems = 4;
					m_currentSelectedOption = 3;
					m_cutOff = 0;
				}
			}
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_ESCAPE)) {
		if (m_drawGeneralUi) {
			m_monsters[m_currentSelectedMonster]->setDefending(true);
			m_drawGeneralUi = false;
			m_visibleItems = 4;
			m_currentSelectedOption = 0;
			m_currentOffset = 0;
			m_currentMax = 4;
			m_currentSelectedMonster = -1;
			std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));		
		}

		if (m_drawAtacksUi || m_drawSwitchUi || m_drawTargetUI) {
			if (m_drawTargetUI) {
				std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setHighlight), std::placeholders::_1, false));
				m_monsters[m_currentSelectedMonster]->setHighlight(true);
				m_currentSelectedOption = 0;
			}

			if (m_drawAtacksUi)
				m_currentSelectedOption = 0;

			if(m_drawSwitchUi)
				m_currentSelectedOption = 2;

			m_drawAtacksUi = false;
			m_drawSwitchUi = false;
			m_drawTargetUI = false;
			m_catchMonster = false;
			m_drawGeneralUi = true;
			m_currentMax = 4;
			
			m_currentOffset = 0;			
		}
	}
}

void Battle::drawAbilityAnimation(float posX, float posY) {
	const TextureRect& rect = TileSetManager::Get().getTileSet("attacks").getTextureRects()[m_abilityOffset + m_currentFrame];
	Batchrenderer::Get().addQuadAA(Vector4f(posX - 0.5f * rect.width, posY - 0.5f * rect.height, rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
	Spritesheet::Bind(m_atlasAbilities);
	Batchrenderer::Get().drawBuffer();
}

void Battle::onAbilityEnd() {
	m_drawGeneralUi = false;
	m_drawAtacksUi = false;
	m_drawTargetUI = false;
	m_drawSwitchUi = false;

	std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));
	std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setHighlight), std::placeholders::_1, false));

	m_currentSelectedMonster = -1;
	m_currentSelectedOption = 0;
	m_currentOffset = 0;

	if (std::count_if(m_monsters.begin(), m_monsters.end(), [](const Monster* monster) { return !monster->getCell().flipped && monster->getHealth() > 0.0f; }) == 0 ||
		std::count_if(m_monsters.begin(), m_monsters.end(), [](const Monster* monster) { return monster->getCell().flipped && monster->getHealth() > 0.0f; }) == 0) {
		m_exitTimer.start(1000u, false, true);
		m_exit = true;
		std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::pause));
	}
}

void Battle::removeDefeteadMonster() {

	for (size_t index = 0; index < m_monsters.size(); index++) {

		if (m_monsters[index]->getHealth() <= 0.0f) {
			if (!m_monsters[index]->getCell().flipped && !m_monsters[index]->getKilled()) {
				float currentExperience = static_cast<float>(m_monsters[index]->getLevel() * 100u) / static_cast<float>(std::count_if(m_monsters.begin(), m_monsters.end(), [](const Monster* monster) { return monster->getCell().flipped && monster->getHealth() > 0.0f; }));
				
				std::for_each(m_monsters.begin(),
					m_monsters.end(),
					[&](Monster* monster) { if (monster->getCell().flipped)
						monster->updateExperience(currentExperience);
					});
			}
			m_monsters[index]->startDelayedKill();
		}
		if (m_monsters[index]->getDelayedKill()) {
			
			m_monsters[index]->setDelayedKill(false);
			
			if (m_supplyIndexPlayer < static_cast<int>(MonsterIndex::Monsters.size()) - 1 && m_monsters[index]->getCell().flipped) {
				m_supplyIndexPlayer++;
					
				m_monsters[index]->setGraphic(static_cast<int>(MonsterIndex::MonsterData[MonsterIndex::Monsters[m_supplyIndexPlayer].name].graphic * 16u));
				m_monsters[index]->setMonsterEntry(MonsterIndex::Monsters[m_supplyIndexPlayer]);
				m_monsters[index]->setInitiative(0.0f);
				m_monsters[index]->setKilled(false);

				m_canSwitch = m_supplyIndexPlayer < static_cast<int>(MonsterIndex::Monsters.size()) - 1;
			}else if(m_supplyIndexOpponent < static_cast<int>(m_opponentMonsters.size()) - 1 && !m_monsters[index]->getCell().flipped) {
				m_supplyIndexOpponent++;
					
				m_monsters[index]->setGraphic(static_cast<int>(MonsterIndex::MonsterData[m_opponentMonsters[m_supplyIndexOpponent].name].graphic * 16u));
				m_monsters[index]->setMonsterEntry(m_opponentMonsters[m_supplyIndexOpponent]);
				m_monsters[index]->setInitiative(0.0f);
				m_monsters[index]->setKilled(false);

			}else {
				if (m_currentSelectedMonster >= index)
					m_currentSelectedMonster--;
				m_monsters[index]->eraseSelf();
				m_monsters.erase(m_monsters.begin() + index);
				
			}
			break;
		}
	}	
}

void Battle::opponentAttack() {

	m_opponentTimer.stop();	
	Monster* currentMonster = m_monsters[m_currentSelectedMonster];
	const tsl::ordered_map<std::string, unsigned int>& abilities = MonsterIndex::MonsterData[currentMonster->getName()].abilities;
	
	tsl::ordered_map<std::string, unsigned int> abilitiesFiltered;
	std::copy_if(abilities.begin(), abilities.end(), std::inserter(abilitiesFiltered, abilitiesFiltered.end()), [currentMonster = currentMonster](std::pair<std::string, unsigned int> ability) {
		return  MonsterIndex::_AttackData[ability.first].cost <= currentMonster->getEnergy();
	});

	int upperBound = std::count_if(abilitiesFiltered.begin(), abilitiesFiltered.end(), [&currentMonster = currentMonster](const std::pair<std::string, unsigned int>& ability) { return ability.second <= currentMonster->getLevel(); });
	
	//Energy empty
	if (upperBound == 0) {
		std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));
		return;
	}
	
	size_t index = 0;
	std::uniform_int_distribution<size_t> dist(0, upperBound - 1);
	index = dist(MersenTwist);
	tsl::ordered_map<std::string, unsigned int>::iterator ability = abilitiesFiltered.begin();
	std::advance(ability, index);
	
	std::string currentTarget = MonsterIndex::_AttackData[ability->first].target;
	int playerMonsterCount = std::count_if(m_monsters.begin(), m_monsters.end(), [](const Monster* monster) { return monster->getCell().flipped; });
	upperBound = currentTarget == "opponent" ? playerMonsterCount : static_cast<int>(m_monsters.size()) - playerMonsterCount;

	std::uniform_int_distribution<size_t> dist2(0, upperBound - 1);
	index = dist2(MersenTwist);
	if (currentTarget == "player")
		index += playerMonsterCount;

	Monster* target = m_monsters[index];
	float amount = currentMonster->getBaseDamage(ability->first);
	target->applyAttack(amount, MonsterIndex::_AttackData[ability->first]);
	m_playAbility = true;
	m_abilityPosX = target->getCell().centerX;
	m_abilityPosY = target->getCell().centerY;
	m_abilityOffset = MonsterIndex::_AttackData[ability->first].graphic;
	currentMonster->reduceEnergy(MonsterIndex::_AttackData[ability->first]);
}

void Battle::exit() {
	m_fade.setOnFadeOut([&m_isRunning = m_isRunning]() {
		m_isRunning = false;
	});
	m_fade.fadeOut();
}

Fade& Battle::getFade() {
	return m_fade;
}

void Battle::setOpponentMonsters() {

	m_opponentMonsters.push_back({ "Atrox", 3u, 3.0f, 200.0f, 0.0f, false });
	m_opponentMonsters.push_back({ "Finiette", 4u, 3.0f, 20.0f, 0.0f, false });
	m_opponentMonsters.push_back({ "Pouch", 5u, 3.0f, 20.0f, 0.0f, false });
	//m_opponentMonsters.push_back({ "Finsta", 2u, 3.0f, 20.0f, 0.0f, false });
	//m_opponentMonsters.push_back({ "Cleaf", 2u, 3.0f, 20.0f, 0.0f, false });
	//m_opponentMonsters.push_back({ "Friolera", 5u, 3.0f, 20.0f, 0.0f, false });

	MonsterIndex::Monsters.reserve(MonsterIndex::Monsters.size() + m_opponentMonsters.size());
	m_supplyIndexPlayer = std::max(std::min(2, static_cast<int>(MonsterIndex::Monsters.size()) - 1), 0);

	Monster* monster;
	for (int i = 0; i < std::min(static_cast<int>(MonsterIndex::Monsters.size()), 3); i++) {
		m_cells.push_back({ positions[i][0], positions[i][1], 192.0f, 192.0f, static_cast<int>(MonsterIndex::MonsterData[MonsterIndex::Monsters[i].name].graphic * 16u), centers[i][0], centers[i][1], true, true });

		monster = addChild<Monster>(m_cells.back(), MonsterIndex::Monsters[i]);
		monster->translateRelative(positions[i][0], positions[i][1]);
		monster->updateWorldTransformation();
		monster->initUI();
		m_monsters.push_back(monster);
	}

	std::for_each(m_opponentMonsters.begin(), m_opponentMonsters.end(), std::mem_fn(&MonsterEntry::resetStats));
	m_supplyIndexOpponent = std::max(std::min(2, static_cast<int>(m_opponentMonsters.size()) - 1), 0);
	for (int i = 0; i < std::min(static_cast<int>(m_opponentMonsters.size()), 3); i++) {
		m_cells.push_back({ positions[i + 3][0], positions[i + 3][1], 192.0f, 192.0f, static_cast<int>(MonsterIndex::MonsterData[m_opponentMonsters[i].name].graphic * 16u), centers[i + 3][0], centers[i + 3][1], true, false });
		monster = addChild<Monster>(m_cells.back(), m_opponentMonsters[i]);
		monster->translateRelative(positions[i + 3][0], positions[i][1]);
		monster->updateWorldTransformation();
		monster->initUI();
		m_monsters.push_back(monster);
	}

	m_canSwitch = std::count_if(MonsterIndex::Monsters.begin() + m_supplyIndexPlayer + 1, MonsterIndex::Monsters.end(), [](const MonsterEntry& monsterEntry) { return monsterEntry.health > 0.0f; }) != 0;
}

void Battle::setOpponentMonsters(const std::vector<MonsterEntry>& monsters, bool canCatch) {

	m_opponentMonsters.assign(monsters.begin(), monsters.end());

	MonsterIndex::Monsters.reserve(MonsterIndex::Monsters.size() + m_opponentMonsters.size());
	m_supplyIndexPlayer = std::max(std::min(2, static_cast<int>(MonsterIndex::Monsters.size()) - 1), 0);

	Monster* monster;
	for (int i = 0; i < std::min(static_cast<int>(MonsterIndex::Monsters.size()), 3); i++) {
		m_cells.push_back({ positions[i][0], positions[i][1], 192.0f, 192.0f, static_cast<int>(MonsterIndex::MonsterData[MonsterIndex::Monsters[i].name].graphic * 16u), centers[i][0], centers[i][1], true, true });

		monster = addChild<Monster>(m_cells.back(), MonsterIndex::Monsters[i]);
		monster->translateRelative(positions[i][0], positions[i][1]);
		monster->updateWorldTransformation();
		monster->initUI();
		m_monsters.push_back(monster);
	}
	
	std::for_each(m_opponentMonsters.begin(), m_opponentMonsters.end(), std::mem_fn(&MonsterEntry::resetStats));
	m_supplyIndexOpponent = std::max(std::min(2, static_cast<int>(m_opponentMonsters.size()) - 1), 0);
	for (int i = 0; i < std::min(static_cast<int>(m_opponentMonsters.size()), 3); i++) {
		m_cells.push_back({ positions[i + 3][0], positions[i + 3][1], 192.0f, 192.0f, static_cast<int>(MonsterIndex::MonsterData[m_opponentMonsters[i].name].graphic * 16u), centers[i + 3][0], centers[i + 3][1], true, false });
		monster = addChild<Monster>(m_cells.back(), m_opponentMonsters[i]);
		monster->translateRelative(positions[i + 3][0], positions[i][1]);
		monster->updateWorldTransformation();
		monster->initUI();
		m_monsters.push_back(monster);
	}

	m_canSwitch = std::count_if(MonsterIndex::Monsters.begin() + m_supplyIndexPlayer + 1, MonsterIndex::Monsters.end(), [](const MonsterEntry& monsterEntry) { return monsterEntry.health > 0.0f; }) != 0;
	m_canCatch = canCatch;
}

void Battle::setBiomeBackground(const std::string& biomeBackground) {
	m_biomeBackground = biomeBackground;
}

void Battle::DrawBar(const Rect& rect, float value, float maxValue, const Vector4f& bgColor, const Vector4f& color, float radius) {
	float ratio = rect.width / maxValue;
	float progress = std::max(0.0f, std::min(rect.width, value * ratio));

	if (radius != 0.0f) {
		SurfaceBar.setShader(Globals::shaderManager.getAssetPointer("list"));
		auto shader = SurfaceBar.getShader();
		shader->use();
		shader->loadFloat("u_radius", radius);
		shader->loadUnsignedInt("u_edge", Edge::ALL);

		shader->loadVector("u_dimensions", Vector2f(rect.width, rect.height));
		SurfaceBar.setPosition(rect.posX, rect.posY);
		SurfaceBar.setScale(rect.width, rect.height);
		SurfaceBar.draw(bgColor);

		shader->loadVector("u_dimensions", Vector2f(progress, rect.height));
		SurfaceBar.setPosition(rect.posX, rect.posY);
		SurfaceBar.setScale(progress, rect.height);
		SurfaceBar.draw(color);

	}else {
		Spritesheet::Bind(TileSetManager::Get().getTileSet("monster_icon").getAtlas());
		SurfaceBar.resetShader();

		SurfaceBar.setPosition(rect.posX, rect.posY);
		SurfaceBar.setScale(rect.width, rect.height);
		SurfaceBar.draw(TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[16], bgColor);

		SurfaceBar.setPosition(rect.posX, rect.posY);
		SurfaceBar.setScale(progress, rect.height);
		SurfaceBar.draw(TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[16], color);
	}
}

void Battle::initUI() {
	setSpriteSheet(TileSetManager::Get().getTileSet("backgrounds").getAtlas());
	scale(BackgroundRect.width, BackgroundRect.height);	
}