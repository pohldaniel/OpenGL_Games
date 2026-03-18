#ifndef __zoomableQuadH__
#define __zoomableQuadH__

#include "Vector.h"

class ZoomableQuad {

public:

	ZoomableQuad();
	ZoomableQuad(const Vector3f& position, const Vector2f& size);
	~ZoomableQuad();

	void drawRaw();
	void setPosition(const Vector3f& position);
	void setSize(const Vector2f& size);
	void mapBuffer(float offsetX, float offsetY, float width, float height);
	void mapBuffer(float zoom, float originX = 0.5f, float originY = 0.5f);
	void mapBuffer();
	void setZoom(float zoom, float originX = 0.5f, float originY = 0.5f);

	const Vector3f& getPosition() const;
	const Vector2f& getSize() const;
	const float getZoom() const;

private:
	
	int m_uResolution;
	int m_vResolution;
	float m_zoom = 1.0f;

	Vector3f m_position;
	Vector2f m_size;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;

	void createBuffer();
};

#endif