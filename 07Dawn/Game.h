#pragma once
#include <vector>

#include "engine/Rect.h"
#include "engine/Batchrenderer.h"
#include "StateMachine.h"

class Game : public State {

	friend class LoadingManager;
	friend class Editor;

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

	static std::vector<TextureRect> TextureRects;
	static void Init();

private:
	
	void processInput();
	static bool s_init;
};