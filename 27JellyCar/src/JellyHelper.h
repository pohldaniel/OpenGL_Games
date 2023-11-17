#include <engine/Texture.h>
#include <engine/Shader.h>
#include "JellyPhysics/JellyPhysics.h"

#include "Mesh.h"

using namespace JellyPhysics;

class JellyHellper{

private:

	// color shader
	Shader* m_colorShader;

	//texture shader
	Shader* m_textureShader;

	static JellyHellper* _helper;

	JellyHellper();

public:

	static JellyHellper* Instance();

	void LoadShaders();

	//updating and drawing lines
	void UpdateLines(Mesh* vertexArray, std::vector<PointMass> &pointMasses,bool create);
	void DrawLines(Mesh* vertexArray, Matrix4f& proj, Vector4f& color);

	void UpdateSpringShape(Mesh* vertexArray, std::vector<PointMass> &pointMasses, int *mIndices, int mIndicesCount, bool create);
	void UpdateBlobShape(Mesh* vertexArray, std::vector<Vector2> &points, int count, bool create);
	void DrawShape(Mesh* vertexArray, Matrix4f& proj, Vector4f& color);

	//textured points
	std::vector<Vector2> GetTexturePositions(const AABB& aabb, const Body::PointMassList& vector);

	void UpdateTexturedBlob(Mesh* vertexArray, std::vector<Vector2> &points, int count, std::vector<Vector2> &mTetxure, bool create);
	void UpdateTextured(Mesh* vertexArray, std::vector<PointMass> &pointMasses, std::vector<Vector2> &mTextureList, int *mIndices, int mIndicesCount, bool create);
	void DrawTextured(Mesh* vertexArray, Matrix4f& proj, Texture* texture, Vector4f& color);
};