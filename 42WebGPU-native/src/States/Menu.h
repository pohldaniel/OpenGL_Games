#pragma once
#include <vector>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/ui/Widget.h>
#include <engine/ui/Surface.h>
#include <engine/ui/Button.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

class Menu : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Menu(StateMachine& machine);
	~Menu();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);

	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void resize(int deltaW, int deltaH) override;

private:
	Surface* m_surface;
};