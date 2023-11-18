#include "GameSpringBody.h"
#include "JellyHelper.h"

GameSpringBody::GameSpringBody(World* w, const ClosedShape& shape, float massPerPoint,
	float edgeSpringK, float edgeSpringDamp,
	const Vector2& pos, float angleInRadians,
	const Vector2& scale, bool kinematic) :
	SpringBody(w, shape, massPerPoint, edgeSpringK, edgeSpringDamp, pos, angleInRadians, scale, kinematic)
{
	dragPoint = -1;

	//lines
	_vertexObject = new Mesh();
	_vertexObject->setVertexBufferDrawType(_DynamicDraw);
	_vertexObject->setVertexType(_Simple);
	_vertexObject->setVertexPrimitive(_Lines);

	_shapeObject = 0;

	_color = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
	_lineColor = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

	_created = false;

	m_texture = nullptr;
}

GameSpringBody::GameSpringBody(World* w, const ClosedShape& shape, float massPerPoint,
	float shapeSpringK, float shapeSpringDamp,
	float edgeSpringK, float edgeSpringDamp,
	const Vector2& pos, float angleinRadians,
	const Vector2& scale, bool kinematic) :
	SpringBody(w, shape, massPerPoint, shapeSpringK, shapeSpringDamp, edgeSpringK, edgeSpringDamp, pos, angleinRadians, scale, kinematic)
{
	dragPoint = -1;

	//lines
	_vertexObject = new Mesh();
	_vertexObject->setVertexBufferDrawType(_DynamicDraw);
	_vertexObject->setVertexType(_Simple);
	_vertexObject->setVertexPrimitive(_Lines);

	_shapeObject = 0;

	_color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	_lineColor = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

	_created = false;

	m_texture = nullptr;
}

GameSpringBody::~GameSpringBody()
{
	delete _vertexObject;
	delete _shapeObject;
}

void GameSpringBody::accumulateExternalForces()
{
	if (!mIsStatic)
	{
		SpringBody::accumulateExternalForces();

		for (unsigned int i = 0; i < mPointMasses.size(); i++)
			mPointMasses[i].Force += Vector2(0.0f, -9.8f * mPointMasses[i].Mass);

		if (dragPoint != -1)
			mPointMasses[dragPoint].Force += dragForce;

		dragPoint = -1;
	}
}

void GameSpringBody::accumulateInternalForces()
{
	if (!mIsStatic)
	{
		SpringBody::accumulateInternalForces();
	}
}

void GameSpringBody::SetTextureRect(const TextureRect& rect)
{
	m_textureRect = rect;
}

void GameSpringBody::SetTexture(Texture* texture)
{
	m_texture = texture;
}

void GameSpringBody::SetLineColor(Vector4f color)
{
	_lineColor = color;
}

void GameSpringBody::Draw(Matrix4f& proj, int *mIndices, int mIndicesCount, float  R, float  G, float B)
{
	if (!_created)
	{
		if (m_texture != 0)
		{
			//generate texture positions
			_textPositions = JellyHellper::Instance()->GetTexturePositions(getAABB(), mPointMasses);

			//texture fill
			_shapeObject = new Mesh();
			_shapeObject->setVertexBufferDrawType(_DynamicDraw);
			_shapeObject->setVertexType(_Textured);

			//generate object
			JellyHellper::Instance()->UpdateTextured(_shapeObject, mPointMasses, _textPositions, mIndices, mIndicesCount, true);
		}
		else
		{
			//color fill
			_shapeObject = new Mesh();
			_shapeObject->setVertexBufferDrawType(_DynamicDraw);
			_shapeObject->setVertexType(_Simple);

			//generate object
			JellyHellper::Instance()->UpdateSpringShape(_shapeObject, mPointMasses, mIndices, mIndicesCount, true);
		}

		JellyHellper::Instance()->UpdateLines(_vertexObject, mPointMasses, true);
		
		_created = true;
	}
	else if(!mIsStatic)
	{

		JellyHellper::Instance()->UpdateLines(_vertexObject, mPointMasses, false);

		if (m_texture != 0)
		{
			JellyHellper::Instance()->UpdateTextured(_shapeObject, mPointMasses, _textPositions, mIndices, mIndicesCount, false);
		}
		else
		{
			JellyHellper::Instance()->UpdateSpringShape(_shapeObject, mPointMasses, mIndices, mIndicesCount, false);
		}
	}

	if (m_texture != 0)
	{
		JellyHellper::Instance()->DrawTextured(_shapeObject, proj, m_texture, Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else
	{
		JellyHellper::Instance()->DrawShape(_shapeObject, proj, Vector4f(R, G, B, 0.7f));
	}
	
	//draw lines
	JellyHellper::Instance()->DrawLines(_vertexObject, proj, _lineColor);
}