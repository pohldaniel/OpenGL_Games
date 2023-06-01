#include <time.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Culling.h"
#include "Constants.h"
#include "Application.h"

Culling::Culling(StateMachine& machine) : State(machine, CurrentState::SHAPEINTERFACE) {
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 1.5f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-2.5f);
	applyTransformation(m_trackball);

	Globals::shaderManager.loadShader("culling", "glsl120shader.vs", "glsl120shader.fs");
	ViewMatrix = mat4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	Wireframe = false;
	RenderAABB = false;
	RenderTree2D = false;
	VisualizeRenderingOrder = false;
	SortVisibleGeometryNodes = true;
	VisibilityCheckingPerformanceTest = false;

	Depth = -1;

	m_camera2.SetPerspective(45.0f, (float)Application::Width / (float)Application::Height, 0.125f, 1024.0f);
	CalculateProjectionMatrix();

	if (m_terrain.LoadBinary("terrain1.bin")) { 
		CalculateProjectionMatrix(); 
		vec3 Movement; 
		CheckCameraTerrainPosition(Movement); 
		m_camera2.Move(Movement); 
	}

	float Height = m_terrain.GetHeight(0.0f, 0.0f);
	m_camera2.Look(vec3(0.0f, Height + 1.75f, 0.0f), vec3(0.0f, Height + 1.75f, -1.0f));
}

Culling::~Culling() {

}

void Culling::fixedUpdate() {

}

void Culling::update() {
	BYTE Keys = 0x00;

	if (GetKeyState('W') & 0x80) Keys |= 0x01;
	if (GetKeyState('S') & 0x80) Keys |= 0x02;
	if (GetKeyState('A') & 0x80) Keys |= 0x04;
	if (GetKeyState('D') & 0x80) Keys |= 0x08;
	// if(GetKeyState('R') & 0x80) Keys |= 0x10;
	// if(GetKeyState('F') & 0x80) Keys |= 0x20;

	if (GetKeyState(VK_SHIFT) & 0x80) Keys |= 0x40;
	if (GetKeyState(VK_CONTROL) & 0x80) Keys |= 0x80;

	if (Keys & 0x3F){
		vec3 Movement = m_camera2.OnKeys(Keys, m_dt * 2.0f);

		CheckCameraTerrainPosition(Movement);

		m_camera2.Move(Movement);
	}

	Mouse &mouse = Mouse::instance();
	float dx = mouse.xPosRelative();
	float dy = mouse.yPosRelative();


	if (dx || dy) {
		m_camera2.OnMouseMove(-dx, -dy);
	}
};

void Culling::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!VisibilityCheckingPerformanceTest) {
		if (!RenderTree2D) {

			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(&m_camera2.ProjectionMatrix);

			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(&m_camera2.ViewMatrix);

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);

		}else {
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(&ProjectionMatrix);

			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(&ViewMatrix);
		}
	}

	int TrianglesRendered = m_terrain.CheckVisibility(m_camera2.Frustum, SortVisibleGeometryNodes);

	if (!VisibilityCheckingPerformanceTest) {
		if (!RenderTree2D && Wireframe) {
			TrianglesRendered *= 2;
		}
	}

	//Text.Set("Triangles rendered: %d", TrianglesRendered);

	if (!VisibilityCheckingPerformanceTest) {
		if (!RenderTree2D && Wireframe) {
			glColor3f(0.0f, 0.0f, 0.0f);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			m_terrain.Render();

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glColor3f(1.0f, 1.0f, 1.0f);

		auto shader = Globals::shaderManager.getAssetPointer("culling");
		shader->use();
		shader->loadVector("CameraPosition", !RenderTree2D ? Vector3f(m_camera2.Position.x, m_camera2.Position.y, m_camera2.Position.z) : Vector3f(0.0f, 4096.0f, 0.0f));
		m_terrain.Render(VisualizeRenderingOrder);
		shader->unuse();

		if (RenderAABB){
			glColor3f(0.0f, 1.0f, 0.0f);

			m_terrain.RenderAABB(Depth);
		}

		if (!RenderTree2D) {
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
		}
		else
		{
			glColor3f(1.0f, 0.5f, 0.25f);

			m_camera2.Frustum.Render();
		}
	}
	renderUi();
}

void Culling::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Culling::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}
	else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Culling::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}
	else if (event.button == 2u) {
		Mouse::instance().detach();
	}
}


void Culling::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}

