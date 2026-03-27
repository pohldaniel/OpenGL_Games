#include "WgpFontRenderer.h"

WgpFontRenderer WgpFontRenderer::s_instance;

WgpFontRenderer& WgpFontRenderer::Get() {
	return s_instance;
}

WgpFontRenderer::WgpFontRenderer() : m_wgpBatchRenderer(new WgpBatchRenderer()), m_markForDelete(false){
	wgpBatchRenderer = m_wgpBatchRenderer;
}

WgpFontRenderer::~WgpFontRenderer() {
	if (m_markForDelete) {
		cleanup();
	}
}

void WgpFontRenderer::cleanup() {
	if (wgpBatchRenderer == m_wgpBatchRenderer) {
		delete m_wgpBatchRenderer;
		m_wgpBatchRenderer = wgpBatchRenderer = nullptr;
	}
}

void WgpFontRenderer::markForDelete() {
	if (wgpBatchRenderer == m_wgpBatchRenderer)
		m_wgpBatchRenderer->markForDelete();

	m_markForDelete = true;
}

void WgpFontRenderer::init(size_t size) {
	m_wgpBatchRenderer->init(size);
}

void WgpFontRenderer::addText(const CharacterSet& characterSet, float posX, float posY, const std::string& text, const std::array<float, 4>& color, float size, bool flipGlyph) {
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		const Char& ch = characterSet.getCharacter(*c);
		m_wgpBatchRenderer->addQuadAA({ posX + ch.offset[0] * size, posY + ch.offset[1] * size, static_cast<float>(ch.size[0]) * size, static_cast<float>(ch.size[1]) * size },  { flipGlyph ? ch.textureOffset[0] : ch.textureOffset[0], flipGlyph ? ch.textureOffset[1] + ch.textureSize[1] : ch.textureOffset[1], flipGlyph ? ch.textureSize[0] : ch.textureSize[0], flipGlyph ? -ch.textureSize[1] : ch.textureSize[1] }, color, characterSet.layer);
		posX = posX + (ch.offset[0] + ch.advance) * size;
	}
}

WgpBatchRenderer* WgpFontRenderer::getWgpBatchRenderer() {
	return m_wgpBatchRenderer;
}

void WgpFontRenderer::setBindGroups(const std::function<std::vector<WGPUBindGroup>()>& onBindGroups) {
	wgpBatchRenderer->setBindGroups(onBindGroups);
}

void WgpFontRenderer::draw(const WGPURenderPassEncoder& renderPassEncoder) const {
	wgpBatchRenderer->draw(renderPassEncoder);
}