#include "OptionsWindow.h"
#include "Interface.h"
#include "Editor.h"
#include "Quest.h"
#include "Application.h"
#include "Utils.h"
#include "Constants.h"

OptionsWindow OptionsWindow::s_instance;

OptionsWindow & OptionsWindow::Get() {
	return s_instance;
}

OptionsWindow::OptionsWindow() : Widget(0, 0, 279, 217, 20, 19) {
}

OptionsWindow::~OptionsWindow(){}

void OptionsWindow::init(std::vector<TextureRect> textures) {
	m_textures = textures;
	m_font = &Globals::fontManager.get("verdana_20");
	m_visible = false;
	m_keepInputHandling = true;
	setTextureDependentPositions();
	Interface::Get().connectWidget(*this, false);
}

void OptionsWindow::draw() {
	if (!m_visible) return;
	TextureManager::DrawTextureBatched(m_textures[0], m_posX, m_posY, false, false);

	// show option names (continue, quit, load, save, pause)
	int textX = m_posX + 64;
	int textY = m_posY + m_height - 64 - m_font->lineHeight;

	int selectedEntry = -1;
	if (ViewPort::Get().getCursorPosRelX() < m_posX + 64 || ViewPort::Get().getCursorPosRelX() > m_posX + m_width - 64 || m_posY + m_height - 64 < ViewPort::Get().getCursorPosRelY()) {
		selectedEntry = -1;
	}else {
		selectedEntry = (m_posY + m_height - 64 - ViewPort::Get().getCursorPosRelY()) / static_cast<int>(m_font->lineHeight * 1.5);
		if ((m_posY + m_height - 64 - ViewPort::Get().getCursorPosRelY()) % static_cast<int>(m_font->lineHeight*1.5) > static_cast<int>(m_font->lineHeight)) {
			selectedEntry = -1;
		}
	}

	Fontrenderer::Get().addText(*m_font, textX, textY, "Quit Game", (selectedEntry == 0) ? Vector4f(1.0f, 1.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	textY -= static_cast<int>(m_font->lineHeight * 1.5);
	Fontrenderer::Get().addText(*m_font, textX, textY, "Load Game", (selectedEntry == 1) && !Utils::file_exists("res/_lua/save/savegame.lua") ? Vector4f(1.0f, 0.0f, 0.0f, 1.0f) : !Utils::file_exists("res/_lua/save/savegame.lua") ? Vector4f(0.5f, 0.5f, 0.5f, 1.0f) : (selectedEntry == 1) ? Vector4f(1.0f, 1.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	textY -= static_cast<int>(m_font->lineHeight * 1.5);
	Fontrenderer::Get().addText(*m_font, textX, textY, "Save Game", (selectedEntry == 2) && !Globals::savingAllowed ? Vector4f(1.0f, 0.0f, 0.0f, 1.0f) : !Globals::savingAllowed ? Vector4f(0.5f, 0.5f, 0.5f, 1.0f) : (selectedEntry == 2 ) ? Vector4f(1.0f, 1.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	textY -= static_cast<int>(m_font->lineHeight * 1.5);
	Fontrenderer::Get().addText(*m_font, textX, textY, "Editor", (selectedEntry == 3) ? Vector4f(1.0f, 1.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	textY -= static_cast<int>(m_font->lineHeight * 1.5);
	Fontrenderer::Get().addText(*m_font, textX, textY, "Continue", (selectedEntry == 4) ? Vector4f(1.0f, 1.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	textY -= static_cast<int>(m_font->lineHeight * 1.5);
	Fontrenderer::Get().addText(*m_font, textX, textY, "Pause", (selectedEntry == 5) ? Vector4f(1.0f, 1.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
}

void OptionsWindow::processInput() {

	if (Keyboard::instance().keyPressed(Keyboard::KEY_1)) {	
		
		if (Interface::Get().getWidgets().size() > 0) {
			Interface::Get().closeAll();
			close();
		}else {
			activate();
		}	
	}

	if (!m_visible) return;
	Widget::processInput();	


	if (!isMouseOnFrame(Mouse::instance().xPosAbsolute(), Mouse::instance().yPosAbsolute())) {
		return;
	}

	// check for quit and the other options
	int selectedEntry = -1;
	if (ViewPort::Get().getCursorPosRelX() < m_posX + 64 || ViewPort::Get().getCursorPosRelX() > m_posX + m_width - 64 || m_posY + m_height - 64 < ViewPort::Get().getCursorPosRelY()) {
		selectedEntry = -1;

	}else {
		selectedEntry = Mouse::instance().buttonPressed(Mouse::BUTTON_LEFT) ? (m_posY + m_height - 64 - ViewPort::Get().getCursorPosRelY()) / static_cast<int>(m_font->lineHeight * 1.5) : -1;
		if ((m_posY + m_height - 64 - ViewPort::Get().getCursorPosRelY()) % static_cast<int>(m_font->lineHeight*1.5) > static_cast<int>(m_font->lineHeight)) {
			selectedEntry = -1;
		}
	}

	if (selectedEntry == 0){
		//setQuitGame();
	} else if (selectedEntry == 1 && Utils::file_exists("res/_lua/save/savegame.lua") == true) {
		// Load Game

		// clear current game data
		QuestCanvas::Get().removeAllQuests();
		std::unordered_map<std::string, Zone>& allZones = ZoneManager::Get().getAllZones();
		for (std::unordered_map<std::string, Zone>::iterator it = allZones.begin(); it != allZones.end(); ++it) {

			Zone& currentZone = ZoneManager::Get().getZone(it->first);
			currentZone.purgeInteractionList();
			currentZone.purgeInteractionRegionList();
			currentZone.purgeNpcList();
		}

		Player::Get().clearInventory();	
		Spellbook::Get().clear();
		Interface::Get().clear();
		Player::Get().clearCooldownSpells();
		Player::Get().clearActiveSpells();

		// reenter map
		// 1. Load all zones
		// TODO: Load all zones
		// 2. Restore lua variables
		LuaFunctions::executeLuaScript("loadGame( 'res/_lua/save/savegame' )");
		DawnInterface::clearLogWindow();

	}else if (selectedEntry == 2) {
		if (Globals::savingAllowed) {
			// save Game
			LuaFunctions::executeLuaScript("saveGame( 'res/_lua/save/savegame' )");
			GLfloat yellow[] = { 1.0f, 1.0f, 0.0f };
			DawnInterface::addTextToLogWindow(yellow, "Game saved.");
		}
	}else if (selectedEntry == 3){
		Interface::Get().closeAll();
		close();
		Application::AddStateAtTop(new Editor(Application::GetStateMachine()));

	} else if (selectedEntry == 4) {
		close();

	} else if (selectedEntry == 5) {
		GLfloat yellow[] = { 1.0f, 1.0f, 0.0f };
		if (Globals::isPaused) {
			Globals::isPaused = false;
			DawnInterface::addTextToLogWindow(yellow, "Game unpaused. (not implemented)");

		}else {
			Globals::isPaused = true;
			DawnInterface::addTextToLogWindow(yellow, "Game paused. (not implemented)");

		}
	}
}

void OptionsWindow::setTextureDependentPositions() {
	m_width = m_textures[0].width;
	m_height = m_textures[0].height;
	// center on screen
	m_posX = (ViewPort::Get().getWidth() - m_width) / 2;
	m_posY = (ViewPort::Get().getHeight() - m_height) / 2;
}

void  OptionsWindow::resize(int deltaW, int deltaH) {
	setTextureDependentPositions();
}