void Culling::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void Culling::renderUi() {
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
	ImGui::Checkbox("Draw AABB", &RenderAABB);
	ImGui::Checkbox("Draw Tree 2D", &RenderTree2D);
	ImGui::Checkbox("Draworder", &VisualizeRenderingOrder);
	ImGui::Checkbox("Sort Nodes", &SortVisibleGeometryNodes);
	ImGui::Checkbox("Performancetest", &VisibilityCheckingPerformanceTest);
	

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Culling::CalculateProjectionMatrix()
{
	float Aspect = (float)Application::Width / (float)Application::Height;

	vec3 Min = m_terrain.GetMin(), Max = m_terrain.GetMax();

	ProjectionMatrix = ortho(Min.x * Aspect, Max.x * Aspect, -Max.z, -Min.z, -4096.0f, 4096.0f);
}

void Culling::CheckCameraTerrainPosition(vec3 &Movement)
{
	vec3 CameraPosition = m_camera2.Reference + Movement, Min = m_terrain.GetMin(), Max = m_terrain.GetMax();

	if (CameraPosition.x < Min.x) Movement += vec3(Min.x - CameraPosition.x, 0.0f, 0.0f);
	if (CameraPosition.x > Max.x) Movement += vec3(Max.x - CameraPosition.x, 0.0f, 0.0f);
	if (CameraPosition.z < Min.z) Movement += vec3(0.0f, 0.0f, Min.z - CameraPosition.z);
	if (CameraPosition.z > Max.z) Movement += vec3(0.0f, 0.0f, Max.z - CameraPosition.z);

	CameraPosition = m_camera2.Reference + Movement;

	float Height = m_terrain.GetHeight(CameraPosition.x, CameraPosition.z);

	Movement += vec3(0.0f, Height + 1.75f - m_camera2.Reference.y, 0.0f);
}

CString ModuleDirectory, ErrorLog;

CPlane::CPlane()
{
}

CPlane::~CPlane()
{
}

void CPlane::Set(const vec3 &A, const vec3 &B, const vec3 &C)
{
	N = normalize(cross(B - A, C - A));
	ND = dot(N, A);
	O = N.z < 0.0f ? (N.y < 0.0f ? (N.x < 0.0f ? 0 : 1) : (N.x < 0.0f ? 2 : 3)) : (N.y < 0.0f ? (N.x < 0.0f ? 4 : 5) : (N.x < 0.0f ? 6 : 7));
}

bool CPlane::AABBBehind(const vec3 *AABBVertices)
{
	return dot(N, AABBVertices[O]) < ND;
}

float CPlane::AABBDistance(const vec3 *AABBVertices)
{
	return dot(N, AABBVertices[O]);
}

// ----------------------------------------------------------------------------------------------------------------------------

CFrustum::CFrustum()
{
}

CFrustum::~CFrustum()
{
}

void CFrustum::Set(const mat4x4 &ViewProjectionMatrixInverse)
{
	vec4 A = ViewProjectionMatrixInverse * vec4(-1.0f, -1.0f, 1.0f, 1.0f);
	vec4 B = ViewProjectionMatrixInverse * vec4(1.0f, -1.0f, 1.0f, 1.0f);
	vec4 C = ViewProjectionMatrixInverse * vec4(-1.0f, 1.0f, 1.0f, 1.0f);
	vec4 D = ViewProjectionMatrixInverse * vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 E = ViewProjectionMatrixInverse * vec4(-1.0f, -1.0f, -1.0f, 1.0f);
	vec4 F = ViewProjectionMatrixInverse * vec4(1.0f, -1.0f, -1.0f, 1.0f);
	vec4 G = ViewProjectionMatrixInverse * vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	vec4 H = ViewProjectionMatrixInverse * vec4(1.0f, 1.0f, -1.0f, 1.0f);

	Vertices[0] = vec3(A.x / A.w, A.y / A.w, A.z / A.w);
	Vertices[1] = vec3(B.x / B.w, B.y / B.w, B.z / B.w);
	Vertices[2] = vec3(C.x / C.w, C.y / C.w, C.z / C.w);
	Vertices[3] = vec3(D.x / D.w, D.y / D.w, D.z / D.w);
	Vertices[4] = vec3(E.x / E.w, E.y / E.w, E.z / E.w);
	Vertices[5] = vec3(F.x / F.w, F.y / F.w, F.z / F.w);
	Vertices[6] = vec3(G.x / G.w, G.y / G.w, G.z / G.w);
	Vertices[7] = vec3(H.x / H.w, H.y / H.w, H.z / H.w);

	Planes[0].Set(Vertices[4], Vertices[0], Vertices[2]);
	Planes[1].Set(Vertices[1], Vertices[5], Vertices[7]);
	Planes[2].Set(Vertices[4], Vertices[5], Vertices[1]);
	Planes[3].Set(Vertices[2], Vertices[3], Vertices[7]);
	Planes[4].Set(Vertices[0], Vertices[1], Vertices[3]);
	Planes[5].Set(Vertices[5], Vertices[4], Vertices[6]);
}

bool CFrustum::AABBVisible(const vec3 *AABBVertices)
{
	for (int i = 0; i < 6; i++)
	{
		if (Planes[i].AABBBehind(AABBVertices))
		{
			return false;
		}
	}

	return true;
}

float CFrustum::AABBDistance(const vec3 *AABBVertices)
{
	return Planes[5].AABBDistance(AABBVertices);
}

void CFrustum::Render()
{
	glBegin(GL_LINES);

	glVertex3fv(&Vertices[0]); glVertex3fv(&Vertices[1]);
	glVertex3fv(&Vertices[2]); glVertex3fv(&Vertices[3]);
	glVertex3fv(&Vertices[4]); glVertex3fv(&Vertices[5]);
	glVertex3fv(&Vertices[6]); glVertex3fv(&Vertices[7]);

	glVertex3fv(&Vertices[0]); glVertex3fv(&Vertices[2]);
	glVertex3fv(&Vertices[1]); glVertex3fv(&Vertices[3]);
	glVertex3fv(&Vertices[4]); glVertex3fv(&Vertices[6]);
	glVertex3fv(&Vertices[5]); glVertex3fv(&Vertices[7]);

	glVertex3fv(&Vertices[0]); glVertex3fv(&Vertices[4]);
	glVertex3fv(&Vertices[1]); glVertex3fv(&Vertices[5]);
	glVertex3fv(&Vertices[2]); glVertex3fv(&Vertices[6]);
	glVertex3fv(&Vertices[3]); glVertex3fv(&Vertices[7]);

	glEnd();
}

// ----------------------------------------------------------------------------------------------------------------------------

CCamera::CCamera()
{
	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);

	CalculateViewMatrix();
}

