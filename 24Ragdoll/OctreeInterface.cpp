#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "OctreeInterface.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

#include <tracy/Tracy.hpp>


OctreeInterface::OctreeInterface(StateMachine& machine) : State(machine, CurrentState::OCTREEINTERFACE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	glClearColor(0.7f, 0.7f, 0.7f, 0.0f);

	bool useThreads = true;

	// Create subsystems that don't depend on the application window / OpenGL context
	workQueue = new WorkQueue(useThreads ? 0 : 1);
	log = new Log();
	cache = new ResourceCache();
	cache->AddResourceDir("Data");

	// Create the Graphics subsystem to open the application window and initialize OpenGL
	graphics = new Graphics("Turso3D renderer test", IntVector2(Application::Width, Application::Height));
	graphics->Initialize();
	hasInstancing = false;
	if (hasInstancing){
		instanceVertexBuffer = new VertexBuffer();
		instanceVertexElements.push_back(VertexElement(ELEM_VECTOR4, SEM_TEXCOORD, 3));
		instanceVertexElements.push_back(VertexElement(ELEM_VECTOR4, SEM_TEXCOORD, 4));
		instanceVertexElements.push_back(VertexElement(ELEM_VECTOR4, SEM_TEXCOORD, 5));
	}
	// Create subsystems that depend on the application window / OpenGL
	RegisterRendererLibrary();

	debugRenderer = new DebugRenderer();


	scene = ObjectTu::Create<Scene>();
	camera = ObjectTu::Create<CameraTu>();

	CreateScene(scene, camera, 0);

	camera->SetPosition(Vector3(0.0f, 20.0f, 75.0f));
	camera->SetAspectRatio((float)Application::Width / (float)Application::Height);

	camera->setPosition(Vector3f(0.0f, 20.0f, 75.0f));
	camera->perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);


	perViewDataBuffer = new UniformBuffer();
	perViewDataBuffer->Define(USAGE_DYNAMIC, sizeof(PerViewUniforms));

	octantResults = new OctreeInterface::ThreadOctantResult[NUM_OCTANT_TASKS];
	batchResults = new OctreeInterface::ThreadBatchResult[workQueue->NumThreads()];
	for (size_t i = 0; i < NUM_OCTANTS + 1; ++i) {
		collectOctantsTasks[i] = new OctreeInterface::CollectOctantsTask(this, &OctreeInterface::CollectOctantsWork);
		collectOctantsTasks[i]->resultIdx = i;
	}
}

OctreeInterface::~OctreeInterface() {
	Globals::physics->removeAllCollisionObjects();
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void OctreeInterface::fixedUpdate() {
	Globals::physics->stepSimulation(m_fdt);
}

void OctreeInterface::update() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyPressed(Keyboard::KEY_F1))
		CreateScene(scene, camera, 0);
	if (keyboard.keyPressed(Keyboard::KEY_F2))
		CreateScene(scene, camera, 1);
	if (keyboard.keyPressed(Keyboard::KEY_F3))
		CreateScene(scene, camera, 2);

	if (keyboard.keyPressed(Keyboard::KEY_4))
		drawDebug = !drawDebug;
	
	
	Vector3 directrion = Vector3();
	Vector3f _directrion = Vector3f();
	bool move = false;
	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3::FORWARD;
		_directrion += Vector3f(0.0f, 0.0f, 1.0f);
		move = true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3::BACK;
		_directrion += Vector3f(0.0f, 0.0f, -1.0f);
		move = true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3::LEFT;
		_directrion += Vector3f(1.0f, 0.0f, 0.0f);
		move = true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3::RIGHT;
		_directrion += Vector3f(-1.0f, 0.0f, 0.0f);
		move = true;
	}

	Mouse &mouse = Mouse::instance();
	float dx = 0.0f;
	float dy = 0.0f;

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {

			yaw += dx * 0.1f;
			pitch += dy * 0.1f;

			pitch = Clamp(pitch, -90.0f, 90.0f);

			camera->SetRotation(QuaternionTu(pitch, yaw, 0.0f));
			camera->rotate(-dx, -dy);
		}

		if (move) {
			float moveSpeed = (keyboard.keyDown(Keyboard::KEY_LSHIFT) || keyboard.keyDown(Keyboard::KEY_RSHIFT)) ? 50.0f : 5.0f;

			camera->Translate(directrion * m_dt * moveSpeed);
			camera->move(-_directrion  * m_dt * moveSpeed);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	// Scene animation
	if (animate) {
		if (rotatingObjects.size()) {
			angle += 100.0f * m_dt;
			QuaternionTu rotQuat(angle, Vector3::ONE);
			for (auto it = rotatingObjects.begin(); it != rotatingObjects.end(); ++it)
				(*it)->SetRotation(rotQuat);

		}
		else if (animatingObjects.size()) {
			for (auto it = animatingObjects.begin(); it != animatingObjects.end(); ++it) {
				AnimatedModel* object = *it;
				AnimationState* state = object->AnimationStates()[0];
				state->AddTime(m_dt);
				object->Translate(Vector3::FORWARD * 2.0f * m_dt);

				// Rotate to avoid going outside the plane
				Vector3 pos = object->Position();
				if (pos.x < -45.0f || pos.x > 45.0f || pos.z < -45.0f || pos.z > 45.0f)
					object->Yaw(45.0f * m_dt);
			}
		}
	}

	++frameNumber;
	if (!frameNumber)
		++frameNumber;

	std::cout << "#################" << std::endl;

	updateOctree();
}

