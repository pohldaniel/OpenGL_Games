#pragma once
#include <functional>
#include <engine/Camera.h>
#include <UI/Surface.h>

#include "MonsterIndex.h"

struct DialogOptions {
	std::vector<std::string> undefeated;
	std::vector<std::string> defeated;
};

struct Trainer {
	std::vector<MonsterEntry> monsters;
	DialogOptions dialog;
	std::vector<ViewDirection> viewDirections;
	bool lookAround;
	bool defeated;
	std::string biome;
};

struct Dialog {
	float posX;
	float posY;
	float paddingX;
	float paddingY;
	std::string text;
};

class DialogTree : public ui::Surface {

public:

	DialogTree(const Camera& camera);
	virtual ~DialogTree();

	void draw();
	void addDialog(float posX, float posY, float paddingX, float paddingY, const std::string& text, int currentIndex = -1);
	void setFinished(bool finished);
	void processInput();
	bool isFinished();
	void setBlockInput(bool blockInput);
	void setOnDialogFinished(std::function<void()> fun);
	void incrementIndex();

	static std::unordered_map<std::string, Trainer> Trainers;

private:

	void drawDefault() override;
	void initUI();

	int m_currentIndex;
	bool m_finished;
	const Camera& camera;
	bool m_blockInput;
	float(&quadPos)[8];
	float(&texPos)[8];
	float(&color)[4];
	unsigned int& frame;
	std::function<void()> OnDialogFinished;
	static std::vector<Dialog> DialogData;
	
};