#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <engine/input/Keyboard.h>
#include <Entities/Player.h>
#include "Dialog.h"
#include "Globals.h"

std::vector<Dialog> DialogTree::DialogData;
std::unordered_map<std::string, Trainer> DialogTree::Trainers;

DialogTree::DialogTree(const Camera& camera) : 
	camera(camera), 
	m_currentIndex(-1), 
	m_finished(true), 
	m_blockInput(false),
	OnDialogFinished(nullptr){

	std::ifstream file("res/trainer.json", std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Could not open file: " << "res/trainer.json" << std::endl;
	}

	rapidjson::IStreamWrapper streamWrapper(file);
	rapidjson::Document doc;
	doc.ParseStream(streamWrapper);

	for (rapidjson::Value::ConstMemberIterator trainer = doc.MemberBegin(); trainer != doc.MemberEnd(); ++trainer) {
		if (!trainer->value["biome"].IsNull()) {
			Trainers[trainer->name.GetString()].biome = trainer->value["biome"].GetString();
		}
		Trainers[trainer->name.GetString()].defeated = trainer->value["defeated"].GetBool();
		Trainers[trainer->name.GetString()].lookAround = trainer->value["look_around"].GetBool();

		if (trainer->value.HasMember("monsters")) {
			rapidjson::GenericArray<true, rapidjson::Value>  monsters = trainer->value["monsters"].GetArray();
			for (rapidjson::Value::ConstValueIterator monster = monsters.Begin(); monster != monsters.End(); ++monster) {
				for (rapidjson::Value::ConstMemberIterator iter = monster->MemberBegin(); iter != monster->MemberEnd(); ++iter) {
					Trainers[trainer->name.GetString()].monsters.push_back({ iter->name.GetString(), iter->value.GetUint(), false });
				}
			}
		}

		rapidjson::GenericArray<true, rapidjson::Value>  directions = trainer->value["directions"].GetArray();
		for (rapidjson::Value::ConstValueIterator direction = directions.Begin(); direction != directions.End(); ++direction) {
			std::string _direction = direction->GetString();
			if (_direction == "up")
				Trainers[trainer->name.GetString()].viewDirections.push_back(ViewDirection::UP);
			else if (_direction == "down")
				Trainers[trainer->name.GetString()].viewDirections.push_back(ViewDirection::DOWN);
			else if (_direction == "left")
				Trainers[trainer->name.GetString()].viewDirections.push_back(ViewDirection::LEFT);
			else if (_direction == "right")
				Trainers[trainer->name.GetString()].viewDirections.push_back(ViewDirection::RIGHT);
		}

		rapidjson::GenericArray<true, rapidjson::Value>  undefeated = trainer->value["dialog"]["default"].GetArray();
		for (rapidjson::Value::ConstValueIterator entry = undefeated.Begin(); entry != undefeated.End(); ++entry) {
			Trainers[trainer->name.GetString()].dialog.undefeated.push_back(entry->GetString());
		}

		if (!trainer->value["dialog"]["defeated"].IsNull()) {
			rapidjson::GenericArray<true, rapidjson::Value>  defeated = trainer->value["dialog"]["defeated"].GetArray();
			for (rapidjson::Value::ConstValueIterator entry = defeated.Begin(); entry != defeated.End(); ++entry) {
				Trainers[trainer->name.GetString()].dialog.defeated.push_back(entry->GetString());
			}
		}
		
	}
	file.close();
	initUI();
}

DialogTree::~DialogTree() {

}

void DialogTree::draw() {
	if (m_currentIndex >= 0) {
		ui::Widget::draw();
	}
}

void DialogTree::addDialog(float posX, float posY, float paddingX, float paddingY, const std::string& text, int currentIndex) {
	DialogData.push_back({posX, posY, paddingX, paddingY, text});
	m_currentIndex = currentIndex;
	updateUI();
}

void DialogTree::setFinished(bool finished) {
		m_finished = finished;
}

void DialogTree::processInput() {
	if (m_finished || m_blockInput)
		return;

	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {		
			m_currentIndex++;
			if (m_currentIndex > (DialogData.size() - 1)) {
				
				m_finished = true;
				m_currentIndex = -1;
				DialogData.clear();
				DialogData.shrink_to_fit();				

				if (OnDialogFinished) {
					OnDialogFinished();
					OnDialogFinished = nullptr;
				}
			}
			updateUI();
	}
}

bool DialogTree::isFinished() {
	return m_finished;
}

void DialogTree::setBlockInput(bool blockInput) {
	m_blockInput = blockInput;
}

void DialogTree::setOnDialogFinished(std::function<void()> fun) {
	OnDialogFinished = fun;
}

void DialogTree::initUI() {
	setShader(Globals::shaderManager.getAssetPointer("list"));
	setBorderRadius(5.0f);
	setEdge(ui::Edge::ALL);
	setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	ui::Label* label = addChild<ui::Label>(Globals::fontManager.get("dialog"));
	label->setName("text");
}

void DialogTree::updateUI() {

	if (m_currentIndex >= 0) {
		const Dialog& dialogData = DialogData[m_currentIndex];

		float posX = dialogData.posX - camera.getPositionX();
		float posY = dialogData.posY - camera.getPositionY();
		float padding = 7.5f;
		Vector2f dimension = Vector2f(std::max(30.0f, Globals::fontManager.get("dialog").getWidth(dialogData.text) * 0.075f + 2.0f * padding), Globals::fontManager.get("dialog").lineHeight * 0.075f + 2.0f * padding);

		setPosition(posX, posY);
		setScale(dimension[0], dimension[1]);
		setOrigin(dimension[0] * 0.5f, dimension[1] * 0.5f);

		ui::Label* label = findChild<ui::Label>("text");
		label->setLabel(dialogData.text);
		label->setOffsetX(dialogData.paddingX + padding);
		label->setOffsetY(dialogData.paddingY + padding);
		label->setSize(0.075f);
	}
}