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

void WgpFontRenderer::init(size_t size, size_t maxBatches) {
	m_wgpBatchRenderer->init(size, maxBatches);
}

void WgpFontRenderer::addText(const CharacterSet& characterSet, const std::string& text, float posX, float posY,  const std::array<float, 4>& color, float size, bool flipGlyph) {
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		float kerningAmount = 0.0f;
		if (characterSet.hasKernings() && characterSet.kerningsHasChar(*c) && (c + 1) != text.end()) {
			const std::vector<Kerning>& kernings = characterSet.getKernings(*c);
			for (const Kerning& kerning : kernings) {
				if (kerning.nextChar == *(c + 1)) {
					kerningAmount = kerning.amount;
				}
			}
		}
		const Char& ch = characterSet.getCharacter(*c);
		m_wgpBatchRenderer->addQuadAA({ posX + ch.offset[0] * size, posY + ch.offset[1] * size, static_cast<float>(ch.size[0]) * size, static_cast<float>(ch.size[1]) * size },  { flipGlyph ? ch.textureOffset[0] : ch.textureOffset[0], flipGlyph ? ch.textureOffset[1] + ch.textureSize[1] : ch.textureOffset[1], flipGlyph ? ch.textureSize[0] : ch.textureSize[0], flipGlyph ? -ch.textureSize[1] : ch.textureSize[1] }, color, characterSet.layer);
		posX = posX + (ch.advance + kerningAmount) * size;
	}
}

void WgpFontRenderer::addTextTransformed(const CharacterSet& characterSet, const std::string& text, const float* transformation, const std::array<float, 4>& color, float size, bool flipGlyph) {
	std::string::const_iterator c;
	float offset = 0.0f;
	for (c = text.begin(); c != text.end(); c++) {
		float kerningAmount = 0.0f;
		if (characterSet.hasKernings() && characterSet.kerningsHasChar(*c) && (c + 1) != text.end()) {
			const std::vector<Kerning>& kernings = characterSet.getKernings(*c);
			for (const Kerning& kerning : kernings) {
				if (kerning.nextChar == *(c + 1)) {
					kerningAmount = kerning.amount;
				}
			}
		}
		const Char& ch = characterSet.getCharacter(*c);	

		//float sx = sqrtf(transformation[0] * transformation[0] + transformation[4] * transformation[4] + transformation[8] * transformation[8]);
		//float sy = sqrtf(transformation[1] * transformation[1] + transformation[5] * transformation[5] + transformation[9] * transformation[9]);
		//float sz = sqrtf(transformation[2] * transformation[2] + transformation[6] * transformation[6] + transformation[10] * transformation[10]);

		//sx = (1.0f / sx) * size;
		//sy = (1.0f / sy) * size;
		//sz = (1.0f / sz) * size;

		float v00 = offset +  ch.offset[0] * size;               float v01 =  ch.offset[1] * size;               //float v02 = 0.0f;
		float v10 = offset + (ch.offset[0] + ch.size[0]) * size; float v11 =  ch.offset[1] * size;               //float v12 = 0.0f;
		float v20 = offset + (ch.offset[0] + ch.size[0]) * size; float v21 = (ch.offset[1] + ch.size[1]) * size; //float v22 = 0.0f;
		float v30 = offset +  ch.offset[0] * size;               float v31 = (ch.offset[1] + ch.size[1]) * size; //float v32 = 0.0f;

		std::array<std::array<float,3>,4> vertices;
		vertices[0][0] = v00 * transformation[0] + v01 * transformation[4] + transformation[12]; // + v02 * transformation[8];
		vertices[0][1] = v00 * transformation[1] + v01 * transformation[5] + transformation[13]; // + v02 * transformation[9];
		vertices[0][2] = v00 * transformation[2] + v01 * transformation[6] + transformation[14]; // + v02 * transformation[10];

		vertices[1][0] = v10 * transformation[0] + v11 * transformation[4] + transformation[12]; // + v12 * transformation[8];
		vertices[1][1] = v10 * transformation[1] + v11 * transformation[5] + transformation[13]; // + v12 * transformation[9];
		vertices[1][2] = v10 * transformation[2] + v11 * transformation[6] + transformation[14]; // + v12 * transformation[10];

		vertices[2][0] = v20 * transformation[0] + v21 * transformation[4] + transformation[12]; // + v22 * transformation[8];
		vertices[2][1] = v20 * transformation[1] + v21 * transformation[5] + transformation[13]; // + v22 * transformation[9];
		vertices[2][2] = v20 * transformation[2] + v21 * transformation[6] + transformation[14]; // + v22 * transformation[10];

		vertices[3][0] = v30 * transformation[0] + v31 * transformation[4] + transformation[12]; // + v32 * transformation[8];
		vertices[3][1] = v30 * transformation[1] + v31 * transformation[5] + transformation[13]; // + v32 * transformation[9];
		vertices[3][2] = v30 * transformation[2] + v31 * transformation[6] + transformation[14]; // + v32 * transformation[10];

		m_wgpBatchRenderer->addQuad(vertices, { flipGlyph ? ch.textureOffset[0] : ch.textureOffset[0], flipGlyph ? ch.textureOffset[1] + ch.textureSize[1] : ch.textureOffset[1], flipGlyph ? ch.textureSize[0] : ch.textureSize[0], flipGlyph ? -ch.textureSize[1] : ch.textureSize[1] }, color, characterSet.layer);
		offset = offset + (ch.advance + kerningAmount) * size;
	}
}

