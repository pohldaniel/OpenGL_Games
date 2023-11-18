#include <iostream>
#include "JellyHelper.h"

JellyHellper* JellyHellper::_helper = 0;

JellyHellper::JellyHellper(){
	m_colorShader = 0;
	m_textureShader = 0;
}

void JellyHellper::LoadShaders(){
	//load color shader
	m_colorShader = new Shader("Assets/Shader/simple_color_transform.vert", "Assets/Shader/simple_color_transform.frag");

	//load texture shader
	m_textureShader = new Shader("Assets/Shader/sprite.vert", "Assets/Shader/sprite.frag");
}

JellyHellper* JellyHellper::Instance(){
	if (_helper == 0){
		_helper = new JellyHellper();
	}

	return _helper;
}

void JellyHellper::UpdateLines(Mesh* vertexArray, std::vector<PointMass> &pointMasses, bool create){
	unsigned int vertsCount = pointMasses.size();
	unsigned int indicesCount = pointMasses.size() * 2;

	float* _simpleData = 0;
	unsigned short* _indices = 0;

	if (create){
		vertexArray->createVertices(vertsCount);
		vertexArray->createIndices(indicesCount);

		_simpleData = static_cast<float*>(vertexArray->getVertices());
		_indices = static_cast<unsigned short*>(vertexArray->getIndices());
	}else{
		//_simpleData = new SimpleVertex[vertsCount];
        _simpleData = static_cast<float*>(vertexArray->getVertices());
	}

	//update verts
	for (unsigned int i = 0; i < vertsCount; i++){
		_simpleData[i * 3 + 0] = pointMasses[i].Position.X;
		_simpleData[i * 3 + 1] = pointMasses[i].Position.Y;
		_simpleData[i * 3 + 2] = 0.0f;
	}

	//create indices
	if (create){
		int counter = 0;

		for (unsigned int i = 0; i < pointMasses.size(); i++){
			if (i == 0){
				_indices[counter] = i;
				counter++;
			}else{
				_indices[counter] = i;
				counter++;
				_indices[counter] = i;
				counter++;
			}
		}

		_indices[counter] = 0;
	}


	//update or create
	if (create){
		vertexArray->createBuffer(false);
	}else{
		vertexArray->updateVertices(_simpleData, vertsCount, false);
	}
}

void JellyHellper::DrawLines(Mesh* vertexArray, Matrix4f& proj, Vector4f& color) {
	if (m_colorShader != 0 && vertexArray != 0){
		m_colorShader->use();

		m_colorShader->loadMatrix("translation", proj);

		m_colorShader->loadVector("colors", color);

		vertexArray->drawRaw();
	}
}

void JellyHellper::UpdateSpringShape(Mesh* vertexArray, std::vector<PointMass> &pointMasses, int *mIndices, int mIndicesCount, bool create)
{
	unsigned int vertsCount = pointMasses.size();
	int indicesCount = mIndicesCount;

	float* _simpleData = 0;
	unsigned short* _indices = 0;

	if (create)
	{
		vertexArray->createVertices(vertsCount);
		vertexArray->createIndices(indicesCount);

		_simpleData = static_cast<float*>(vertexArray->getVertices());
		_indices = static_cast<unsigned short*>(vertexArray->getIndices());
	}
	else
	{
		//_simpleData = new SimpleVertex[vertsCount];
        _simpleData = static_cast<float*>(vertexArray->getVertices());
	}

	//update verts
	for (unsigned int i = 0; i < vertsCount; i++)
	{
		_simpleData[i * 3] = pointMasses[i].Position.X;
		_simpleData[i * 3 + 1] = pointMasses[i].Position.Y;
		_simpleData[i * 3 + 2] = 0.0f;
	}

	//create indices
	if (create)
	{
		for (int i = 0; i < indicesCount; i++)
		{
			_indices[i] = mIndices[i];
		}
	}

	//update or create
	if (create)
	{
		vertexArray->createBuffer(false);
	}
	else
	{
		vertexArray->updateVertices(_simpleData, vertsCount, false);
	}
}

void JellyHellper::UpdateBlobShape(Mesh* vertexArray, std::vector<Vector2> &points, int count, bool create)
{
	unsigned int vertsCount = points.size();
	unsigned int indicesCount = points.size();

	float* _simpleData = 0;
	unsigned short* _indices = 0;

	if (create)
	{
		vertexArray->createVertices(vertsCount);
		vertexArray->createIndices(indicesCount);

		_simpleData = static_cast<float*>(vertexArray->getVertices());
		_indices = static_cast<unsigned short*>(vertexArray->getIndices());
	}
	else
	{
		//_simpleData = new SimpleVertex[vertsCount];
        _simpleData = static_cast<float*>(vertexArray->getVertices());
	}

	//update verts
	for (unsigned int i = 0; i < vertsCount; i++)
	{
		_simpleData[i * 3] = points[i].X;
		_simpleData[i * 3 + 1] = points[i].Y;
		_simpleData[i * 3 + 2] = 0.0f;
	}

	//create indices
	if (create)
	{
		for (unsigned int i = 0; i < indicesCount; i++)
		{
			_indices[i] = i;
		}
	}

	//update or create
	if (create)
	{
		vertexArray->createBuffer(false);
	}
	else
	{
		vertexArray->updateVertices(_simpleData, vertsCount, false);
	}
}

