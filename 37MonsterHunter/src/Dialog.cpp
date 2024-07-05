#include <engine/input/Keyboard.h>
#include <engine/Fontrenderer.h>
#include <Entities/Player.h>
#include "Dialog.h"
#include "Globals.h"

std::vector<Dialog> DialogTree::DialogData;

DialogTree::DialogTree(const Camera& camera) : 
	camera(camera), 
	m_currentIndex(-1), 
	m_finished(true), 
	m_blockInput(false),
	OnDialogFinished(nullptr),
	quadPos(Fontrenderer::Get().getBatchRenderer()->getQuadPos()),
	texPos(Fontrenderer::Get().getBatchRenderer()->getTexPos()),
	color(Fontrenderer::Get().getBatchRenderer()->getColor()),
	frame(Fontrenderer::Get().getBatchRenderer()->getFrame()) {
}

DialogTree::~DialogTree() {

}

void DialogTree::draw() {
	
	if (m_currentIndex >= 0) {
		Globals::fontManager.get("dialog").bind();
		const Dialog& dialogData = DialogData[m_currentIndex];
		
		float posX = dialogData.posX - camera.getPositionX();
		float posY = dialogData.posY - camera.getPositionY();
		float padding = 7.5f;
		Vector2f dimension = Vector2f(std::max(30.0f, Globals::fontManager.get("dialog").getWidth(dialogData.text) * 0.06f + 2.0f * padding), Globals::fontManager.get("dialog").lineHeight * 0.06f + 2.0f * padding);

		quadPos[0] = posX;
		quadPos[1] = posY;

		quadPos[2] = posX + dimension[0];
		quadPos[3] = posY;

		quadPos[4] = posX + dimension[0];
		quadPos[5] = posY + dimension[1];

		quadPos[6] = posX;
		quadPos[7] = posY + dimension[1];

		texPos[0] = 0.0f; texPos[1] = 0.0f;
		texPos[2] = 1.0f; texPos[3] = 0.0f;
		texPos[4] = 1.0f; texPos[5] = 1.0f;
		texPos[6] = 0.0f; texPos[7] = 1.0f;

		color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f; color[3] = 1.0f;
		frame = 0u;

		auto shader = Globals::shaderManager.getAssetPointer("dialog");
		shader->use();
		shader->loadFloat("u_radius", 5.0f);
		shader->loadVector("u_dimensions", dimension);

		Fontrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("dialog"));
		Fontrenderer::Get().getBatchRenderer()->processSingleQuad();
		Fontrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("font"));
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), dialogData.posX + dialogData.paddingX - camera.getPositionX() + padding, dialogData.posY + dialogData.paddingY - camera.getPositionY() + padding, dialogData.text, Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 0.06f);
		Fontrenderer::Get().drawBuffer();
	}
}

void DialogTree::addDialog(float posX, float posY, float paddingX, float paddingY, const std::string& text, int currentIndex) {
	DialogData.push_back({posX, posY, paddingX, paddingY, text});
	m_currentIndex = currentIndex;
}

void DialogTree::setFinished(bool finished) {
		m_finished = false;
}

void DialogTree::processInput() {
	if (m_finished || m_blockInput)
		return;

	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {		
			m_currentIndex++;
			if (m_currentIndex > (DialogData.size() - 1)) {
				m_finished = true;
				m_currentIndex = -1;
				DialogData.clear();
				DialogData.shrink_to_fit();

				if (OnDialogFinished) {
					OnDialogFinished();
					OnDialogFinished = nullptr;
				}
			}
	}
}

bool DialogTree::isFinished() {
	return m_finished;
}

void DialogTree::setBlockInput(bool blockInput) {
	m_blockInput = blockInput;
}

void DialogTree::incrementIndex() {
	m_currentIndex++;
}

void DialogTree::setOnDialogFinished(std::function<void()> fun) {
	OnDialogFinished = fun;
}