void OctreeInterface::renderDirect() {



	debugRenderer->SetView(camera);


	glClearColor(DEFAULT_FOG_COLOR.r, DEFAULT_FOG_COLOR.g, DEFAULT_FOG_COLOR.b, DEFAULT_FOG_COLOR.a);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	UpdateInstanceTransforms(instanceTransforms);
	RenderBatches(camera, opaqueBatches);

	if (drawDebug) {
		DebugRenderer* debug = Subsystem<DebugRenderer>();
		for (size_t i = 0; i < rootLevelOctants.size(); ++i) {
			const ThreadOctantResult& result = octantResults[i];

			for (auto oIt = result.octants.begin(); oIt != result.octants.end(); ++oIt) {
				Octant* octant = oIt->first;
				octant->OnRenderDebug(debug);
				const std::vector<Drawable*>& drawables = octant->Drawables();

				for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt) {
					Drawable* drawable = *dIt;
					if (drawable->TestFlag(DF_GEOMETRY) && drawable->LastFrameNumber() == frameNumber)
						drawable->OnRenderDebug(debug);
				}
			}
		}
	}

	debugRenderer->Render();
}

void OctreeInterface::render() {
	
	renderDirect();

	if (m_drawUi)
		renderUi();
}

void OctreeInterface::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void OctreeInterface::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void OctreeInterface::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void OctreeInterface::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void OctreeInterface::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void OctreeInterface::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	camera->SetAspectRatio((float)Application::Width / (float)Application::Height);
}


