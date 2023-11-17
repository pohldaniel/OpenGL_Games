#ifndef GameSpringBody_H
#define GameSpringBody_H

#include <engine/Rect.h>
#include <engine/Texture.h>
#include <engine/Vector.h>

#include "JellyPhysics/JellyPhysics.h"
#include "Mesh.h"

namespace JellyPhysics
{
	class GameSpringBody : public SpringBody
	{
	private:

		Mesh* _vertexObject;
		Mesh* _shapeObject;
		Vector4f _color;
		Vector4f _lineColor;
		bool _created;
	
		std::vector<Vector2> _textPositions;

		Texture* m_texture;
		TextureRect m_textureRect;

	public:

		GameSpringBody(World* w, const ClosedShape& shape, float massPerPoint,
			float edgeSpringK, float edgeSpringDamp,
			const Vector2& pos, float angleInRadians,
			const Vector2& scale, bool kinematic);

		GameSpringBody(World* w, const ClosedShape& shape, float massPerPoint,
			float shapeSpringK, float shapeSpringDamp,
			float edgeSpringK, float edgeSpringDamp,
			const Vector2& pos, float angleinRadians,
			const Vector2& scale, bool kinematic);

		~GameSpringBody();

		void accumulateExternalForces();
		void accumulateInternalForces();

		void SetTextureRect(const TextureRect& rect);
		void SetTexture(Texture* texture);
		void SetLineColor(Vector4f color);

		void Draw(Matrix4f& proj, int *mIndices, int mIndicesCount, float  R, float  G, float B);
	};
}


#endif

