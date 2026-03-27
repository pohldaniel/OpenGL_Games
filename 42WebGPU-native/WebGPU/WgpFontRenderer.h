#pragma once

#include <engine/CharacterSet.h>
#include "WgpBatchRenderer.h"

class WgpFontRenderer {

public:

	void init(size_t size = 60);
	void draw(const WGPURenderPassEncoder& renderPassEncoder) const;
	void addText(const CharacterSet& characterSet, float posX, float posY, const std::string& text, const std::array<float, 4>& color = { 1.0f, 1.0f, 1.0f, 1.0f }, float size = 1.0f, bool flipGlyph = false);
	void cleanup();
	void markForDelete();
	void setBindGroups(const std::function<std::vector<WGPUBindGroup>()>& onBindGroups);

	WgpBatchRenderer* getWgpBatchRenderer();

	static WgpFontRenderer& Get();

private:

	WgpFontRenderer();
	~WgpFontRenderer();

	WgpBatchRenderer* m_wgpBatchRenderer;
	WgpBatchRenderer* wgpBatchRenderer;
	bool m_markForDelete;

	static WgpFontRenderer s_instance;
};