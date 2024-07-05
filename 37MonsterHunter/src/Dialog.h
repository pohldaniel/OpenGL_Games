#pragma once
#include <functional>
#include <engine/Camera.h>

struct Dialog {
	float posX;
	float posY;
	float paddingX;
	float paddingY;
	std::string text;
};

class DialogTree {

public:

	DialogTree(const Camera& camera);
	~DialogTree();

	void draw();
	void addDialog(float posX, float posY, float paddingX, float paddingY, const std::string& text, int currentIndex = -1);
	void setFinished(bool finished);
	void processInput();
	bool isFinished();
	void setBlockInput(bool blockInput);
	void setOnDialogFinished(std::function<void()> fun);
	void incrementIndex();

private:

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