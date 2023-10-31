#include <_Andromeda/RenderManager.h>
#include <_Andromeda/ShaderManager.h>
#include <_Andromeda/Shader2.h>
#include <_Andromeda/VertexTypes.h>

#include "JellyPhysics/JellyPhysics.h"

using namespace JellyPhysics;

class JellyHellper
{
private:

	VertexArrayObject* _arrayObject;

	// color shader
	Shader2* _colorShader;

	//texture shader
	Shader2* _textureShader;

	static JellyHellper* _helper;

	JellyHellper();

public:

	static JellyHellper* Instance();

	void LoadShaders();

	//updating and drawing lines
	void UpdateLines(VertexArrayObject* vertexArray, std::vector<PointMass> &pointMasses,bool create);
	void DrawLines(VertexArrayObject* vertexArray, glm::mat4 &proj, glm::vec4 &color);

	void UpdateSpringShape(VertexArrayObject* vertexArray, std::vector<PointMass> &pointMasses, int *mIndices, int mIndicesCount, bool create);
	void UpdateBlobShape(VertexArrayObject* vertexArray, std::vector<Vector2> &points, int count, bool create);
	void DrawShape(VertexArrayObject* vertexArray, glm::mat4 &proj, glm::vec4 &color);

	//textured points
	std::vector<Vector2> GetTexturePositions(const AABB& aabb, const Body::PointMassList& vector);

	void UpdateTexturedBlob(VertexArrayObject* vertexArray, std::vector<Vector2> &points, int count, std::vector<Vector2> &mTetxure, bool create);
	void UpdateTextured(VertexArrayObject* vertexArray, std::vector<PointMass> &pointMasses, std::vector<Vector2> &mTextureList, int *mIndices, int mIndicesCount, bool create);
	void DrawTextured(VertexArrayObject* vertexArray, glm::mat4 &proj, Texture2* texture, glm::vec4 &color);
	
};