#pragma once
#include <functional>
#include "Shader.h"
#include "Constants.h"

enum class Mode {
	Veil = 0,
	Unveil = 1
};

class Transition {
public:
	Transition(const Transition&) = delete;

	static Transition& get();

	void init();
	void update(const float& dt);
	void start(Mode mode);
	void setFunction(std::function<void()> fun);
	Shader& getShader() const;

private:
	Transition() {}
	~Transition() {}

	static Transition instance;

	Shader* m_shader = nullptr;
	float m_updateTime = 0.0075f;
	float m_elapsedTime = 0.0f;
	float m_progress = 0.0f;

	Mode m_mode = Mode::Unveil;
	std::function<void()> m_fun = 0;

	void updateTransition(int multi);
};