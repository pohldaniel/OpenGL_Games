#pragma once

#include <engine/Camera.h>

struct Dialog {
	float posX;
	float posY;
	std::string text;
};

class DialogTree {

public:

	DialogTree(const Camera& camera);
	~DialogTree();

	void draw();
	void addDialog(float posX, float posY, const std::string& text);
	void setFinished(bool finished);
	void processInput();
	bool isFinished();

private:

	int m_currentIndex;
	bool m_finished;
	const Camera& camera;
	static std::vector<Dialog> DialogData;
};