void OctreeInterface::renderUi() {
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
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OctreeInterface::CreateScene(Scene* scene, CameraTu* camera, int preset) {
	rotatingObjects.clear();
	animatingObjects.clear();

	ResourceCache* cache = ObjectTu::Subsystem<ResourceCache>();

	scene->Clear();
	scene->CreateChild<Octree>("octree");
	m_octree = scene->FindChild<Octree>();
	//m_octree = new Octree();
	LightEnvironment* lightEnvironment = scene->CreateChild<LightEnvironment>();
	SetRandomSeed(1);

	// Preset 0: occluders, static meshes and many local shadowcasting lights in addition to ambient light
	if (preset == 0)
	{
		lightEnvironment->SetAmbientColor(Color(0.3f, 0.3f, 0.3f));
		camera->SetFarClip(1000.0f);


		{
			StaticModel* model = new StaticModel();
			m_octree->QueueUpdate(model->GetDrawable());
			model->SetStatic(true);
			model->SetPosition(Vector3(0, -0.05f, 0));
			model->SetScale(Vector3(100.0f, 0.1f, 100.0f));
			model->SetModel(cache->LoadResource<Model>("Box.mdl"));
			model->SetMaterial(cache->LoadResource<MaterialTu>("Stone.json"));
		}

		{
			AnimatedModel* object = new AnimatedModel();
			m_octree->QueueUpdate(object->GetDrawable());
			static_cast<AnimatedModelDrawable*>(object->GetDrawable())->SetOctree(m_octree);

			object->SetStatic(true);
			object->SetPosition(Vector3(Random() * 90.0f - 45.0f, 0.0f, Random() * 90.0f - 45.0f));
			object->SetRotation(QuaternionTu(Random(360.0f), Vector3::UP));
			object->SetModel(cache->LoadResource<Model>("Jack.mdl"));
			object->SetCastShadows(true);
			object->SetMaxDistance(600.0f);
			AnimationState* state = object->AddAnimationState(cache->LoadResource<Animation>("Jack_Walk.ani"));
			state->SetWeight(1.0f);
			state->SetLooped(true);
			animatingObjects.push_back(object);
		}
	}
	// Preset 1: high number of animating cubes
	else if (preset == 1)
	{
		lightEnvironment->SetFogColor(Color(0.3f, 0.3f, 0.3f));
		lightEnvironment->SetFogStart(300.0f);
		lightEnvironment->SetFogEnd(500.0f);
		camera->SetFarClip(500.0f);

		SharedPtr<MaterialTu> customMat = MaterialTu::DefaultMaterial()->Clone();
		customMat->SetUniform("matDiffColor", Vector4(0.75f, 0.35f, 0.0f, 1.0f));
		customMat->SetUniform("matSpecColor", Vector4(0.75f / 3.0f, 0.35f / 3.0f, 0.0f, 1.0f));

		for (int y = -125; y <= 125; ++y)
		{
			for (int x = -125; x <= 125; ++x)
			{
				StaticModel* object = scene->CreateChild<StaticModel>();
				//object->SetStatic(true);
				object->SetPosition(Vector3(x * 0.3f, 0.0f, y * 0.3f));
				object->SetScale(0.25f);
				object->SetModel(cache->LoadResource<Model>("Box.mdl"));
				object->SetMaterial(customMat);
				rotatingObjects.push_back(object);
			}
		}

		Light* light = scene->CreateChild<Light>();
		light->SetLightType(LIGHT_DIRECTIONAL);
		light->SetColor(Color(1.0f, 1.0f, 1.0f, 0.5f));
		light->SetRotation(QuaternionTu(45.0f, 45.0f, 0.0f));
	}
	// Preset 2: skinned characters with directional light shadows
	else if (preset == 2)
	{
		lightEnvironment->SetFogColor(Color(0.5f, 0.5f, 0.75f));
		lightEnvironment->SetFogStart(300.0f);
		lightEnvironment->SetFogEnd(500.0f);
		camera->SetFarClip(500.0f);

		{
			StaticModel* object = scene->CreateChild<StaticModel>();
			object->SetStatic(true);
			object->SetPosition(Vector3(0, -0.05f, 0));
			object->SetScale(Vector3(100.0f, 0.1f, 100.0f));
			object->SetModel(cache->LoadResource<Model>("Box.mdl"));
			object->SetMaterial(cache->LoadResource<MaterialTu>("Stone.json"));
		}

		for (int i = 0; i < 500; ++i)
		{
			AnimatedModel* object = scene->CreateChild<AnimatedModel>();
			object->SetStatic(true);
			object->SetPosition(Vector3(Random() * 90.0f - 45.0f, 0.0f, Random() * 90.0f - 45.0f));
			object->SetRotation(QuaternionTu(Random(360.0f), Vector3::UP));
			object->SetModel(cache->LoadResource<Model>("Jack.mdl"));
			object->SetCastShadows(true);
			object->SetMaxDistance(600.0f);
			AnimationState* state = object->AddAnimationState(cache->LoadResource<Animation>("Jack_Walk.ani"));
			state->SetWeight(1.0f);
			state->SetLooped(true);
			animatingObjects.push_back(object);
		}

		Light* light = scene->CreateChild<Light>();
		light->SetLightType(LIGHT_DIRECTIONAL);
		light->SetCastShadows(true);
		light->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		light->SetRotation(QuaternionTu(45.0f, 45.0f, 0.0f));
		light->SetShadowMapSize(2048);
		light->SetShadowMaxDistance(100.0f);
	}
}

void OctreeInterface::updateOctree() {

	frustum = camera->WorldFrustum();
	viewMask = camera->ViewMask();
	rootLevelOctants.clear();
	opaqueBatches.Clear();
	instanceTransforms.clear();
	geometryBounds.Undefine();

	for (size_t i = 0; i < NUM_OCTANT_TASKS; ++i)
		octantResults[i].Clear();

	for (size_t i = 0; i < workQueue->NumThreads(); ++i)
		batchResults[i].Clear();

	// Process moved / animated objects' octree reinsertions
	m_octree->Update(frameNumber);
	m_octree->FinishUpdate();

	// Enable threaded update during geometry / light gathering in case nodes' OnPrepareRender() causes further reinsertion queuing
	m_octree->SetThreadedUpdate(workQueue->NumThreads() > 1);

	// Find the starting points for octree traversal. Include the root if it contains drawables that didn't fit elsewhere
	Octant* rootOctant = m_octree->Root();
	if (rootOctant->Drawables().size()) {
		rootLevelOctants.push_back(rootOctant);
	}

	for (size_t i = 0; i < NUM_OCTANTS; ++i) {
		if (rootOctant->Child(i)) {
			rootLevelOctants.push_back(rootOctant->Child(i));
		}
	}

	numPendingBatchTasks.store((int)rootLevelOctants.size());

	for (size_t i = 0; i < rootLevelOctants.size(); ++i) {
		collectOctantsTasks[i]->startOctant = rootLevelOctants[i];
	}

	workQueue->QueueTasks(rootLevelOctants.size(), reinterpret_cast<Task**>(&collectOctantsTasks[0]));

	while (numPendingBatchTasks.load() > 0)
		workQueue->TryComplete();

	SortMainBatches();
	workQueue->Complete();
	m_octree->SetThreadedUpdate(false);
}

void OctreeInterface::ThreadOctantResult::Clear() {
	drawableAcc = 0;
	taskOctantIdx = 0;
	batchTaskIdx = 0;
	octants.clear();
	occlusionQueries.clear();
}

void OctreeInterface::ThreadBatchResult::Clear(){
	minZ = M_MAX_FLOAT;
	maxZ = 0.0f;
	geometryBounds.Undefine();
	opaqueBatches.clear();
}

void OctreeInterface::RegisterRendererLibrary() {
	static bool registered = false;
	if (registered)
		return;

	// Scene node base attributes are needed
	RegisterSceneLibrary();
	Octree::RegisterObject();
	Bone::RegisterObject();
	CameraTu::RegisterObject();
	OctreeNode::RegisterObject();
	GeometryNode::RegisterObject();
	StaticModel::RegisterObject();
	AnimatedModel::RegisterObject();
	Light::RegisterObject();
	LightEnvironment::RegisterObject();
	MaterialTu::RegisterObject();
	Model::RegisterObject();
	Animation::RegisterObject();

	registered = true;
}

void OctreeInterface::RenderBatches(CameraTu* camera_, const BatchQueue& queue) {
	//fill in uniforms
	float nearClip = camera->NearClip();
	float farClip = camera->FarClip();
	perViewData.projectionMatrix = camera->ProjectionMatrix();
	perViewData.viewMatrix = camera->ViewMatrix();
	perViewData.viewProjMatrix = camera->ProjectionMatrix() * camera->ViewMatrix();
	perViewData.depthParameters = Vector4(nearClip, farClip, camera->IsOrthographic() ? 0.5f : 0.0f, camera->IsOrthographic() ? 0.5f : 1.0f / farClip);
	perViewData.cameraPosition = Vector4(camera->WorldPosition(), 1.0f);
	perViewData.ambientColor = DEFAULT_AMBIENT_COLOR;

	Matrix4 view = Matrix4(camera->ViewMatrix());
	Matrix4f _view = camera->m_viewMatrix;

	Matrix4 projection = camera->ProjectionMatrix();
	Matrix4f _projection = camera->m_persMatrix;
	//projection.print();
	//_projection.print();

	perViewData.view = camera->m_viewMatrix;
	perViewData.projection = camera->m_persMatrix;
	//perViewData.projection = camera->ProjectionMatrix();

	perViewData.fogColor = DEFAULT_FOG_COLOR;
	float fogStart = DEFAULT_FOG_START;
	float fogEnd = DEFAULT_FOG_END;
	float fogRange = Max(fogEnd - fogStart, M_EPSILON);
	perViewData.fogParameters = Vector4(fogEnd / farClip, farClip / fogRange, 0.0f, 0.0f);
	perViewData.dirLightDirection = Vector4::ZERO;
	perViewData.dirLightColor = Color::BLACK;
	perViewData.dirLightShadowParameters = Vector4::ONE;
	perViewDataBuffer->SetData(0, sizeof(PerViewUniforms), &perViewData);

	perViewDataBuffer->Bind(UB_PERVIEWDATA);

	//std::cout << "Size: " << queue.batches.size() << std::endl;

	for (auto it = queue.batches.begin(); it != queue.batches.end(); ++it){

		std::cout << "------------" << std::endl;

		const Batch& batch = *it;
		unsigned char geometryBits = batch.programBits & SP_GEOMETRYBITS;

		ShaderProgram* program = batch.pass->GetShaderProgram(batch.programBits);
		if (!program->Bind())
			continue;

		MaterialTu* material = batch.pass->Parent();
		
		for (size_t i = 0; i < MAX_MATERIAL_TEXTURE_UNITS; ++i){
			TextureTu* texture = material->GetTexture(i);
			if (texture)
				texture->Bind(i);
		}

		UniformBuffer* materialUniforms = material->GetUniformBuffer();
		if (materialUniforms)
			materialUniforms->Bind(UB_MATERIALDATA);

		CullMode cullMode = material->GetCullMode();		
		graphics->SetRenderState(batch.pass->GetBlendMode(), cullMode, batch.pass->GetDepthTest(), batch.pass->GetColorWrite(), batch.pass->GetDepthWrite());

		Geometry* geometry = batch.geometry;
		VertexBuffer* vb = geometry->vertexBuffer;
		IndexBuffer* ib = geometry->indexBuffer;
		vb->Bind(program->Attributes());
		if (ib)
			ib->Bind();

		if (geometryBits == GEOM_INSTANCED){
			/*if (ib)
				graphics->DrawIndexedInstanced(PT_TRIANGLE_LIST, geometry->drawStart, geometry->drawCount, instanceVertexBuffer, batch.instanceStart, batch.instanceCount);
			else
				graphics->DrawInstanced(PT_TRIANGLE_LIST, geometry->drawStart, geometry->drawCount, instanceVertexBuffer, batch.instanceStart, batch.instanceCount);

			it += batch.instanceCount - 1;*/
		}else{

			if (!geometryBits) {
				//graphics->SetUniform(program, U_WORLDMATRIX, *batch.worldTransform);
				graphics->loadMatrix(program, "worldMatrix", *batch.worldTransform);
				graphics->loadMatrix(program, "worldMatrix4", Matrix4(*batch.worldTransform));
			}else
				batch.drawable->OnRender(program, batch.geomIndex);

			if (ib)
				graphics->DrawIndexed(PT_TRIANGLE_LIST, geometry->drawStart, geometry->drawCount);
			else
				graphics->Draw(PT_TRIANGLE_LIST, geometry->drawStart, geometry->drawCount);
		}
	}

}

void OctreeInterface::CollectOctants(Octant* octant, ThreadOctantResult& result, unsigned char planeMask) {
	const BoundingBox& octantBox = octant->CullingBox();

	if (planeMask) {
		// If not already inside all frustum planes, do frustum test and terminate if completely outside
		planeMask = frustum.IsInsideMasked(octantBox, planeMask);
		//planeMask = 0x00;
		if (planeMask == 0xff) {
			// If octant becomes frustum culled, reset its visibility for when it comes back to view, including its children
			if (octant->Visibility() != VIS_OUTSIDE_FRUSTUM)
				octant->SetVisibility(VIS_OUTSIDE_FRUSTUM, true);
			return;
		}
	}

	octant->SetVisibility(VIS_VISIBLE, false);

	const std::vector<Drawable*>& drawables = octant->Drawables();

	for (auto it = drawables.begin(); it != drawables.end(); ++it) {
		Drawable* drawable = *it;

		if (!drawable->TestFlag(DF_LIGHT)) {
			result.octants.push_back(std::make_pair(octant, planeMask));
			result.drawableAcc += drawables.end() - it;
			break;
		}
	}

	if (result.drawableAcc >= DRAWABLES_PER_BATCH_TASK) {
		if (result.collectBatchesTasks.size() <= result.batchTaskIdx)
			result.collectBatchesTasks.push_back(new CollectBatchesTask(this, &OctreeInterface::CollectBatchesWork));

		CollectBatchesTask* batchTask = result.collectBatchesTasks[result.batchTaskIdx];
		batchTask->octants.clear();
		batchTask->octants.insert(batchTask->octants.end(), result.octants.begin() + result.taskOctantIdx, result.octants.end());
		numPendingBatchTasks.fetch_add(1);
		workQueue->QueueTask(batchTask);

		result.drawableAcc = 0;
		result.taskOctantIdx = result.octants.size();
		++result.batchTaskIdx;
	}

	if (octant != m_octree->Root() && octant->HasChildren()) {
		for (size_t i = 0; i < NUM_OCTANTS; ++i){
			if (octant->Child(i))
				CollectOctants(octant->Child(i), result, planeMask);
		}
	}
}

void OctreeInterface::SortMainBatches() {

	for (size_t i = 0; i < workQueue->NumThreads(); ++i){
		ThreadBatchResult& res = batchResults[i];
		if (res.geometryBounds.IsDefined())
			geometryBounds.Merge(res.geometryBounds);
	}

	// Join per-thread collected batches and sort
	for (size_t i = 0; i < workQueue->NumThreads(); ++i) {
		ThreadBatchResult& res = batchResults[i];
		if (res.opaqueBatches.size())
			opaqueBatches.batches.insert(opaqueBatches.batches.end(), res.opaqueBatches.begin(), res.opaqueBatches.end());

	}

	opaqueBatches.Sort(instanceTransforms, SORT_STATE_AND_DISTANCE, hasInstancing);
}


void OctreeInterface::CollectOctantsWork(Task* task_, unsigned threadIndex) {

	CollectOctantsTask* task = static_cast<CollectOctantsTask*>(task_);
	Octant* octant = task->startOctant;
	ThreadOctantResult& result = octantResults[task->resultIdx];

	CollectOctants(octant, result);

	if (result.drawableAcc) {
		if (result.collectBatchesTasks.size() <= result.batchTaskIdx)
			result.collectBatchesTasks.push_back(new CollectBatchesTask(this, &OctreeInterface::CollectBatchesWork));

		CollectBatchesTask* batchTask = result.collectBatchesTasks[result.batchTaskIdx];
		batchTask->octants.clear();
		batchTask->octants.insert(batchTask->octants.end(), result.octants.begin() + result.taskOctantIdx, result.octants.end());
		numPendingBatchTasks.fetch_add(1);

		workQueue->QueueTask(batchTask);
	}

	numPendingBatchTasks.fetch_add(-1);
}

void OctreeInterface::CollectBatchesWork(Task* task_, unsigned threadIndex) {

	CollectBatchesTask* task = static_cast<CollectBatchesTask*>(task_);
	ThreadBatchResult& result = batchResults[threadIndex];
	bool threaded = workQueue->NumThreads() > 1;

	std::vector<std::pair<Octant*, unsigned char> >& octants = task->octants;
	std::vector<Batch>& opaqueQueue = threaded ? result.opaqueBatches : opaqueBatches.batches;


	const Matrix3x4& viewMatrix = camera->ViewMatrix();
	Vector3 viewZ = Vector3(viewMatrix.m20, viewMatrix.m21, viewMatrix.m22);
	Vector3 absViewZ = viewZ.Abs();
	float farClipMul = 32767.0f / camera->FarClip();

	// Scan octants for geometries
	for (auto it = octants.begin(); it != octants.end(); ++it){
		Octant* octant = it->first;
		unsigned char planeMask = it->second;
		const std::vector<Drawable*>& drawables = octant->Drawables();

		for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt){
			Drawable* drawable = *dIt;

			if (drawable->TestFlag(DF_GEOMETRY) && (drawable->LayerMask() & viewMask)){
				const BoundingBox& geometryBox = drawable->WorldBoundingBox();

				// Note: to strike a balance between performance and occlusion accuracy, per-geometry occlusion tests are skipped for now,
				// as octants are already tested with combined actual drawable bounds
				if ((!planeMask || frustum.IsInsideMaskedFast(geometryBox, planeMask)) && drawable->OnPrepareRender(frameNumber, camera)){

					result.geometryBounds.Merge(geometryBox);

					Vector3 center = geometryBox.Center();
					Vector3 edge = geometryBox.Size() * 0.5f;

					float viewCenterZ = viewZ.DotProduct(center) + viewMatrix.m23;
					float viewEdgeZ = absViewZ.DotProduct(edge);
					result.minZ = Min(result.minZ, viewCenterZ - viewEdgeZ);
					result.maxZ = Max(result.maxZ, viewCenterZ + viewEdgeZ);

					Batch newBatch;

					unsigned short distance = (unsigned short)(drawable->Distance() * farClipMul);
					const SourceBatches& batches = static_cast<GeometryDrawable*>(drawable)->Batches();
					size_t numGeometries = batches.NumGeometries();

					for (size_t j = 0; j < numGeometries; ++j){
						MaterialTu* material = batches.GetMaterial(j);

						// Assume opaque first
						newBatch.pass = material->GetPass(PASS_OPAQUE);
						newBatch.geometry = batches.GetGeometry(j);
						newBatch.programBits = (unsigned char)(drawable->Flags() & DF_GEOMETRY_TYPE_BITS);
						newBatch.geomIndex = (unsigned char)j;

						if (!newBatch.programBits)
							newBatch.worldTransform = &drawable->WorldTransform();
						else
							newBatch.drawable = static_cast<GeometryDrawable*>(drawable);

						if (newBatch.pass){
							// Perform distance sort in addition to state sort
							if (newBatch.pass->lastSortKey.first != frameNumber || newBatch.pass->lastSortKey.second > distance){
								newBatch.pass->lastSortKey.first = frameNumber;
								newBatch.pass->lastSortKey.second = distance;
							}

							if (newBatch.geometry->lastSortKey.first != frameNumber || newBatch.geometry->lastSortKey.second > distance + (unsigned short)j){
								newBatch.geometry->lastSortKey.first = frameNumber;
								newBatch.geometry->lastSortKey.second = distance + (unsigned short)j;
							}

							opaqueQueue.push_back(newBatch);
						}

					}
				}
			}
		}
	}

	numPendingBatchTasks.fetch_add(-1);
}


void OctreeInterface::UpdateInstanceTransforms(const std::vector<Matrix3x4>& transforms){
	if (hasInstancing && transforms.size()){
		if (instanceVertexBuffer->NumVertices() < transforms.size())
			instanceVertexBuffer->Define(USAGE_DYNAMIC, transforms.size(), instanceVertexElements, &transforms[0]);
		else
			instanceVertexBuffer->SetData(0, transforms.size(), &transforms[0]);
	}
}