CCamera::~CCamera()
{
}

void CCamera::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);

	GetXY(Z, X, Y);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position - Z * 0.05f;
	}

	CalculateViewMatrix();
}

void CCamera::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

vec3 CCamera::OnKeys(BYTE Keys, float FrameTime)
{
	float Speed = 5.0f;

	if (Keys & 0x40) Speed *= 2.0f;
	if (Keys & 0x80) Speed *= 0.5f;

	float Distance = Speed * FrameTime;

	vec3 Up(0.0f, 1.0f, 0.0f);
	vec3 Right = X;
	vec3 Forward = cross(Up, Right);

	Up *= Distance;
	Right *= Distance;
	Forward *= Distance;

	vec3 Movement;

	if (Keys & 0x01) Movement += Forward;
	if (Keys & 0x02) Movement -= Forward;
	if (Keys & 0x04) Movement -= Right;
	if (Keys & 0x08) Movement += Right;
	if (Keys & 0x10) Movement += Up;
	if (Keys & 0x20) Movement -= Up;

	return Movement;
}

void CCamera::OnMouseMove(int dx, int dy)
{
	float Sensitivity = 0.25f;

	Position -= Reference;

	if (dx != 0)
	{
		float DeltaX = (float)dx * Sensitivity;

		X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * Sensitivity;

		Y = rotate(Y, DeltaY, X);
		Z = rotate(Z, DeltaY, X);

		if (Y.y < 0.0f)
		{
			Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = cross(Z, X);
		}
	}

	Position = Reference + Z * length(Position);

	CalculateViewMatrix();
}

void CCamera::OnMouseWheel(float zDelta)
{
	Position -= Reference;

	if (zDelta < 0 && length(Position) < 500.0f)
	{
		Position += Position * 0.1f;
	}

	if (zDelta > 0 && length(Position) > 0.05f)
	{
		Position -= Position * 0.1f;
	}

	Position += Reference;

	CalculateViewMatrix();
}

void CCamera::SetPerspective(float fovy, float aspect, float n, float f)
{
	ProjectionMatrix = perspective(fovy, aspect, n, f);
	ProjectionMatrixInverse = inverse(ProjectionMatrix);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	ViewProjectionMatrixInverse = ViewMatrixInverse * ProjectionMatrixInverse;

	Frustum.Set(ViewProjectionMatrixInverse);
}

void CCamera::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	ViewProjectionMatrixInverse = ViewMatrixInverse * ProjectionMatrixInverse;

	Frustum.Set(ViewProjectionMatrixInverse);
}

// ----------------------------------------------------------------------------------------------------------------------------

CAABB::CAABB()
{
}

CAABB::~CAABB()
{
}

void CAABB::Set(const vec3 &Min, const vec3 &Max)
{
	Vertices[0] = vec3(Min.x, Min.y, Min.z);
	Vertices[1] = vec3(Max.x, Min.y, Min.z);
	Vertices[2] = vec3(Min.x, Max.y, Min.z);
	Vertices[3] = vec3(Max.x, Max.y, Min.z);
	Vertices[4] = vec3(Min.x, Min.y, Max.z);
	Vertices[5] = vec3(Max.x, Min.y, Max.z);
	Vertices[6] = vec3(Min.x, Max.y, Max.z);
	Vertices[7] = vec3(Max.x, Max.y, Max.z);
}

