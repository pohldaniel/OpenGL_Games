#pragma once
#include <vector>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/ui/Empty.h>
#include <engine/ui/Surface.h>
#include <engine/ui/Button.h>
#include <engine/ui/Label.h>
#include <engine/CharacterSet.h>

#include <engine/TrackBall.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

class Menu : public State, public MouseEventListener, public KeyboardEventListener {
	enum SelectedLayout {
		M_HORIZONTAL,
		M_VERTICAL,
		M_GRID
	};
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

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = false;

	Empty* m_uiScene;
	CharacterSet m_characterSet;
	SelectedLayout m_layout = SelectedLayout::M_VERTICAL;
};