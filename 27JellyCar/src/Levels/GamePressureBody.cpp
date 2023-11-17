#include "GamePressureBody.h"
#include "../JellyHelper.h"

GamePressureBody::GamePressureBody(World* w, const ClosedShape& s, float mpp, float gasPressure, float shapeK, float shapeD,
	float edgeK, float edgeD, const Vector2& pos, float angleInRadians, const Vector2& scale, bool kinematic) :
	PressureBody(w, s, mpp, gasPressure, shapeK, shapeD, edgeK, edgeD, pos, angleInRadians, scale, kinematic)
{
	dragPoint = -1;

	_vertexObject = new Mesh();
	_vertexObject->setVertexBufferDrawType(_DynamicDraw);
	_vertexObject->setVertexType(_Simple);
	_vertexObject->setVertexPrimitive(_Lines);

	_shapeObject = 0;

	_color = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
	_lineColor = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

	_created = false;

	shape.resize(mPointMasses.size() + 2);

	m_texture = nullptr;
}

GamePressureBody::~GamePressureBody()
{
	delete _vertexObject;
	delete _shapeObject;
}

void GamePressureBody::accumulateInternalForces()
{
	if (!mIsStatic)
	{
		PressureBody::accumulateInternalForces();
	}
}

void GamePressureBody::accumulateExternalForces()
{
	if (!mIsStatic)
	{
		PressureBody::accumulateExternalForces();

		for (unsigned int i = 0; i < mPointMasses.size(); i++)
			mPointMasses[i].Force += Vector2(0.0f, -9.8f * mPointMasses[i].Mass);

		if (dragPoint != -1)
			mPointMasses[dragPoint].Force += dragForce;

		dragPoint = -1;
	}
}

void GamePressureBody::SetTextureRect(const TextureRect& rect) {
	m_textureRect = rect;
}

void GamePressureBody::SetTexture(Texture* texture)
{
	m_texture = texture;
}


void GamePressureBody::SetLineColor(Vector4f color)
{
	_lineColor = color;
}

void GamePressureBody::Draw(Matrix4f& proj, int *mIndices, int mIndicesCount, float  R, float  G, float B)
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
		}
		else
		{
			//color fill
			_shapeObject = new Mesh();
			_shapeObject->setVertexBufferDrawType(_DynamicDraw);
			_shapeObject->setVertexType(_Simple);
		}


		JellyHellper::Instance()->UpdateLines(_vertexObject, mPointMasses, true);
		JellyHellper::Instance()->UpdateSpringShape(_shapeObject, mPointMasses, mIndices, mIndicesCount, true);
		
		_created = true;
	}
	else if (!mIsStatic)
	{
		JellyHellper::Instance()->UpdateLines(_vertexObject, mPointMasses, false);
		JellyHellper::Instance()->UpdateSpringShape(_shapeObject, mPointMasses, mIndices, mIndicesCount, false);
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