bool CAABB::PointInside(const vec3 &Point)
{
	if (Point.x < Vertices[0].x) return false;
	if (Point.y < Vertices[0].y) return false;
	if (Point.z < Vertices[0].z) return false;

	if (Point.x > Vertices[7].x) return false;
	if (Point.y > Vertices[7].y) return false;
	if (Point.z > Vertices[7].z) return false;

	return true;
}

bool CAABB::Visible(CFrustum &Frustum)
{
	return Frustum.AABBVisible(Vertices);
}

float CAABB::Distance(CFrustum &Frustum)
{
	return Frustum.AABBDistance(Vertices);
}

void CAABB::Render()
{
	glBegin(GL_LINES);

	glVertex3fv(&Vertices[0]); glVertex3fv(&Vertices[1]);
	glVertex3fv(&Vertices[2]); glVertex3fv(&Vertices[3]);
	glVertex3fv(&Vertices[4]); glVertex3fv(&Vertices[5]);
	glVertex3fv(&Vertices[6]); glVertex3fv(&Vertices[7]);

	glVertex3fv(&Vertices[0]); glVertex3fv(&Vertices[2]);
	glVertex3fv(&Vertices[1]); glVertex3fv(&Vertices[3]);
	glVertex3fv(&Vertices[4]); glVertex3fv(&Vertices[6]);
	glVertex3fv(&Vertices[5]); glVertex3fv(&Vertices[7]);

	glVertex3fv(&Vertices[0]); glVertex3fv(&Vertices[4]);
	glVertex3fv(&Vertices[1]); glVertex3fv(&Vertices[5]);
	glVertex3fv(&Vertices[2]); glVertex3fv(&Vertices[6]);
	glVertex3fv(&Vertices[3]); glVertex3fv(&Vertices[7]);

	glEnd();
}

// ----------------------------------------------------------------------------------------------------------------------------

CBSPTreeNode::CBSPTreeNode()
{
	SetDefaults();
}

CBSPTreeNode::~CBSPTreeNode()
{
}

void CBSPTreeNode::SetDefaults()
{
	Min = Max = vec3(0.0f);

	Depth = 0;

	Indices = NULL;

	IndicesCount = 0;

	IndexBufferObject = 0;

	Children[0] = NULL;
	Children[1] = NULL;
}

void CBSPTreeNode::InitAABB(const vec3 &Min, const vec3 &Max, int Depth, float MinAABBSize)
{
	this->Min = Min;
	this->Max = Max;

	this->Depth = Depth;

	vec3 Mid = (Min + Max) / 2.0f;
	vec3 Size = Max - Min;

	AABB.Set(Min, Max);

	if (Size.x > MinAABBSize || Size.z > MinAABBSize)
	{
		Children[0] = new CBSPTreeNode();
		Children[1] = new CBSPTreeNode();

		if (Size.x >= Size.z)
		{
			Children[0]->InitAABB(vec3(Min.x, Min.y, Min.z), vec3(Mid.x, Max.y, Max.z), Depth + 1, MinAABBSize);
			Children[1]->InitAABB(vec3(Mid.x, Min.y, Min.z), vec3(Max.x, Max.y, Max.z), Depth + 1, MinAABBSize);
		}
		else
		{
			Children[0]->InitAABB(vec3(Min.x, Min.y, Min.z), vec3(Max.x, Max.y, Mid.z), Depth + 1, MinAABBSize);
			Children[1]->InitAABB(vec3(Min.x, Min.y, Mid.z), vec3(Max.x, Max.y, Max.z), Depth + 1, MinAABBSize);
		}
	}
}

bool CBSPTreeNode::CheckTriangle(CVertex *Vertices, int *Indices, int A, int B, int C)
{
	if (AABB.PointInside(Vertices[Indices[A]].Position))
	{
		if (AABB.PointInside(Vertices[Indices[B]].Position))
		{
			if (AABB.PointInside(Vertices[Indices[C]].Position))
			{
				bool BelongsToAChild = false;

				if (Children[0] != NULL)
				{
					BelongsToAChild |= Children[0]->CheckTriangle(Vertices, Indices, A, B, C);
				}

				if (Children[1] != NULL && !BelongsToAChild)
				{
					BelongsToAChild |= Children[1]->CheckTriangle(Vertices, Indices, A, B, C);
				}

				if (!BelongsToAChild)
				{
					IndicesCount += 3;
				}

				return true;
			}
		}
	}

	return false;
}

void CBSPTreeNode::AllocateMemory()
{
	if (IndicesCount > 0)
	{
		Indices = new int[IndicesCount];
		IndicesCount = 0;
	}

	if (Children[0] != NULL)
	{
		Children[0]->AllocateMemory();
	}

	if (Children[1] != NULL)
	{
		Children[1]->AllocateMemory();
	}
}