void WgpFontRenderer::addText(const CharacterSet& characterSet, const FormatedText& text, float posX, float posY, const std::array<float, 4>& color, float size, bool flipGlyph) {
	float i = 0.0f;
	for (const std::string& str : text.splittedText) {
		float offsetY = characterSet.lineHeight * i;
		addText(characterSet, str, posX, posY - offsetY, color, size, flipGlyph);
		i += 1.0f;
	}
}

void WgpFontRenderer::addTextTransformed(const CharacterSet& characterSet, const FormatedText& text, const float* transformation, const std::array<float, 4>& color, float size, bool flipGlyph) {
	float trans[16];
	memcpy(trans, transformation, 16 * sizeof(float));

	for (size_t index = 0u; index < text.splittedText.size(); ++index) {
		float offsetY = (index == 0u) ? 0.0f : characterSet.lineHeight;		
		trans[12] = trans[12] - (offsetY * size * trans[4]); // + (dx * trans[0]) + (dz * trans[8])
		trans[13] = trans[13] - (offsetY * size * trans[5]); // + (dx * trans[1]) + (dz * trans[9])
		trans[14] = trans[14] - (offsetY * size * trans[6]); // + (dx * trans[2]) + (dz * trans[10])
		addTextTransformed(characterSet, text.splittedText[index], trans, color, size, flipGlyph);
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

void WgpFontRenderer::reset() {
	wgpBatchRenderer->reset();
}

FormatedText::FormatedText(const std::string& text) {
	FormatedText::create(text);
}
#include <iostream>
void FormatedText::create(const std::string& text) {
	if (text.empty())
		return;

	size_t len = text.length();
	char nextChar = (len > 0u) ? text[0] : -1;
	size_t offset = 0u;

	for (size_t i = 0; i < len; ++i) {
		char charCode = nextChar;
		nextChar = (i < len - 1) ? text[i + 1] : -1;
		switch (charCode) {
		case 10:
			if ((text.length() == 1) && (text[0] == '\n'))
				splittedText.push_back("");
			else if (i - offset == 0)
				splittedText.push_back("");
			else 				
				splittedText.push_back(text.substr(offset, i - offset));

			offset = i + 1u;
			break;
		default:
			break;
		}
	}

	splittedText.push_back(text.substr(offset, text.length() - offset));
}