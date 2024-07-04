#pragma once

#include <engine/Camera.h>

struct DialogDataNew {
	float posX;
	float posY;
	std::string text;
};

class Dialog {

public:

	Dialog(const Camera& camera);
	~Dialog();

	void draw();
	void addDialog(float posX, float posY, const std::string& text);
	void setFinished(bool finished);
	void processInput();
	bool isFinished();

private:

	int m_currentIndex;
	bool m_finished;
	const Camera& camera;
	static std::vector<DialogDataNew> DialogData;
};