bool CBSPTreeNode::AddTriangle(CVertex *Vertices, int *Indices, int A, int B, int C)
{
	if (AABB.PointInside(Vertices[Indices[A]].Position))
	{
		if (AABB.PointInside(Vertices[Indices[B]].Position))
		{
			if (AABB.PointInside(Vertices[Indices[C]].Position))
			{
				bool BelongsToAChild = false;

				if (Children[0] != NULL)
				{
					BelongsToAChild |= Children[0]->AddTriangle(Vertices, Indices, A, B, C);
				}

				if (Children[1] != NULL && !BelongsToAChild)
				{
					BelongsToAChild |= Children[1]->AddTriangle(Vertices, Indices, A, B, C);
				}

				if (!BelongsToAChild)
				{
					this->Indices[IndicesCount++] = Indices[A];
					this->Indices[IndicesCount++] = Indices[B];
					this->Indices[IndicesCount++] = Indices[C];
				}

				return true;
			}
		}
	}

	return false;
}

void CBSPTreeNode::ResetAABB(CVertex *Vertices)
{
	float MinY = Min.y, MaxY = Max.y;

	Min.y = MaxY;
	Max.y = MinY;

	if (IndicesCount > 0)
	{
		for (int i = 0; i < IndicesCount; i++)
		{
			if (Vertices[Indices[i]].Position.y < Min.y) Min.y = Vertices[Indices[i]].Position.y;
			if (Vertices[Indices[i]].Position.y > Max.y) Max.y = Vertices[Indices[i]].Position.y;
		}
	}

	if (Children[0] != NULL)
	{
		Children[0]->ResetAABB(Vertices);

		if (Children[0]->Min.y < Min.y) Min.y = Children[0]->Min.y;
		if (Children[0]->Max.y > Max.y) Max.y = Children[0]->Max.y;
	}

	if (Children[1] != NULL)
	{
		Children[1]->ResetAABB(Vertices);

		if (Children[1]->Min.y < Min.y) Min.y = Children[1]->Min.y;
		if (Children[1]->Max.y > Max.y) Max.y = Children[1]->Max.y;
	}

	AABB.Set(Min, Max);
}

int CBSPTreeNode::InitIndexBufferObject()
{
	int GeometryNodesCount = 0;

	if (IndicesCount > 0)
	{
		glGenBuffers(1, &IndexBufferObject);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndicesCount * sizeof(int), Indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		delete[] Indices;
		Indices = NULL;

		GeometryNodesCount++;
	}

	if (Children[0] != NULL)
	{
		GeometryNodesCount += Children[0]->InitIndexBufferObject();
	}

	if (Children[1] != NULL)
	{
		GeometryNodesCount += Children[1]->InitIndexBufferObject();
	}

	return GeometryNodesCount;
}

int CBSPTreeNode::CheckVisibility(CFrustum &Frustum, CBSPTreeNode **VisibleGeometryNodes, int &VisibleGeometryNodesCount)
{
	int TrianglesRendered = 0;

	Visible = AABB.Visible(Frustum);

	if (Visible)
	{
		if (IndicesCount > 0)
		{
			Distance = AABB.Distance(Frustum);

			VisibleGeometryNodes[VisibleGeometryNodesCount++] = this;

			TrianglesRendered += IndicesCount / 3;
		}

		if (Children[0] != NULL)
		{
			TrianglesRendered += Children[0]->CheckVisibility(Frustum, VisibleGeometryNodes, VisibleGeometryNodesCount);
		}

		if (Children[1] != NULL)
		{
			TrianglesRendered += Children[1]->CheckVisibility(Frustum, VisibleGeometryNodes, VisibleGeometryNodesCount);
		}
	}

	return TrianglesRendered;
}

float CBSPTreeNode::GetDistance()
{
	return Distance;
}

