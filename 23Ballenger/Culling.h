#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/Keyboard.h"
#include "engine/input/Mouse.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "engine/Shader.h"
#include "StateMachine.h"

#include "glmath.h"
#include "_string.h"


// ----------------------------------------------------------------------------------------------------------------------------

class CPlane
{
private:
	vec3 N;
	float ND;
	int O;

public:
	CPlane();
	~CPlane();

public:
	void Set(const vec3 &A, const vec3 &B, const vec3 &C);
	bool AABBBehind(const vec3 *AABBVertices);
	float AABBDistance(const vec3 *AABBVertices);
};

// ----------------------------------------------------------------------------------------------------------------------------

class CFrustum
{
private:
	vec3 Vertices[8];

private:
	CPlane Planes[6];

public:
	CFrustum();
	~CFrustum();

public:
	void Set(const mat4x4 &ViewProjectionMatrixInverse);
	bool AABBVisible(const vec3 *AABBVertices);
	float AABBDistance(const vec3 *AABBVertices);
	void Render();
};

// ----------------------------------------------------------------------------------------------------------------------------

class CCamera
{
public:
	vec3 X, Y, Z, Position, Reference;

public:
	mat4x4 ViewMatrix, ViewMatrixInverse, ProjectionMatrix, ProjectionMatrixInverse, ViewProjectionMatrix, ViewProjectionMatrixInverse;

public:
	CFrustum Frustum;

public:
	CCamera();
	~CCamera();

public:
	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void Move(const vec3 &Movement);
	vec3 OnKeys(BYTE Keys, float FrameTime);
	void OnMouseMove(int dx, int dy);
	void OnMouseWheel(float zDelta);
	void SetPerspective(float fovy, float aspect, float n, float f);

private:
	void CalculateViewMatrix();
};

// ----------------------------------------------------------------------------------------------------------------------------

class CVertex
{
public:
	vec3 Position;
	vec3 Normal;
};

// ----------------------------------------------------------------------------------------------------------------------------

class CAABB
{
private:
	vec3 Vertices[8];

public:
	CAABB();
	~CAABB();

public:
	void Set(const vec3 &Min, const vec3 &Max);
	bool PointInside(const vec3 &Point);
	bool Visible(CFrustum &Frustum);
	float Distance(CFrustum &Frustum);
	void Render();
};

// ----------------------------------------------------------------------------------------------------------------------------

class CBSPTreeNode
{
private:
	vec3 Min, Max;

private:
	int Depth;

private:
	CAABB AABB;

private:
	bool Visible;
	float Distance;

private:
	int *Indices;

private:
	int IndicesCount;

private:
	GLuint IndexBufferObject;

private:
	CBSPTreeNode *Children[2];

public:
	CBSPTreeNode();
	~CBSPTreeNode();

private:
	void SetDefaults();

public:
	void InitAABB(const vec3 &Min, const vec3 &Max, int Depth, float MinAABBSize);
	bool CheckTriangle(CVertex *Vertices, int *Indices, int A, int B, int C);
	void AllocateMemory();
	bool AddTriangle(CVertex *Vertices, int *Indices, int A, int B, int C);
	void ResetAABB(CVertex *Vertices);
	int InitIndexBufferObject();
	int CheckVisibility(CFrustum &Frustum, CBSPTreeNode **VisibleGeometryNodes, int &VisibleGeometryNodesCount);
	float GetDistance();
	void Render();
	void RenderAABB(int Depth);
	void Destroy();
};

// ----------------------------------------------------------------------------------------------------------------------------

class CBSPTree
{
private:
	CBSPTreeNode *Root;

private:
	CBSPTreeNode **VisibleGeometryNodes;
	int VisibleGeometryNodesCount;

public:
	CBSPTree();
	~CBSPTree();

private:
	void SetDefaults();

public:
	void Init(CVertex *Vertices, int *Indices, int IndicesCount, const vec3 &Min, const vec3 &Max, float MinAABBSize = 16.0f);
	void QuickSortVisibleGeometryNodes(int Left, int Right);
	int CheckVisibility(CFrustum &Frustum, bool SortVisibleGeometryNodes);
	void Render(bool VisualizeRenderingOrder);
	void RenderAABB(int Depth);
	void Destroy();
};

// ----------------------------------------------------------------------------------------------------------------------------

class CTerrain
{
private:
	int Size, SizeP1;
	float SizeD2;

private:
	vec3 Min, Max;

private:
	float *Heights;

private:
	int VerticesCount;

private:
	GLuint VertexBufferObject;

public:
	CBSPTree BSPTree;

public:
	CTerrain();
	~CTerrain();

private:
	void SetDefaults();

public:
	//bool LoadTexture2D(char *FileName, float Scale = 256.0f, float Offset = -128.0f);
	bool LoadBinary(char *FileName);
	bool SaveBinary(char *FileName);
	int CheckVisibility(CFrustum &Frustum, bool SortVisibleGeometryNodes = true);
	void Render(bool VisualizeRenderingOrder = false);
	void RenderAABB(int Depth = -1);
	void Destroy();

public:
	vec3 GetMin();
	vec3 GetMax();

private:
	int GetIndex(int X, int Z);
	float GetHeight(int X, int Z);

public:
	float GetHeight(float X, float Z);

private:
	float GetHeight(float *Heights, int Size, float X, float Z);
};

class Culling : public State, public MouseEventListener {

public:

	Culling(StateMachine& machine);
	~Culling();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void applyTransformation(TrackBall& arc);
	void renderUi();

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;
	bool m_initUi = true;

	CTerrain m_terrain;
	CCamera m_camera2;
	void CalculateProjectionMatrix();
	void CheckCameraTerrainPosition(vec3 &Movement);
	mat4x4 ViewMatrix, ProjectionMatrix;

	bool Wireframe, RenderAABB, RenderTree2D, VisualizeRenderingOrder, SortVisibleGeometryNodes, VisibilityCheckingPerformanceTest;
	int Depth;
};

