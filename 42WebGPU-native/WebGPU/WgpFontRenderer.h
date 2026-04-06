#pragma once

#include <engine/CharacterSet.h>
#include "WgpBatchRenderer.h"

struct FormatedText {
	FormatedText() = default;
	FormatedText(const std::string& text);
	void create(const std::string& text);

	std::vector<std::string> splittedText;
};

class WgpFontRenderer {

public:

	void init(size_t size = 200u, size_t maxBatches = 4u);
	void draw(const WGPURenderPassEncoder& renderPassEncoder) const;

	void addText(const CharacterSet& characterSet, const std::string& text, float posX, float posY, const std::array<float, 4>& color = { 1.0f, 1.0f, 1.0f, 1.0f }, float size = 1.0f, bool flipGlyph = false);
	void addTextTransformed(const CharacterSet& characterSet, const std::string& text, const float* transformation, const std::array<float, 4>& color = { 1.0f, 1.0f, 1.0f, 1.0f }, float size = 1.0f, bool flipGlyph = false);
	
	void addText(const CharacterSet& characterSet, const FormatedText& text, float posX, float posY, const std::array<float, 4>& color = { 1.0f, 1.0f, 1.0f, 1.0f }, float size = 1.0f, bool flipGlyph = false);
	void addTextTransformed(const CharacterSet& characterSet, const FormatedText& text, const float* transformation, const std::array<float, 4>& color = { 1.0f, 1.0f, 1.0f, 1.0f }, float size = 1.0f, bool flipGlyph = false);
	
	void cleanup();
	void markForDelete();
	void reset();
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