void CBSPTreeNode::Render()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferObject);

	glDrawElements(GL_TRIANGLES, IndicesCount, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CBSPTreeNode::RenderAABB(int Depth)
{
	if (Visible)
	{
		if (Depth == -1 || Depth == this->Depth)
		{
			AABB.Render();
		}

		if (Children[0] != NULL)
		{
			Children[0]->RenderAABB(Depth);
		}

		if (Children[1] != NULL)
		{
			Children[1]->RenderAABB(Depth);
		}
	}
}

void CBSPTreeNode::Destroy()
{
	if (Indices != NULL)
	{
		delete[] Indices;
	}

	if (IndexBufferObject != 0)
	{
		glDeleteBuffers(1, &IndexBufferObject);
	}

	if (Children[0] != NULL)
	{
		Children[0]->Destroy();
		delete Children[0];
	}

	if (Children[1] != NULL)
	{
		Children[1]->Destroy();
		delete Children[1];
	}

	SetDefaults();
}

// ----------------------------------------------------------------------------------------------------------------------------

CBSPTree::CBSPTree()
{
	SetDefaults();
}

CBSPTree::~CBSPTree()
{
}

void CBSPTree::SetDefaults()
{
	Root = NULL;

	VisibleGeometryNodes = NULL;
}

void CBSPTree::Init(CVertex *Vertices, int *Indices, int IndicesCount, const vec3 &Min, const vec3 &Max, float MinAABBSize)
{
	Destroy();

	if (Vertices != NULL && Indices != NULL && IndicesCount > 0)
	{
		Root = new CBSPTreeNode();

		Root->InitAABB(Min, Max, 0, MinAABBSize);

		for (int i = 0; i < IndicesCount; i += 3)
		{
			Root->CheckTriangle(Vertices, Indices, i, i + 1, i + 2);
		}

		Root->AllocateMemory();

		for (int i = 0; i < IndicesCount; i += 3)
		{
			Root->AddTriangle(Vertices, Indices, i, i + 1, i + 2);
		}

		Root->ResetAABB(Vertices);

		int GeometryNodesCount = Root->InitIndexBufferObject();

		VisibleGeometryNodes = new CBSPTreeNode*[GeometryNodesCount];
	}
}

void CBSPTree::QuickSortVisibleGeometryNodes(int Left, int Right)
{
	float Pivot = VisibleGeometryNodes[(Left + Right) / 2]->GetDistance();
	int i = Left, j = Right;

	while (i <= j)
	{
		while (VisibleGeometryNodes[i]->GetDistance() < Pivot) i++;
		while (VisibleGeometryNodes[j]->GetDistance() > Pivot) j--;

		if (i <= j)
		{
			if (i != j)
			{
				CBSPTreeNode *Temp = VisibleGeometryNodes[i];
				VisibleGeometryNodes[i] = VisibleGeometryNodes[j];
				VisibleGeometryNodes[j] = Temp;
			}

			i++;
			j--;
		}
	}

	if (Left < j)
	{
		QuickSortVisibleGeometryNodes(Left, j);
	}

	if (i < Right)
	{
		QuickSortVisibleGeometryNodes(i, Right);
	}
}

int CBSPTree::CheckVisibility(CFrustum &Frustum, bool SortVisibleGeometryNodes)
{
	int TrianglesRendered = 0;

	VisibleGeometryNodesCount = 0;

	if (Root != NULL)
	{
		TrianglesRendered = Root->CheckVisibility(Frustum, VisibleGeometryNodes, VisibleGeometryNodesCount);

		if (SortVisibleGeometryNodes)
		{
			if (VisibleGeometryNodesCount > 1)
			{
				QuickSortVisibleGeometryNodes(0, VisibleGeometryNodesCount - 1);
			}
		}
	}

	return TrianglesRendered;
}

void CBSPTree::Render(bool VisualizeRenderingOrder)
{
	if (VisibleGeometryNodesCount > 0)
	{
		if (!VisualizeRenderingOrder)
		{
			for (int i = 0; i < VisibleGeometryNodesCount; i++)
			{
				VisibleGeometryNodes[i]->Render();
			}
		}
		else
		{
			for (int i = 0; i < VisibleGeometryNodesCount; i++)
			{
				float Color = ((float)i / (float)VisibleGeometryNodesCount) + 0.2f;

				glColor3f(Color, Color, Color);

				VisibleGeometryNodes[i]->Render();
			}
		}
	}
}

void CBSPTree::RenderAABB(int Depth)
{
	if (Root != NULL)
	{
		Root->RenderAABB(Depth);
	}
}

void CBSPTree::Destroy()
{
	if (Root != NULL)
	{
		Root->Destroy();
		delete Root;
	}

	if (VisibleGeometryNodes != NULL)
	{
		delete[] VisibleGeometryNodes;
	}

	SetDefaults();
}

// ----------------------------------------------------------------------------------------------------------------------------

CTerrain::CTerrain()
{
	SetDefaults();
}

CTerrain::~CTerrain()
{
}

void CTerrain::SetDefaults()
{
	Size = 0;
	SizeP1 = 0;
	SizeD2 = 0.0f;

	Min = Max = vec3(0.0f);

	Heights = NULL;

	VerticesCount = 0;

	VertexBufferObject = 0;
}

/*bool CTerrain::LoadTexture2D(char *FileName, float Scale, float Offset)
{
	CTexture Texture;

	if (!Texture.LoadTexture2D(FileName))
	{
		return false;
	}

	if (Texture.GetWidth() != Texture.GetHeight())
	{
		ErrorLog.Append("Unsupported texture dimensions (%s)!\r\n", FileName);
		Texture.Destroy();
		return false;
	}

	Destroy();

	Size = Texture.GetWidth();
	SizeP1 = Size + 1;
	SizeD2 = (float)Size / 2.0f;

	VerticesCount = SizeP1 * SizeP1;

	float *TextureHeights = new float[Size * Size];

	glBindTexture(GL_TEXTURE_2D, Texture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_GREEN, GL_FLOAT, TextureHeights);
	glBindTexture(GL_TEXTURE_2D, 0);

	Texture.Destroy();

	for (int i = 0; i < Size * Size; i++)
	{
		TextureHeights[i] = TextureHeights[i] * Scale + Offset;
	}

	Heights = new float[VerticesCount];

	int i = 0;

	for (int z = 0; z <= Size; z++)
	{
		for (int x = 0; x <= Size; x++)
		{
			Heights[i++] = GetHeight(TextureHeights, Size, (float)x - 0.5f, (float)z - 0.5f);
		}
	}

	delete[] TextureHeights;

	float *SmoothedHeights = new float[VerticesCount];

	i = 0;

	for (int z = 0; z <= Size; z++)
	{
		for (int x = 0; x <= Size; x++)
		{
			SmoothedHeights[i] = 0.0f;

			SmoothedHeights[i] += GetHeight(x - 1, z + 1) + GetHeight(x, z + 1) * 2 + GetHeight(x + 1, z + 1);
			SmoothedHeights[i] += GetHeight(x - 1, z) * 2 + GetHeight(x, z) * 3 + GetHeight(x + 1, z) * 2;
			SmoothedHeights[i] += GetHeight(x - 1, z - 1) + GetHeight(x, z - 1) * 2 + GetHeight(x + 1, z - 1);

			SmoothedHeights[i] /= 15.0f;

			i++;
		}
	}

	delete[] Heights;

	Heights = SmoothedHeights;

	Min.x = Min.z = -SizeD2;
	Max.x = Max.z = SizeD2;

	Min.y = Max.y = Heights[0];

	for (int i = 1; i < VerticesCount; i++)
	{
		if (Heights[i] < Min.y) Min.y = Heights[i];
		if (Heights[i] > Max.y) Max.y = Heights[i];
	}

	CVertex *Vertices = new CVertex[VerticesCount];

	i = 0;

	for (int z = 0; z <= Size; z++)
	{
		for (int x = 0; x <= Size; x++)
		{
			Vertices[i].Position = vec3((float)x - SizeD2, Heights[i], SizeD2 - (float)z);
			Vertices[i].Normal = normalize(vec3(GetHeight(x - 1, z) - GetHeight(x + 1, z), 2.0f, GetHeight(x, z + 1) - GetHeight(x, z - 1)));

			i++;
		}
	}

	glGenBuffers(1, &VertexBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, VerticesCount * sizeof(CVertex), Vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	int IndicesCount = Size * Size * 2 * 3;

	int *Indices = new int[IndicesCount];

	i = 0;

	for (int z = 0; z < Size; z++)
	{
		for (int x = 0; x < Size; x++)
		{
			Indices[i++] = GetIndex(x, z);
			Indices[i++] = GetIndex(x + 1, z);
			Indices[i++] = GetIndex(x + 1, z + 1);

			Indices[i++] = GetIndex(x + 1, z + 1);
			Indices[i++] = GetIndex(x, z + 1);
			Indices[i++] = GetIndex(x, z);
		}
	}

	BSPTree.Init(Vertices, Indices, IndicesCount, Min, Max);

	delete[] Vertices;
	delete[] Indices;

	return true;
}*/

bool CTerrain::LoadBinary(char *FileName)
{
	CString DirectoryFileName = ModuleDirectory + FileName;

	FILE *File;

	if (fopen_s(&File, DirectoryFileName, "rb") != 0)
	{
		ErrorLog.Append("Error opening file " + DirectoryFileName + "!\r\n");
		return false;
	}

	int Size;

	if (fread(&Size, sizeof(int), 1, File) != 1 || Size <= 0)
	{
		ErrorLog.Append("Error reading file " + DirectoryFileName + "!\r\n");
		fclose(File);
		return false;
	}

	Destroy();

	this->Size = Size;
	SizeP1 = Size + 1;
	SizeD2 = (float)Size / 2.0f;

	VerticesCount = SizeP1 * SizeP1;

	Heights = new float[VerticesCount];

	if (fread(Heights, sizeof(float), VerticesCount, File) != VerticesCount)
	{
		ErrorLog.Append("Error reading file " + DirectoryFileName + "!\r\n");
		fclose(File);
		Destroy();
		return false;
	}

	fclose(File);

	Min.x = Min.z = -SizeD2;
	Max.x = Max.z = SizeD2;

	Min.y = Max.y = Heights[0];

	for (int i = 1; i < VerticesCount; i++)
	{
		if (Heights[i] < Min.y) Min.y = Heights[i];
		if (Heights[i] > Max.y) Max.y = Heights[i];
	}

	CVertex *Vertices = new CVertex[VerticesCount];

	int i = 0;

	for (int z = 0; z <= Size; z++)
	{
		for (int x = 0; x <= Size; x++)
		{
			Vertices[i].Position = vec3((float)x - SizeD2, Heights[i], SizeD2 - (float)z);
			Vertices[i].Normal = normalize(vec3(GetHeight(x - 1, z) - GetHeight(x + 1, z), 2.0f, GetHeight(x, z + 1) - GetHeight(x, z - 1)));

			i++;
		}
	}

	glGenBuffers(1, &VertexBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, VerticesCount * sizeof(CVertex), Vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	int IndicesCount = Size * Size * 2 * 3;

	int *Indices = new int[IndicesCount];

	i = 0;

	for (int z = 0; z < Size; z++)
	{
		for (int x = 0; x < Size; x++)
		{
			Indices[i++] = GetIndex(x, z);
			Indices[i++] = GetIndex(x + 1, z);
			Indices[i++] = GetIndex(x + 1, z + 1);

			Indices[i++] = GetIndex(x + 1, z + 1);
			Indices[i++] = GetIndex(x, z + 1);
			Indices[i++] = GetIndex(x, z);
		}
	}

	BSPTree.Init(Vertices, Indices, IndicesCount, Min, Max);

	delete[] Vertices;
	delete[] Indices;

	return true;
}

bool CTerrain::SaveBinary(char *FileName)
{
	CString DirectoryFileName = ModuleDirectory + FileName;

	FILE *File;

	if (fopen_s(&File, DirectoryFileName, "wb+") != 0)
	{
		return false;
	}

	fwrite(&Size, sizeof(int), 1, File);

	fwrite(Heights, sizeof(float), VerticesCount, File);

	fclose(File);

	return true;
}

int CTerrain::CheckVisibility(CFrustum &Frustum, bool SortVisibleGeometryNodes)
{
	return BSPTree.CheckVisibility(Frustum, SortVisibleGeometryNodes);
}

void CTerrain::Render(bool VisualizeRenderingOrder)
{
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(CVertex), (void*)(sizeof(vec3) * 0));

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, sizeof(CVertex), (void*)(sizeof(vec3) * 1));

	BSPTree.Render(VisualizeRenderingOrder);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CTerrain::RenderAABB(int Depth)
{
	BSPTree.RenderAABB(Depth);
}

void CTerrain::Destroy()
{
	if (Heights != NULL)
	{
		delete[] Heights;
	}

	if (VertexBufferObject != 0)
	{
		glDeleteBuffers(1, &VertexBufferObject);
	}

	BSPTree.Destroy();

	SetDefaults();
}

vec3 CTerrain::GetMin()
{
	return Min;
}

vec3 CTerrain::GetMax()
{
	return Max;
}

int CTerrain::GetIndex(int X, int Z)
{
	return SizeP1 * Z + X;
}

float CTerrain::GetHeight(int X, int Z)
{
	return Heights[GetIndex(X < 0 ? 0 : X > Size ? Size : X, Z < 0 ? 0 : Z > Size ? Size : Z)];
}

float CTerrain::GetHeight(float X, float Z)
{
	Z = -Z;

	X += SizeD2;
	Z += SizeD2;

	float Size = (float)this->Size;

	if (X < 0.0f) X = 0.0f;
	if (X > Size) X = Size;
	if (Z < 0.0f) Z = 0.0f;
	if (Z > Size) Z = Size;

	int ix = (int)X, ixp1 = ix + 1;
	int iz = (int)Z, izp1 = iz + 1;

	float fx = X - (float)ix;
	float fz = Z - (float)iz;

	float a = GetHeight(ix, iz);
	float b = GetHeight(ixp1, iz);
	float c = GetHeight(ix, izp1);
	float d = GetHeight(ixp1, izp1);

	float ab = a + (b - a) * fx;
	float cd = c + (d - c) * fx;

	return ab + (cd - ab) * fz;
}

float CTerrain::GetHeight(float *Heights, int Size, float X, float Z)
{
	float SizeM1F = (float)Size - 1.0f;

	if (X < 0.0f) X = 0.0f;
	if (X > SizeM1F) X = SizeM1F;
	if (Z < 0.0f) Z = 0.0f;
	if (Z > SizeM1F) Z = SizeM1F;

	int ix = (int)X, ixp1 = ix + 1;
	int iz = (int)Z, izp1 = iz + 1;

	int SizeM1 = Size - 1;

	if (ixp1 > SizeM1) ixp1 = SizeM1;
	if (izp1 > SizeM1) izp1 = SizeM1;

	float fx = X - (float)ix;
	float fz = Z - (float)iz;

	int izMSize = iz * Size, izp1MSize = izp1 * Size;

	float a = Heights[izMSize + ix];
	float b = Heights[izMSize + ixp1];
	float c = Heights[izp1MSize + ix];
	float d = Heights[izp1MSize + ixp1];

	float ab = a + (b - a) * fx;
	float cd = c + (d - c) * fx;

	return ab + (cd - ab) * fz;
}