void JellyHellper::DrawShape(Mesh* vertexArray, Matrix4f& proj, Vector4f& color){

	if (m_colorShader != 0 && vertexArray != 0){
		m_colorShader->use();

		m_colorShader->loadMatrix("translation", proj);
		m_colorShader->loadVector("colors", color);

		vertexArray->drawRaw();
	}
}

std::vector<Vector2> JellyHellper::GetTexturePositions(const AABB& aabb, const Body::PointMassList& vector){
	std::vector<Vector2> positions;

	float width = aabb.Max.X - aabb.Min.X;// fabsf() + fabsf();
	float height = aabb.Max.Y - aabb.Min.Y;

	for (size_t i = 0; i < vector.size(); ++i){
		float newPosX = fabsf(vector[i].Position.X - aabb.Min.X);
		float newPosY = fabsf(vector[i].Position.Y - aabb.Min.Y);

		float pointX = newPosX / width;
		float pointY = 1.0f - (newPosY / height);

		positions.push_back(Vector2(pointX, pointY));
	}

	return positions;
}

void JellyHellper::UpdateTextured(Mesh* vertexArray, std::vector<PointMass> &pointMasses, std::vector<Vector2> &mTextureList, int *mIndices, int mIndicesCount, bool create){
	unsigned int vertsCount = pointMasses.size();
	int indicesCount = mIndicesCount;

	float* _simpleData = 0;
	unsigned short* _indices = 0;

	if (create){
		vertexArray->createVertices(vertsCount);
		vertexArray->createIndices(indicesCount);

		_simpleData = static_cast<float*>(vertexArray->getVertices());
		_indices = static_cast<unsigned short*>(vertexArray->getIndices());
	}else{
		//_simpleData = new TextureVertex[vertsCount];
        _simpleData = static_cast<float*>(vertexArray->getVertices());
	}

	//update verts
	for (unsigned int i = 0; i < vertsCount; i++){
		_simpleData[i * 5] = pointMasses[i].Position.X;
		_simpleData[i * 5 + 1] = pointMasses[i].Position.Y;
		_simpleData[i * 5 + 2] = 0.0f;

		_simpleData[i * 5 + 3] = mTextureList[i].X;
		_simpleData[i * 5 + 4] = mTextureList[i].Y;
	}

	//create indices
	if (create){
		for (int i = 0; i < indicesCount; i++){
			_indices[i] = mIndices[i];
		}
	}


	//update or create
	if (create){
		vertexArray->createBuffer(false);
	}else{
		vertexArray->updateVertices(_simpleData, vertsCount, false);
	}
}

void JellyHellper::UpdateTexturedBlob(Mesh* vertexArray, std::vector<Vector2> &points, int count, std::vector<Vector2> &mTetxure, bool create){
	unsigned int vertsCount = points.size();
	unsigned int indicesCount = points.size();

	float* _simpleData = 0;
	unsigned short* _indices = 0;

	if (create){
		vertexArray->createVertices(vertsCount);
		vertexArray->createIndices(indicesCount);

		_simpleData = static_cast<float*>(vertexArray->getVertices());
		_indices = static_cast<unsigned short*>(vertexArray->getIndices());
	}else{
		//_simpleData = new TextureVertex[vertsCount];
		_simpleData = static_cast<float*>(vertexArray->getVertices());
	}

	//update verts
	for (unsigned int i = 0; i < vertsCount; i++){
		_simpleData[i * 5] = points[i].X;
		_simpleData[i * 5 + 1] = points[i].Y;
		_simpleData[i * 5 + 2] = 0.0f;

		_simpleData[i * 5 + 3] = mTetxure[i].X;
		_simpleData[i * 5 + 4] = mTetxure[i].Y;
	}

	//create indices
	if (create){
		for (unsigned int i = 0; i < indicesCount; i++){
			_indices[i] = i;
		}
	}

	//update or create
	if (create){
		vertexArray->createBuffer(false);
	}else{
		vertexArray->updateVertices(_simpleData, vertsCount, false);
	}
}

void JellyHellper::DrawTextured(Mesh* vertexArray, Matrix4f& proj, Texture* texture, Vector4f& color) {
	if (m_textureShader != 0 && vertexArray != 0){
		//bind texture
		texture->bind(0);

		//bind shader
		m_textureShader->use();

		//set uniforms
		m_textureShader->loadMatrix("translation", proj);
		m_textureShader->loadVector("colors", color);

		vertexArray->drawRaw();
	}
}

