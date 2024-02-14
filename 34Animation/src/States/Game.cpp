#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Utils/SolidIO.h"

Game::Game(StateMachine& machine) : State(machine, States::GAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.5f, 20.0f), Vector3f(0.0f, 2.5f, 20.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	Globals::animationManager.loadAnimation("vampire_dance", "res/models/vampire/dancing_vampire.dae", "Hips", "vampire_dance", false, 0u, 0u, 1000.0f);
	assimpAnimated.loadModel("res/models/vampire/dancing_vampire.dae", "res/models/vampire/textures/Vampire_diffuse.png");
	assimpAnimated.translate(0.0f, 0.0f, 0.0f);
	assimpAnimated.scale(0.03f, 0.03f, 0.03f);
	assimpAnimated.getAnimator()->addAnimation(Globals::animationManager.getAssetPointer("vampire_dance"));
	assimpAnimated.getAnimator()->startAnimation("vampire_dance");

	readMdl("res/models/BetaLowpoly/Beta.mdl");
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {

}

void Game::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f directrion = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		directrion += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		directrion += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xDelta();
		dy = mouse.yDelta();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);
		}

		if (move) {
			m_camera.move(directrion * m_dt);
		}
	}

	assimpAnimated.update(m_dt);
}

void Game::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	assimpAnimated.draw(m_camera);

	if (m_drawUi)
		renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {

}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Game::renderUi() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceId = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();

	if (m_initUi) {
		m_initUi = false;
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

ModelBone::ModelBone() :
	initialPosition(Vector3f::ZERO),
	initialRotation(Quaternion::IDENTITY),
	initialScale(Vector3f::ONE),
	offsetMatrix(Matrix4f::IDENTITY),
	radius(0.0f),
	boundingBox(0.0f, 0.0f),
	parentIndex(0),
	active(true)
{
}

ModelBone::~ModelBone()
{
}

void Game::readMdl(std::string path) {
	std::ifstream file(path, std::ios::binary);

	std::string ret;
	ret.resize(4);
	file.read(&ret[0], 4 * sizeof(char));
	char metaData[4];

	//Num VertexBuffer
	file.read(metaData, sizeof(unsigned int));

	file.read(metaData, sizeof(unsigned int));
	unsigned int vertexCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(metaData, sizeof(unsigned int));
	unsigned int numElements = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	unsigned int vertexSize = 0;

	for (unsigned j = 0; j < numElements; ++j) {
		file.read(metaData, sizeof(unsigned int));
		unsigned int elementDesc = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
		vertexSize += Utils::ELEMENT_TYPESIZES[elementDesc & 0xff];
	}

	file.read(metaData, sizeof(unsigned int));
	file.read(metaData, sizeof(unsigned int));

	char* buffer = new char[vertexCount * vertexSize];
	file.read(buffer, vertexCount * vertexSize);

	for (unsigned int i = 0; i < vertexCount * vertexSize; i = i + vertexSize) {

		Utils::UFloat value[3];
		value[0].c[0] = buffer[i + 0]; value[0].c[1] = buffer[i + 1]; value[0].c[2] = buffer[i + 2]; value[0].c[3] = buffer[i + 3];
		value[1].c[0] = buffer[i + 4]; value[1].c[1] = buffer[i + 5]; value[1].c[2] = buffer[i + 6]; value[1].c[3] = buffer[i + 7];
		value[2].c[0] = buffer[i + 8]; value[2].c[1] = buffer[i + 9]; value[2].c[2] = buffer[i + 10]; value[2].c[3] = buffer[i + 11];
		positions.push_back({ value[0].flt , value[1].flt , value[2].flt });

		value[0].c[0] = buffer[i + 12]; value[0].c[1] = buffer[i + 13]; value[0].c[2] = buffer[i + 14]; value[0].c[3] = buffer[i + 15];
		value[1].c[0] = buffer[i + 16]; value[1].c[1] = buffer[i + 17]; value[1].c[2] = buffer[i + 18]; value[1].c[3] = buffer[i + 19];
		value[2].c[0] = buffer[i + 20]; value[2].c[1] = buffer[i + 21]; value[2].c[2] = buffer[i + 22]; value[2].c[3] = buffer[i + 23];
		normals.push_back({ value[0].flt , value[1].flt , value[2].flt });

		value[0].c[0] = buffer[i + 24]; value[0].c[1] = buffer[i + 25]; value[0].c[2] = buffer[i + 26]; value[0].c[3] = buffer[i + 27];
		value[1].c[0] = buffer[i + 28]; value[1].c[1] = buffer[i + 29]; value[1].c[2] = buffer[i + 30]; value[1].c[3] = buffer[i + 31];
		uvCoords.push_back({ value[0].flt , value[1].flt });
	}
	delete buffer;

	// Num IndexBuffer
	file.read(metaData, sizeof(unsigned int));

	file.read(metaData, sizeof(unsigned int));
	unsigned int indexCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(metaData, sizeof(unsigned int));
	unsigned int indexSize = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	buffer = new char[indexCount * indexSize];
	file.read(buffer, indexCount * indexSize);

	for (unsigned int i = 0; i < indexCount * indexSize; i = i + indexSize * 3) {
		Utils::UShort value[3];

		value[0].c[0] = buffer[i + 0]; value[0].c[1] = buffer[i + 1];
		value[1].c[0] = buffer[i + 2]; value[1].c[1] = buffer[i + 3];
		value[2].c[0] = buffer[i + 4]; value[2].c[1] = buffer[i + 5];

		faces.push_back({ value[0].shrt, value[1].shrt, value[2].shrt });
	}
	delete buffer;

	file.read(metaData, sizeof(unsigned int));
	unsigned int numGeometries = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
	geomDescs.resize(numGeometries);

	//std::vector<std::vector<unsigned> > boneMappings;
	//std::set<std::pair<unsigned, unsigned> > processedVertices;
	//boneMappings.resize(numGeometries);

	for (size_t i = 0; i < numGeometries; ++i){
		file.read(metaData, sizeof(unsigned int));
		unsigned int boneMappingCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		file.read(metaData, sizeof(unsigned int));
		unsigned int numLodLevels = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		geomDescs[i].resize(numLodLevels);

		for (size_t j = 0; j < numLodLevels; ++j){

			GeometryDesc& geomDesc = geomDescs[i][j];

			file.read(metaData, sizeof(float));
			geomDesc.lodDistance = Utils::bytesToFloatLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			// Primitive type
			file.read(metaData, sizeof(unsigned int));

			file.read(metaData, sizeof(unsigned int));
			geomDesc.vbRef = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			file.read(metaData, sizeof(unsigned int));
			geomDesc.ibRef = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			file.read(metaData, sizeof(unsigned int));
			geomDesc.drawStart = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			file.read(metaData, sizeof(unsigned int));
			geomDesc.drawCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			// Apply bone mappings to geometry
			//if (boneMappingCount)
				//ApplyBoneMappings(geomDesc, boneMappings[i], processedVertices);
		}
	}

	//Num Morphs
	file.read(metaData, sizeof(unsigned int));
	unsigned int numMorphs = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(metaData, sizeof(unsigned int));
	unsigned int numBones = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	bones.resize(numBones);
	for (size_t i = 0; i < numBones; ++i){
		ModelBone& bone = bones[i];

		std::string boneName;
		while(true){
			file.read(metaData, sizeof(char));
			if (!metaData[0])
				break;
			else
				boneName += metaData[0];
		}

		bone.name = boneName;
		bone.nameHash = StringHash(bone.name);

		file.read(metaData, sizeof(unsigned int));
		bone.parentIndex = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);


		char* bufferBoneTrans = new char[48];

		file.read(bufferBoneTrans, 12);
		Utils::UFloat position[3];
		position[0].c[0] = bufferBoneTrans[0]; position[0].c[1] = bufferBoneTrans[1]; position[0].c[2] = bufferBoneTrans[2]; position[0].c[3] = bufferBoneTrans[3];
		position[1].c[0] = bufferBoneTrans[4]; position[1].c[1] = bufferBoneTrans[5]; position[1].c[2] = bufferBoneTrans[6]; position[1].c[3] = bufferBoneTrans[7];
		position[2].c[0] = bufferBoneTrans[8]; position[2].c[1] = bufferBoneTrans[9]; position[2].c[2] = bufferBoneTrans[10]; position[2].c[3] = bufferBoneTrans[11];
		bone.initialPosition.set(position[0].flt, position[1].flt, position[2].flt);

		file.read(bufferBoneTrans, 16);
		Utils::UFloat orientation[4];
		orientation[0].c[0] = bufferBoneTrans[0]; orientation[0].c[1] = bufferBoneTrans[1]; orientation[0].c[2] = bufferBoneTrans[2]; orientation[0].c[3] = bufferBoneTrans[3];
		orientation[1].c[0] = bufferBoneTrans[4]; orientation[1].c[1] = bufferBoneTrans[5]; orientation[1].c[2] = bufferBoneTrans[6]; orientation[1].c[3] = bufferBoneTrans[7];
		orientation[2].c[0] = bufferBoneTrans[8]; orientation[2].c[1] = bufferBoneTrans[9]; orientation[2].c[2] = bufferBoneTrans[10]; orientation[2].c[3] = bufferBoneTrans[11];
		orientation[3].c[0] = bufferBoneTrans[12]; orientation[3].c[1] = bufferBoneTrans[13]; orientation[3].c[2] = bufferBoneTrans[14]; orientation[3].c[3] = bufferBoneTrans[15];
		bone.initialRotation.set(orientation[0].flt, orientation[1].flt, orientation[2].flt, orientation[3].flt);

		file.read(bufferBoneTrans, 12);
		Utils::UFloat scale[3];
		scale[0].c[0] = bufferBoneTrans[0]; scale[0].c[1] = bufferBoneTrans[1]; scale[0].c[2] = bufferBoneTrans[2]; scale[0].c[3] = bufferBoneTrans[3];
		scale[1].c[0] = bufferBoneTrans[4]; scale[1].c[1] = bufferBoneTrans[5]; scale[1].c[2] = bufferBoneTrans[6]; scale[1].c[3] = bufferBoneTrans[7];
		scale[2].c[0] = bufferBoneTrans[8]; scale[2].c[1] = bufferBoneTrans[9]; scale[2].c[2] = bufferBoneTrans[10]; scale[2].c[3] = bufferBoneTrans[11];
		bone.initialScale.set(scale[0].flt, scale[1].flt, scale[2].flt);

		file.read(bufferBoneTrans, 48);
		Utils::UFloat offset[12];
		offset[0].c[0] = bufferBoneTrans[0]; offset[0].c[1] = bufferBoneTrans[1]; offset[0].c[2] = bufferBoneTrans[2]; offset[0].c[3] = bufferBoneTrans[3];
		offset[1].c[0] = bufferBoneTrans[4]; offset[1].c[1] = bufferBoneTrans[5]; offset[1].c[2] = bufferBoneTrans[6]; offset[1].c[3] = bufferBoneTrans[7];
		offset[2].c[0] = bufferBoneTrans[8]; offset[2].c[1] = bufferBoneTrans[9]; offset[2].c[2] = bufferBoneTrans[10]; offset[2].c[3] = bufferBoneTrans[11];
		offset[3].c[0] = bufferBoneTrans[12]; offset[3].c[1] = bufferBoneTrans[13]; offset[3].c[2] = bufferBoneTrans[14]; offset[3].c[3] = bufferBoneTrans[15];


		offset[4].c[0] = bufferBoneTrans[16]; offset[4].c[1] = bufferBoneTrans[17]; offset[4].c[2] = bufferBoneTrans[18]; offset[4].c[3] = bufferBoneTrans[19];
		offset[5].c[0] = bufferBoneTrans[20]; offset[5].c[1] = bufferBoneTrans[21]; offset[5].c[2] = bufferBoneTrans[22]; offset[5].c[3] = bufferBoneTrans[23];
		offset[6].c[0] = bufferBoneTrans[24]; offset[6].c[1] = bufferBoneTrans[25]; offset[6].c[2] = bufferBoneTrans[26]; offset[6].c[3] = bufferBoneTrans[27];
		offset[7].c[0] = bufferBoneTrans[28]; offset[7].c[1] = bufferBoneTrans[29]; offset[7].c[2] = bufferBoneTrans[30]; offset[7].c[3] = bufferBoneTrans[31];

		offset[8].c[0] = bufferBoneTrans[32]; offset[8].c[1] = bufferBoneTrans[33]; offset[8].c[2] = bufferBoneTrans[34]; offset[0].c[8] = bufferBoneTrans[35];
		offset[9].c[0] = bufferBoneTrans[36]; offset[9].c[1] = bufferBoneTrans[37]; offset[9].c[2] = bufferBoneTrans[38]; offset[1].c[9] = bufferBoneTrans[39];
		offset[10].c[0] = bufferBoneTrans[40]; offset[10].c[1] = bufferBoneTrans[41]; offset[10].c[2] = bufferBoneTrans[42]; offset[2].c[10] = bufferBoneTrans[43];
		offset[11].c[0] = bufferBoneTrans[44]; offset[11].c[1] = bufferBoneTrans[45]; offset[11].c[2] = bufferBoneTrans[46]; offset[3].c[11] = bufferBoneTrans[47];

		bone.offsetMatrix.set(offset[0].flt, offset[4].flt, offset[8].flt, 0.0f,
							  offset[1].flt, offset[5].flt, offset[9].flt, 0.0f,
                              offset[2].flt, offset[6].flt, offset[10].flt, 0.0f, 
                              offset[4].flt, offset[7].flt, offset[11].flt, 1.0f);
		
		delete bufferBoneTrans;

		file.read(metaData, sizeof(unsigned char));
		unsigned char boneCollisionType = metaData[0];

		if (boneCollisionType & 1){
			file.read(metaData, sizeof(float));
			bone.radius = Utils::bytesToFloatLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			if (bone.radius < BONE_SIZE_THRESHOLD * 0.5f)
				bone.active = false;
		}

		if (boneCollisionType & 2){
			char* bufferBox = new char[24];
			file.read(bufferBox, 24);
			Utils::UFloat box[6];
			box[0].c[0] = bufferBox[0]; box[0].c[1] = bufferBox[1]; box[0].c[2] = bufferBox[2]; box[0].c[3] = bufferBox[3];
			box[1].c[0] = bufferBox[4]; box[1].c[1] = bufferBox[5]; box[1].c[2] = bufferBox[6]; box[1].c[3] = bufferBox[7];
			box[2].c[0] = bufferBox[8]; box[2].c[1] = bufferBox[9]; box[2].c[2] = bufferBox[10]; box[2].c[3] = bufferBox[11];
			
			box[3].c[0] = bufferBox[12]; box[3].c[1] = bufferBox[13]; box[3].c[2] = bufferBox[14]; box[3].c[3] = bufferBox[15];
			box[4].c[0] = bufferBox[16]; box[4].c[1] = bufferBox[17]; box[4].c[2] = bufferBox[18]; box[4].c[3] = bufferBox[19];
			box[5].c[0] = bufferBox[20]; box[5].c[1] = bufferBox[21]; box[5].c[2] = bufferBox[22]; box[5].c[3] = bufferBox[23];

			bone.boundingBox.min.set(box[0].flt, box[1].flt, box[2].flt);
			bone.boundingBox.max.set(box[3].flt, box[4].flt, box[5].flt);
			delete bufferBox;
			if (bone.boundingBox.getSize().length() < BONE_SIZE_THRESHOLD)
				bone.active = false;
		}
	}

	char* bufferBox = new char[24];
	file.read(bufferBox, 24);
	Utils::UFloat box[6];
	box[0].c[0] = bufferBox[0]; box[0].c[1] = bufferBox[1]; box[0].c[2] = bufferBox[2]; box[0].c[3] = bufferBox[3];
	box[1].c[0] = bufferBox[4]; box[1].c[1] = bufferBox[5]; box[1].c[2] = bufferBox[6]; box[1].c[3] = bufferBox[7];
	box[2].c[0] = bufferBox[8]; box[2].c[1] = bufferBox[9]; box[2].c[2] = bufferBox[10]; box[2].c[3] = bufferBox[11];

	box[3].c[0] = bufferBox[12]; box[3].c[1] = bufferBox[13]; box[3].c[2] = bufferBox[14]; box[3].c[3] = bufferBox[15];
	box[4].c[0] = bufferBox[16]; box[4].c[1] = bufferBox[17]; box[4].c[2] = bufferBox[18]; box[4].c[3] = bufferBox[19];
	box[5].c[0] = bufferBox[20]; box[5].c[1] = bufferBox[21]; box[5].c[2] = bufferBox[22]; box[5].c[3] = bufferBox[23];

	boundingBox.min.set(box[0].flt, box[1].flt, box[2].flt);
	boundingBox.max.set(box[3].flt, box[4].flt, box[5].flt);
	delete bufferBox;

	//std::cout << "Box Min: " << boundingBox.min[0] << "  " << boundingBox.min[1] << "  " << boundingBox.min[2] << std::endl;
	//std::cout << "Box Max: " << boundingBox.max[0] << "  " << boundingBox.max[1] << "  " << boundingBox.max[2] << std::endl;
	file.close();
}