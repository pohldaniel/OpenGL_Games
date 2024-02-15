#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <engine/DebugRenderer.h>

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
	m_camera.lookAt(Vector3f(0.0f, 1.0f, -5.0f), Vector3f(0.0f, 1.0f, -5.0f) + Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	//glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	//Globals::animationManager.loadAnimation("vampire_dance", "res/models/vampire/dancing_vampire.dae", "Hips", "vampire_dance", false, 0u, 0u, 1000.0f);
	//assimpAnimated.loadModel("res/models/vampire/dancing_vampire.dae", "res/models/vampire/textures/Vampire_diffuse.png");
	//assimpAnimated.translate(0.0f, 0.0f, 0.0f);
	//assimpAnimated.scale(0.03f, 0.03f, 0.03f);
	//assimpAnimated.getAnimator()->addAnimation(Globals::animationManager.getAssetPointer("vampire_dance"));
	//assimpAnimated.getAnimator()->startAnimation("vampire_dance");

	readMdl("res/models/BetaLowpoly/Beta.mdl");
	CreateBones();
	animation = new Animation();
	//animation->loadAni("res/models/BetaLowpoly/Beta_Idle.ani");
	animation->loadAni("res/models/BetaLowpoly/Beta_Run.ani");
	AnimationState* state = new AnimationState(animation, rootBone);
	state->SetLooped(true);
	animationStates.push_back(std::shared_ptr<AnimationState>(state));
	DebugRenderer::Get().setEnable(true);

	glGenBuffers(1, &BuiltInShader::matrixUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4f) * numBones, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 3, BuiltInShader::matrixUbo, 0, sizeof(Matrix4f) * numBones);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
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

	//assimpAnimated.update(m_dt);
	UpdateAnimation();
}

