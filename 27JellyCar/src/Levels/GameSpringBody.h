#ifndef GameSpringBody_H
#define GameSpringBody_H

#include <engine/Rect.h>
#include <engine/Texture.h>
#include <_Andromeda/RenderManager.h>

#include "JellyPhysics/JellyPhysics.h"

namespace JellyPhysics
{
	class GameSpringBody : public SpringBody
	{
	private:

		VertexArrayObject* _vertexObject;
		VertexArrayObject* _shapeObject;
		glm::vec4 _color;
		glm::vec4 _lineColor;
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
		void SetLineColor(glm::vec4 color);

		void Draw(glm::mat4 &proj, int *mIndices, int mIndicesCount, float  R, float  G, float B);
	};
}


#endif

