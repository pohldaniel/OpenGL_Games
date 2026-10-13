#pragma once
#include <vector>
#include <iostream>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/thread/ILoadingListener.h>
#include <engine/thread/LoadingManager.h>

#include <engine/ui/Empty.h>
#include <engine/ui/Surface.h>
#include <engine/ui/Button.h>
#include <engine/ui/Label.h>
#include <engine/ui/Canvas.h>
#include <engine/CharacterSet.h>

#include <engine/TrackBall.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

class NoiseGenerationTask : public ILoadingListener {
public:

    NoiseGenerationTask(int width, int height, int seed,  std::function<void()> onUploadComplete)
        : m_width(width), m_height(height), m_seed(seed), m_onUploadComplete(onUploadComplete) {
    }

    void OnProcess() override;
    void OnComplete() override {
        WGPUTexelCopyTextureInfo copyDst = {};
        copyDst.texture = uiContext.texture;
        copyDst.mipLevel = 0u;
        copyDst.origin = { 0u, 0u, 1u };
        copyDst.aspect = WGPUTextureAspect_All;

        WGPUTexelCopyBufferLayout dataLayout = {};
        dataLayout.offset = 0u;
        dataLayout.bytesPerRow = m_width * 4u;
        dataLayout.rowsPerImage = m_height;

        WGPUExtent3D writeSize = {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 1u };

        wgpuQueueWriteTexture(
            wgpContext.queue,
            &copyDst,
            m_pixelData.data(),
            m_pixelData.size(),
            &dataLayout,
            &writeSize
        );

        if (m_onUploadComplete) {
            m_onUploadComplete();
        }

        delete this;
    }

private:

    int m_width;
    int m_height;
    int m_seed;
    std::function<void()> m_onUploadComplete;
    std::vector<unsigned char> m_pixelData;
};

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
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void resize(int deltaW, int deltaH) override;

private:

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = false;

	Empty* m_uiScene;
	CharacterSet m_characterSet;
	SelectedLayout m_layout = SelectedLayout::M_GRID;
};