void Game::render() {
	UpdateSkinning();

	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * numBones, skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DebugRenderer::Get().SetView(&m_camera);

	DebugRenderer::Get().AddBoundingBox(boundingBox, {1.0f, 0.0f, 0.0f, 1.0f});
	DebugRenderer::Get().AddSkeleton(bones, numBones, { 0.0f, 1.0f, 0.0f, 1.0f });

	DebugRenderer::Get().drawBuffer();
	
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

	modelBones.resize(numBones);

	for (size_t i = 0; i < numBones; ++i){
		ModelBone& bone = modelBones[i];

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
		bone.initialRotation.set(orientation[1].flt, orientation[2].flt, orientation[3].flt, orientation[0].flt);
		
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

		offset[8].c[0]  = bufferBoneTrans[32]; offset[8].c[1]  = bufferBoneTrans[33]; offset[8].c[2]  = bufferBoneTrans[34]; offset[8].c[3] = bufferBoneTrans[35];
		offset[9].c[0]  = bufferBoneTrans[36]; offset[9].c[1]  = bufferBoneTrans[37]; offset[9].c[2]  = bufferBoneTrans[38]; offset[9].c[3] = bufferBoneTrans[39];
		offset[10].c[0] = bufferBoneTrans[40]; offset[10].c[1] = bufferBoneTrans[41]; offset[10].c[2] = bufferBoneTrans[42]; offset[10].c[3] = bufferBoneTrans[43];
		offset[11].c[0] = bufferBoneTrans[44]; offset[11].c[1] = bufferBoneTrans[45]; offset[11].c[2] = bufferBoneTrans[46]; offset[11].c[3] = bufferBoneTrans[47];

		bone.offsetMatrix.set(offset[0].flt, offset[4].flt, offset[8].flt, 0.0f,
							  offset[1].flt, offset[5].flt, offset[9].flt, 0.0f,
		                      offset[2].flt, offset[6].flt, offset[10].flt, 0.0f, 
		                      offset[3].flt, offset[7].flt, offset[11].flt, 1.0f);

		//bone.offsetMatrix.set(offset[0].flt, offset[1].flt, offset[2].flt, offset[3].flt,
		//	offset[4].flt, offset[5].flt, offset[6].flt, offset[7].flt,
		//	offset[8].flt, offset[8].flt, offset[10].flt, offset[11].flt,
		//	0.0f, 0.0f, 0.0f, 1.0f);
		
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

void Game::RemoveBones(){
	if (!numBones)
		return;

	// Do not signal transform changes back to the model during deletion
	//for (size_t i = 0; i < numBones; ++i)
		//bones[i]->SetDrawable(nullptr);

	if (rootBone){
		rootBone->removeSelf();
		rootBone = nullptr;
	}

	bones = nullptr;
	skinMatrices = nullptr;
	//skinMatrixBuffer = nullptr;
	numBones = 0;
}

void Game::CreateBones() {

	//if (!model){
		//skinMatrixBuffer.Reset();
		//RemoveBones();
		//return;
	//}

	//const std::vector<ModelBone>& modelBones = modelBones;
	if (numBones != modelBones.size())
		RemoveBones();

	numBones = (unsigned short)modelBones.size();

	bones = new Bone*[numBones];
	skinMatrices = new Matrix4f[numBones];

	for (size_t i = 0; i < modelBones.size(); ++i){
		const ModelBone& modelBone = modelBones[i];

		// Try to find existing bone from scene hierarchy, if not found create new
		//bones[i] = owner->FindChild<Bone>(modelBone.nameHash, true);

		//if (!bones[i]){
			bones[i] = new Bone();
			bones[i]->SetName(modelBone.name);
			bones[i]->setPosition(modelBone.initialPosition);
			bones[i]->setOrientation({ modelBone.initialRotation[0], modelBone.initialRotation[1], modelBone.initialRotation[2], modelBone.initialRotation[3] });
			bones[i]->setScale(modelBone.initialScale);
			//bones[i]->SetTransform(modelBone.initialPosition, modelBone.initialRotation, modelBone.initialScale);
		//}

		//bones[i]->SetDrawable(this);
	}

	// Loop through bones again to set the correct parents
	for (size_t i = 0; i < modelBones.size(); ++i){
		const ModelBone& desc = modelBones[i];
		if (desc.parentIndex == i){
			bones[i]->setParent(nullptr);
			rootBone = bones[i];
			bones[i]->offsetMatrix = modelBones[i].offsetMatrix;
		}else{					
			bones[i]->setParent(bones[desc.parentIndex]);
			bones[i]->offsetMatrix = modelBones[i].offsetMatrix;
		}
	}

	// Count child bones now for optimized transform dirtying
	for (size_t i = 0; i < modelBones.size(); ++i)
		bones[i]->CountChildBones();

	//if (!skinMatrixBuffer)
		//skinMatrixBuffer = new UniformBuffer();
	//skinMatrixBuffer->Define(USAGE_DYNAMIC, numBones * sizeof(Matrix3x4));

	// Set initial bone bounding box recalculation and skinning dirty. Also calculate a valid bone bounding box immediately to ensure models can enter the view without updating animation first
	//OnBoneTransformChanged();
	//OnWorldBoundingBoxUpdate();
}

void Game::UpdateAnimation(){

	//if (animatedModelFlags & AMF_ANIMATION_ORDER_DIRTY)
	//	std::sort(animationStates.begin(), animationStates.end(), CompareAnimationStates);

	//animatedModelFlags |= AMF_IN_ANIMATION_UPDATE | AMF_BONE_BOUNDING_BOX_DIRTY;

	// Reset bones to initial pose, then apply animations
	for (size_t i = 0; i < numBones; ++i){
		Bone* bone = bones[i];
		const ModelBone& modelBone = modelBones[i];
		if (bone->AnimationEnabled())
			bone->SetTransformSilent(modelBone.initialPosition, modelBone.initialRotation, modelBone.initialScale);
	}

	for (auto it = animationStates.begin(); it != animationStates.end(); ++it){
		AnimationState* state = (*it).get();
		if (state->Enabled()) {
			state->AddTime(m_dt);
			state->Apply();
		}
	}

	/*// Dirty the bone hierarchy now. This will also dirty and queue reinsertion for attached models
	SetBoneTransformsDirty();

	animatedModelFlags &= ~(AMF_ANIMATION_ORDER_DIRTY | AMF_ANIMATION_DIRTY | AMF_IN_ANIMATION_UPDATE);

	// Update bounding box already here to take advantage of threaded update, and also to update bone world transforms for skinning
	OnWorldBoundingBoxUpdate();

	// If updating only when visible, queue octree reinsertion for next frame. This also ensures shadowmap rendering happens correctly
	// Else just dirty the skinning
	if (!TestFlag(DF_UPDATE_INVISIBLE))
	{
		if (octree && octant && !TestFlag(DF_OCTREE_REINSERT_QUEUED))
			octree->QueueUpdate(this);
	}

	animatedModelFlags |= AMF_SKINNING_DIRTY;*/
}

void Game::UpdateSkinning(){

	for (size_t i = 0; i < numBones; ++i)
		skinMatrices[i] = bones[i]->getWorldTransformation() * modelBones[i].offsetMatrix;

	//animatedModelFlags &= ~AMF_SKINNING_DIRTY;
	//animatedModelFlags |= AMF_SKINNING_BUFFER_DIRTY;
}
